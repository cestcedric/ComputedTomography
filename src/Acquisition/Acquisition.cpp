#include <limits>
#include "Acquisition/Acquisition.h"
#include <iostream>

Acquisition::Acquisition(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StaticContents);
    setWidgets();
    linkWidgets();

    volume = Volume (Eigen::Vector3f(0,0,0), Eigen::Vector3f(10,10,10), Eigen::Vector3f(1,1,1));
    volume.setContent(Eigen::VectorXf::Zero(1000));
    setPoses(posesSpinBox->value());
    result = Volume (Eigen::Vector3f(0,0,0), Eigen::Vector3f(10,10,10), Eigen::Vector3f(1,1,1));
    result.setContent(Eigen::VectorXf::Zero(25*numberOfPoses));
    loaded = false;
}

void Acquisition::setTracer(Raytracer *raytracer) {
    this->raytracer = raytracer;
    raytracer->setVolume(&volume);
}

void Acquisition::setWidgets() {
    //controls in the left column
    leftColumn = new QWidget(this);
    saveButton = new QPushButton("save");
    loadButton = new QPushButton("load");

    QGroupBox* posesChooser = new QGroupBox("number of poses");
    QVBoxLayout* posesLayout = new QVBoxLayout;
    posesSpinBox = new QSpinBox;
    posesSpinBox->setRange(1, 180);
    posesSpinBox->setValue(15);
    progress = new QProgressBar;
    progress->setRange(0, posesSpinBox->value()*2);
    progress->setValue(0);
    posesLayout->addWidget(posesSpinBox);
    posesLayout->addWidget(progress);
    posesChooser->setLayout(posesLayout);
    QGroupBox* noise = new QGroupBox("noise");
    QHBoxLayout* noiseLayout = new QHBoxLayout;
    noiseCheck = new QCheckBox("2% noise in scans");
    noiseLayout->addWidget(noiseCheck);
    noise->setLayout(noiseLayout);
    QGroupBox* levelChooser = new QGroupBox("view level");
    QHBoxLayout* levelLayout = new QHBoxLayout;
    toggle = new QComboBox();
    toggle->addItem("upper");
    toggle->addItem("lower");
    toggle->setToolTip("show upper or lower scan at this position");
    top = true;
    levelLayout->addWidget(toggle);
    levelChooser->setLayout(levelLayout);

    //show origin and direction of a specific ray
    rayPose = new QSpinBox;
    rayPose->setRange(1, posesSpinBox->value());
    rayPose->setToolTip("pose");
    rayX = new QSpinBox;
    rayX->setRange(0, 4);
    rayX->setToolTip("column");
    rayY = new QSpinBox;
    rayY->setRange(0, 4);
    rayY->setToolTip("row");

    QGroupBox* rayJay = new QGroupBox("get a ray");
    QVBoxLayout* rayLayout = new QVBoxLayout;
    QWidget* rayChooser = new QWidget;
    QHBoxLayout* rayChooserLayout = new QHBoxLayout;
    rayChooserLayout->addWidget(rayPose);
    rayChooserLayout->addWidget(rayX);
    rayChooserLayout->addWidget(rayY);
    rayChooser->setLayout(rayChooserLayout);
    shower = new QPushButton("show ray");
    rayLayout->addWidget(rayChooser);
    rayLayout->addWidget(shower);
    rayJay->setLayout(rayLayout);

    //Layout LEFT
    //includes load/save, amount of poses, upper or lower scan, show ray
    QVBoxLayout* layoutLeft = new QVBoxLayout;
    layoutLeft->addWidget(saveButton);
    layoutLeft->addWidget(loadButton);
    QLabel* emptyLine = new QLabel;
    layoutLeft->addWidget(emptyLine);
    layoutLeft->addWidget(posesChooser);
    layoutLeft->addWidget(noise);
    layoutLeft->addWidget(levelChooser);
    layoutLeft->addWidget(rayJay);
    layoutLeft->addStretch(1);
    leftColumn->setLayout(layoutLeft);
    QSizePolicy fixIt;
    fixIt.setHorizontalPolicy(QSizePolicy::Fixed);
    fixIt.setVerticalPolicy(QSizePolicy::Expanding);
    leftColumn->setSizePolicy(fixIt);

    //big button in the middle to start the simulation
    bottomButton = new QWidget;
    simulateButton = new QPushButton("simulate");
    QHBoxLayout* layoutBottom = new QHBoxLayout;
    layoutBottom->addWidget(simulateButton);
    bottomButton->setLayout(layoutBottom);
    QSizePolicy laylow;
    laylow.setVerticalPolicy(QSizePolicy::Fixed);
    laylow.setHorizontalPolicy(QSizePolicy::Expanding);
    bottomButton->setSizePolicy(laylow);

    //show pictures on the top right, menu on the left, button at the bottom
    // Layout RIGHT
    rightColumn = new QWidget(this);
    QVBoxLayout* layoutRight = new QVBoxLayout;
    poseWidget = new Pose(this);

    QTabWidget* tabs = new QTabWidget;
    QWidget* detectorImageDisplay = new QWidget(this);
    QVBoxLayout* detectorMenuLayout = new QVBoxLayout;
    islide = new QSlider(Qt::Horizontal,this);
    islide->setMaximum(0);
    detectorWidget = new DetectorWidget;
    detectorMenuLayout->addWidget(detectorWidget); // detector Image to display as 5x5 matrix
    detectorMenuLayout->addWidget(islide);
    detectorImageDisplay->setLayout(detectorMenuLayout);
    tabs->addTab(poseWidget, "poses");
    tabs->addTab(detectorImageDisplay, "detector");

    layoutRight->addWidget(tabs);// tab widget to display gantry poses and detector values
    layoutRight->addWidget(bottomButton);
    rightColumn->setLayout(layoutRight);
    QHBoxLayout* layoutMain = new QHBoxLayout;
    layoutMain->addWidget(leftColumn);
    layoutMain->addWidget(rightColumn);
    setLayout(layoutMain);

}

