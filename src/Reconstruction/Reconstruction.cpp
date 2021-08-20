#include "Reconstruction/Reconstruction.h"
#include <iostream>

Reconstruction::Reconstruction(QWidget * parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setWidgets();
    linkWidgets();

    //initialize everything
    setPoses(1);
    //empty volume to start conjugate gradient
    volume = Volume(Eigen::Vector3f(0,0,0), Eigen::Vector3f(0.15,0.15,0.25), Eigen::Vector3f(0.015,0.015,0.025));
    volume.setContent(Eigen::VectorXf::Zero(1000));
    //automatically changed when scanning new volume
    results = Volume(Eigen::Vector3f(0,0,0), Eigen::Vector3f(0.75,0.75,0.05), Eigen::Vector3f(0.015,0.015,0.025));
    results.setContent(Eigen::VectorXf::Zero(50));
    loaded = false;
}

void Reconstruction::setTracer(Raytracer *raytracer) {
    this->raytracer = raytracer;
}


void Reconstruction::setWidgets()
{
    leftColumn = new QWidget(this);
    saveButton = new QPushButton("save");
    loadButton = new QPushButton("load");

    //elements to change parameters for reconstruction
    QWidget* parameters = new QWidget;
    QVBoxLayout* paramLayout = new QVBoxLayout;
    QGroupBox* iter = new QGroupBox("iterations");
    QVBoxLayout* iterLayout = new QVBoxLayout;
    iterations = new QSpinBox;
    iterations->setRange(1, 10000);//just guessing
    iterations->setValue(150);
    progress = new QProgressBar;
    progress->setRange(0, iterations->value());
    progress->setValue(0);
    iterLayout->addWidget(iterations);
    iterLayout->addWidget(progress);
    iter->setLayout(iterLayout);
    QGroupBox* recon = new QGroupBox("reconstruction");
    QVBoxLayout* reconLayout = new QVBoxLayout;
    QWidget* check = new QWidget;
    QHBoxLayout* checkLayout = new QHBoxLayout;
    regularized = new QCheckBox;
    regularized->setToolTip("reconstruction with regularization");
    QLabel* regLabel = new QLabel("regularize");
    checkLayout->addWidget(regLabel);
    checkLayout->addWidget(regularized);
    check->setLayout(checkLayout);
    QWidget* lambdaParam = new QWidget;
    QHBoxLayout* lambdaLayout = new QHBoxLayout;
    QLabel* lambdaLabel = new QLabel("lambda");
    lambda = new QDoubleSpinBox;
    lambda->setRange(0, 10);//still guessing
    lambda->setToolTip("set lambda");
    lambdaLayout->addWidget(lambdaLabel);
    lambda->setValue(1);
    lambdaLayout->addWidget(lambda);
    lambdaParam->setLayout(lambdaLayout);
    reconLayout->addWidget(check);
    reconLayout->addWidget(lambdaParam);
    recon->setLayout(reconLayout);
    paramLayout->addWidget(iter);
    paramLayout->addWidget(recon);
    parameters->setLayout(paramLayout);


    //layout left -> buttons and other boxes
    QVBoxLayout* layoutLeft = new QVBoxLayout;
    saveButton->setMinimumSize(100,10);// minimum size for the buttons
    loadButton->setMinimumSize(100,10);
    layoutLeft->addWidget(saveButton);
    layoutLeft->addWidget(loadButton);
    layoutLeft->addWidget(parameters);
    layoutLeft->addStretch(1);
    leftColumn->setLayout(layoutLeft);
    QSizePolicy fixIt;
    fixIt.setHorizontalPolicy(QSizePolicy::Fixed);
    fixIt.setVerticalPolicy(QSizePolicy::Expanding);
    leftColumn->setSizePolicy(fixIt);

    // big ass button on the bottom
    bottomButton = new QWidget;
    reconstructButton = new QPushButton("reconstruct");
    QHBoxLayout* layoutBottom = new QHBoxLayout;
    layoutBottom->addWidget(reconstructButton);
    bottomButton->setLayout(layoutBottom);
    QSizePolicy laylow;
    laylow.setVerticalPolicy(QSizePolicy::Fixed);
    laylow.setHorizontalPolicy(QSizePolicy::Expanding);
    bottomButton->setSizePolicy(laylow);

    //layout right -> 2D-Slice-Viewer with Slider and reconstruct button
    rightColumn = new QWidget(this);
    QWidget* twoDsliceViewer = new QWidget(this);
    QVBoxLayout* layoutRight = new QVBoxLayout;
    layoutRight->addWidget(twoDsliceViewer);
    layoutRight->addWidget(bottomButton);
    rightColumn->setLayout(layoutRight);

    QVBoxLayout* sliceViewNSlideLayout = new QVBoxLayout;
    sliceViewWidget = new SliceViewer;

    islide = new QSlider(Qt::Horizontal,this);
    islide->setRange(0,9);
    sliceViewNSlideLayout->addWidget(sliceViewWidget);
   // sliceViewNSlideLayout->setAlignment(sliceViewWidget, Qt::AlignHCenter); WTF Alignment?!?!
    sliceViewNSlideLayout->addWidget(islide);
    twoDsliceViewer->setLayout(sliceViewNSlideLayout);


    //setting the main layout of the widget and include the left and right column
    QHBoxLayout* layoutMain = new QHBoxLayout;
    layoutMain->addWidget(leftColumn);
    layoutMain->addWidget(rightColumn);
    setLayout(layoutMain);
}
void Reconstruction::linkWidgets()
{
    connect(loadButton, &QPushButton::clicked, this, &Reconstruction::load);
    connect(saveButton, &QPushButton::clicked, this, &Reconstruction::save);
    connect(islide, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Reconstruction::showSlice);
    connect(iterations, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), progress, &QProgressBar::setMaximum);
    connect(reconstructButton, &QPushButton::clicked, this, &Reconstruction::reconstruct);
}
void Reconstruction::showSlice(int index)
{
   if(reconstructedSlices.size() == 10)
   sliceViewWidget->setReconstructedImageMatrix(reconstructedSlices.at(index));
   sliceViewWidget->update();
}