void Acquisition::linkWidgets() { // set Actions method would be usefull :)
    connect(loadButton, &QPushButton::clicked, this, &Acquisition::load);
    connect(saveButton, &QPushButton::clicked, this, &Acquisition::save);
    connect(simulateButton, &QPushButton::clicked, this, &Acquisition::simulate);
    connect(posesSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Acquisition::setPoses);  
    connect(islide, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Acquisition::showSimulation);
    connect(toggle, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &Acquisition::toggleSimulation);
    connect(shower, &QPushButton::clicked, this, &Acquisition::showRay);
}

void Acquisition::save() {//see simulate method for explanation for change to be done
    //save
    //emit filepath -> reconstruction automatically loads this file
    //save with explicitly typing ".edf" after fileName, need to check whether the saved file ist correct, but it looks good
    QString fileName = QFileDialog::getSaveFileName(this, QDir::currentPath(),"", "scan result (*.edf)");
    try
    {
        EDFHandler::write(fileName.toUtf8().constData(), result);//saves the scan results, not the volume
        emit outputFilePath(fileName);
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Acquisition::load() {
    //loads the edf file
    QString fileName = QFileDialog::getOpenFileName(this,QDir::currentPath());
    try
    {
        volume = EDFHandler::read(fileName.toUtf8().constData());
        raytracer->setVolume(&volume);
        //save the path of the volume directory to later save the results in a subfolder automatically
        int index = fileName.size() - 1;
        while (fileName.at(index) != '/') {
            --index;
        }
        path = fileName.left(index);
        if (!AcquisitionPoses.empty()) {
            setPoses(posesSpinBox->value());
        }
        loaded = true;
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Acquisition::cmdLoad(std::string filepath) {
    QString fileName = QString::fromStdString(filepath);
    try
    {
        volume = EDFHandler::read(fileName.toUtf8().constData());
        raytracer->setVolume(&volume);
        //save the path of the volume directory to later save the results in a subfolder automatically
        int index = fileName.size() - 1;
        while (fileName.at(index) != '/') {
            --index;
        }
        path = fileName.left(index);
        if (!AcquisitionPoses.empty()) {
            setPoses(posesSpinBox->value());
        }
        loaded = true;
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void Acquisition::setPoses(int amount) {
    numberOfPoses = amount;
    progress->setRange(0, numberOfPoses*2);
    progress->setValue(0);
    AcquisitionPoses.clear();
    for(int a = 0; a < amount; a++)
    {
        //HEIGHT SET RANDOMLY, NO REASON NOT TO PUT THEM HIGHER OR LOWER
        //lower half of the object
        AcquisitionPoses.push_back(Gantry(Eigen::Vector3f(0.075,0.075,0.075),M_PI/amount*a,0.22,0.2,5)); // 1 = 1m s2dd ....
        //second pose with same angle to scan the upper half
        AcquisitionPoses.push_back(Gantry(Eigen::Vector3f(0.075,0.075,0.175),M_PI/amount*a,0.22,0.2,5));
    }
    poseWidget->setNumPoses(amount);
    islide->setMaximum(amount - 1);
    rayPose->setMaximum(amount);
    update();
}


//actual methods that calculate things

void Acquisition::simulate() {
    //refresh gantries that may already have been exposed
    setPoses(numberOfPoses);
    progress->setValue(0);
    //make forward projection
    forwardProjection();
    showSimulation(0);//show first detector, not test image
    //connect the detectors of the gantries and create a new fake volume (not actually a volume)
        //-> data of new Volume is a vector of all of the screens concatenated
        //-> size of this vector = 25 * numberOfPoses * 2 <- top and bottom part of scan counted as 1 pose
    result = Volume(volume.getBoundingBox().min(), Eigen::Vector3f(0.075, 0.075, 0.05*numberOfPoses), volume.getSpacing());
    Eigen::VectorXf screens(25*2*numberOfPoses);
    std::list<Gantry>::iterator it = AcquisitionPoses.begin();
    int i{0};//index of resulting concatenation
    for (std::list<Gantry>::iterator it = AcquisitionPoses.begin(), end = AcquisitionPoses.end(); it != end; ++it) {
        for (int k{0}; k < 25; ++k) {
            int y = k/5; // get the y coordinate of the detector at current position
            int x = k%5; // get the x coordinate of the detector at current position
            screens(i) = it->getDetector()(x,y);
            ++i;
        }
    }
    result.setContent(screens);
    //autosave the results
    if (loaded) {
        autosave();
    }   else    {
        ask();
    }
    //-> in Reconstruction: automatically load this fake volume
}

void Acquisition::autosave() {
    QString fileName = path + "/scan results";
    QDir dir(fileName);
    if (!dir.exists()) {//create new results directory in the directory where the volumes are stored
        dir.mkpath(".");
    }
    QString timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm-ss");
    fileName = fileName + "/scan " + timestamp + ".edf";
    try
    {
        EDFHandler::write(fileName.toUtf8().constData(), result);//saves the scan results, not the volume
        //send filepath to Reconstruction
        emit outputFilePath(fileName);
    } catch(std::invalid_argument e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Acquisition::ask() {
    QMessageBox msgBox;
    msgBox.setText("You scanned an empty box");
    msgBox.setInformativeText("Do you want to save your results?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
        save();
    }
}

//returns a list of indexes of the voxels the ray is passing
//start = source; entry point into the volume is computed automatically
//finish = detector pixel; exit point is computed automatically
std::list<int> Acquisition::raytrace(Eigen::Vector3f start, Eigen::Vector3f finish) {
    return raytracer->trace(start, finish);
}

void Acquisition::forwardProjection(){
    // volume in the linearized format
    //Eigen::VectorXf volumeValues = volume.computeLinear();
    Eigen::VectorXf volumeValues = volume.getContent();
    
    //iterate through all positions
    int k = 0;
    for(std::list<Gantry>::iterator gantry = AcquisitionPoses.begin(), end = AcquisitionPoses.end(); gantry != end; ++gantry){
        std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> detectorPositions= gantry->getDetectorPositions();
        int j = detectorPositions.size();
        //iterating over detector-positions and calling raytrace for all rays of a acquisition pose
        for (int i=0; i<j; i++){
            std::list<int> indices = raytrace(gantry->getSourcePosition(), detectorPositions.front());
            //result of the accumulation of voxel values
            int result=0;
            while(!indices.empty()){
                //accumulating the values of the voxels hit by the ray
                result=result+volumeValues[indices.front()];
                indices.pop_front();
            }
            //adding the result to the detector
            int y = i/5; // get the y coordinate of the detector at current position
            int x = i%5; // get the x coordinate of the detector at current position
            gantry->addValuesToDetector(x, y, result);
            if (noiseCheck->isChecked()) {//make some noise
                gantry->makeNoise();
            }
            //get the next detector-position
            detectorPositions.pop_front();
        }
        ++k;
        progress->setValue(k);
    }
}

void Acquisition::showRay() {
    std::list<Gantry>::iterator it = AcquisitionPoses.begin();
    for (int i{0}; i < rayPose->value() - 1; i++) {
        ++it;
        ++it;
    }
    if (top) ++it;
    int pixel = rayX->value() + rayY->value()*5;
    Eigen::ParametrizedLine<float, 3> ray = it->getRay(pixel);
    QMessageBox box;
    //box.setText("this yo ray fam");
    QString sourceText("source       ");
    sourceText = sourceText + QString("(") + QString::number(ray.origin()(0)) + QString(", ");
    sourceText = sourceText + QString::number(ray.origin()(1)) + QString(", ");
    sourceText = sourceText + QString::number(ray.origin()(2)) + QString(")");
    QString directionText("direction   ");
    directionText = directionText + QString("(") + QString::number(ray.direction()(0)) + QString(", ");
    directionText = directionText + QString::number(ray.direction()(1)) + QString(", ");
    directionText = directionText + QString::number(ray.direction()(2)) + QString(")");
    QString boxText = sourceText + "\n" + directionText;
    box.setInformativeText(boxText);
    box.exec();
}

void Acquisition::showPositions() {
    QPixmap* poseIM = new QPixmap(800,600);
    poseIM->fill(Qt::white);
    img = poseIM->toImage();
    update();
}

//shows simulated result
void Acquisition::showSimulation(int index) {
    std::list<Gantry>::iterator it = AcquisitionPoses.begin();
    for (int i{0}; i < index; i++) {
        ++it;
        ++it;
    }
    if (top) ++it;
    detectorWidget->setDetectorMap(it->getDetector());
}

void Acquisition::toggleSimulation(int b) {
    if (b == 0) top = true;
    else top = false;
    showSimulation(islide->value());
}

void Acquisition::paintEvent(QPaintEvent *event) {
    QPainter ptr(this);
    QRect screen = event->rect();//was done like this in a qt example
    QImage img;
    ptr.drawImage(screen, img, screen);
}

QSize Acquisition::minimumSizeHint() const {
    return (QSize(900, 600));
}