void Reconstruction::save()
{
//emit outputFilePath
    QString fileName = QFileDialog::getSaveFileName(this,QDir::currentPath());
    try
    {
        EDFHandler::write(fileName.toUtf8().constData(),volume);
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Reconstruction::load()
{
    QString fileName = QFileDialog::getOpenFileName(this,QDir::currentPath());
    autoload(fileName);
}

void Reconstruction::autoload(const QString &fileName) {
    try {
        results = EDFHandler::read(fileName.toUtf8().constData());
        //save the path of the volume directory to later save the reconstruction in a subfolder automatically
        int index = fileName.size() - 1;
        //remove file name
        while (fileName.at(index) != '/') {
            --index;
        }
        //get out of results subfolder
        /*--index;
        while (fileName.at(index) != '/') {
            --index;
        }*/
        path = fileName.left(index);//save filepath to autosave at directory/reconstruction/*.edf
        if (!poses.empty()) {//set the poses to the same amount used for scanning this object
            int p = results.getNumVoxels().prod()/50;
            setPoses(p);
            iterations->setValue(4000/p);
        }
        loaded = true;
    }   catch (std::invalid_argument e) {
        std::cerr << e.what() << std::endl;
    }
}

void Reconstruction::autosave() {
    QString fileName = path + "/reconstruction";
    QDir dir(fileName);
    if (!dir.exists()) {//create new reconstruction directory in the directory where the results are stored
        dir.mkpath(".");
    }
    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm-ss");
    fileName = fileName + "/recon " + timestamp + ".edf";
    try
    {
        EDFHandler::write(fileName.toUtf8().constData(), volume);//reconstructed volume
        //send filepath to Visualization
        emit outputFilePath(fileName);
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Reconstruction::boiii() {//roasts user
    QMessageBox box;
    box.setWindowTitle("boiii");
    box.setInformativeText("what you think you doin wit yo empty ass volume?!");
    box.exec();
}

//conjugate gradient algorithm, using normal or regularized Reconstruction
//code from presentation, may or may not be able to optimize
//slow af
void Reconstruction::congrads() {
    int n = iterations->value();
    Eigen::VectorXf res = results.getContent();
    Eigen::VectorXf b = matrix->backProjection(&res);
    Eigen::VectorXf v = Eigen::VectorXf::Zero(1000);
    Eigen::VectorXf r = b - matrix->mult(&v);
    Eigen::VectorXf d = r;
    float rtr = r.dot(r);
    float oldRtr;
    Eigen::VectorXf q;
    float alpha;

    for (int i{0}; i < n; ++i) {
        q = matrix->mult(&d);
        alpha = rtr/(d.transpose()*q);
        v = v + alpha * d;
        r = r - alpha * q;

        oldRtr = rtr;
        rtr = r.dot(r);
        d = r + (rtr / oldRtr) * d;

        progress->setValue(i);
    }
    progress->setValue(progress->maximum());
    volume.setContent(v);
}

void Reconstruction::setPoses(int amount) {//not nice, but normalReconstruction,... did not really work with a pointer to std::list<Gantry>
    poses.clear();
    for(int a = 0; a < amount; a++)
    {
        //HEIGHT SET RANDOMLY, NO REASON NOT TO PUT THEM HIGHER OR LOWER, SAME AS IN ACQUISITION!!!
        //lower half of the object
        poses.push_back(Gantry(Eigen::Vector3f(0.075,0.075,0.075),M_PI/amount*a,0.22,0.2,5)); // 1 = 1m s2dd ....
        //second pose with same angle to scan the upper half
        poses.push_back(Gantry(Eigen::Vector3f(0.075,0.075,0.175),M_PI/amount*a,0.22,0.2,5));
    }
}

void Reconstruction::reconstruct()
{
    progress->setValue(0);
    if (regularized->isChecked()) {
        matrix = new regularizedReconstruction(raytracer, poses, lambda->value());
    }   else    {
        matrix = new normalReconstruction(raytracer, poses);
    }
    if (!loaded) {
        boiii();
    }   else    {
        congrads();
        volume.computeMatrix();
        reconstructedSlices = volume.getMatrix();//send stuff to slice viewer I guess, one slice = one matrix -> 10 slices for our cube
        autosave();
        showSlice(islide->value());
    }
}

QSize Reconstruction::minimumSizeHint() const {
    return (QSize(900, 600));
}
