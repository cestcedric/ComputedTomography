#include "Visualization/Visualization.h"
#include <iostream>
#include <thread>
#define _USE_MATH_DEFINES
#include <math.h>



Visualization::Visualization(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setWidgets();
    linkWidgets();

    volume = Volume(Eigen::Vector3f(0,0,0), Eigen::Vector3f(0.15,0.15,0.25), Eigen::Vector3f(0.015,0.015,0.025));
    volume.setContent(Eigen::VectorXf::Zero(1000));
    loaded = false;
}

void Visualization::setWidgets()
{
    leftColumn = new QWidget(this);
//    saveButton = new QPushButton("save");
    loadButton = new QPushButton("load");


    QWidget* params = new QWidget;
    QVBoxLayout* paramsLayout = new QVBoxLayout;
    QGroupBox* mprview = new QGroupBox("MPR View");
    QVBoxLayout* mprLayout = new QVBoxLayout;
    //Classification Intensity Range:
    QGroupBox* cir = new QGroupBox("Classification Intensity Range");
    QVBoxLayout* cirLayout = new QVBoxLayout;
    cirslider1 = new QSlider;
    cirslider1->setRange(0, 21);
    cirslider1->setValue(0);
    cirslider1->setOrientation(Qt::Horizontal);
    cirslider2 = new QSlider;
    cirslider2->setRange(0, 21);
    cirslider2->setValue(21);
    cirslider2->setOrientation(Qt::Horizontal);
    cirLayout->addWidget(cirslider1);
    cirLayout->addWidget(cirslider2);
    cir->setLayout(cirLayout);
    mprLayout->addWidget(cir);
        //distance parameter:
    QWidget* dist = new QWidget;
    QHBoxLayout* distLayout = new QHBoxLayout;
    QLabel* distanceLabel = new QLabel ("Distance:");
    distance = new QDoubleSpinBox;
    distance->setRange(0,25);
    distance->setValue(5);
    distslider = new QSlider;
    distslider->setRange(0, 25);
    distslider->setValue(5);
    distslider->setOrientation(Qt::Horizontal);
    distLayout->addWidget(distanceLabel);
    distLayout->addWidget(distslider);
    distLayout->addWidget(distance);
    dist->setLayout(distLayout);
    mprLayout->addWidget(dist);
    //Normal:
    QGroupBox* normal = new QGroupBox("Normal");
    QVBoxLayout* normalLayout = new QVBoxLayout;
    normalParam1 = new QSlider;
    normalParam2 = new QSlider;
    normalParam3 = new QSlider;
    normalParam1->setOrientation(Qt::Horizontal);
    normalParam2->setOrientation(Qt::Horizontal);
    normalParam3->setOrientation(Qt::Horizontal);
    normalParam1->setRange(-1 , 1);
    normalParam1->setValue(1);
    normalParam2->setRange(-1 , 1);
    normalParam2->setValue(0);
    normalParam3->setRange(-1 , 1);
    normalParam3->setValue(0);
    normalLayout->addWidget(normalParam1);
    normalLayout->addWidget(normalParam2);
    normalLayout->addWidget(normalParam3);
    normal->setLayout(normalLayout);
    normal->setMinimumSize(70, 117); //minimum size for normal

    mprLayout->addWidget(normal);
    //zoom parameter:
    QWidget* zoomParam = new QWidget;
    QHBoxLayout* zoomLayout = new QHBoxLayout;
    QLabel* zoomLabel = new QLabel ("Zoom:");
    zoom = new QDoubleSpinBox;
    zoom->setRange(1, 10);
    zoom->setValue(5.0);
    zoomslider = new QSlider;
    zoomslider->setRange(1, 10);
    zoomslider->setValue(5);
    zoomslider->setOrientation(Qt::Horizontal);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(zoomslider);
    zoomLayout->addWidget(zoom);
    zoomParam->setLayout(zoomLayout);
    mprLayout->addWidget(zoomParam);
    mprview->setLayout(mprLayout);
    
    QGroupBox* mipview = new QGroupBox("MIP View");
    QVBoxLayout* mipLayout = new QVBoxLayout;
    QGroupBox* mpicir = new QGroupBox("Classification Intensity Range");
    QVBoxLayout* mipcirLayout = new QVBoxLayout;
    mipcirslider1 = new QSlider;
    mipcirslider1->setRange(0, 21);
    mipcirslider1->setValue(0);
    mipcirslider1->setOrientation(Qt::Horizontal);
    mipcirslider2 = new QSlider;
    mipcirslider2->setRange(0, 21);
    mipcirslider2->setValue(21);
    mipcirslider2->setOrientation(Qt::Horizontal);
    mipcirLayout->addWidget(mipcirslider1);
    mipcirLayout->addWidget(mipcirslider2);
    mpicir->setLayout(mipcirLayout);
    mpicir->setMinimumSize(70, 90); //set minimum size for classification intensity range in mip
    mipLayout->addWidget(mpicir);
    //rotate around Y
    QWidget* raY = new QWidget;
    QHBoxLayout* raYLayout = new QHBoxLayout;
    QLabel* raYLabel = new QLabel("Rotation around Y:");
    rotaroundYSlider = new QSlider;
    rotaroundYSlider->setRange(0,360);//has to be degrees
    rotaroundYSlider->setOrientation(Qt::Horizontal);
    raYLayout->addWidget(raYLabel);
    raYLayout->addWidget(rotaroundYSlider);
    raY->setLayout(raYLayout);
    raY->setMaximumSize(300, 60); // maximum size of rotate around Y
    mipLayout->addWidget(raY);
    //Step size:
    QWidget* stepSize = new QWidget;
    QHBoxLayout* stepSizeLayout = new QHBoxLayout;
    QLabel* stepSizeLabel = new QLabel("Step Size:");
    stepSizeSlider = new QSlider;
    stepSizeSlider->setRange(1, 100);
    stepSizeSlider->setOrientation(Qt::Horizontal);
    stepSizeLayout->addWidget(stepSizeLabel);
    stepSizeLayout->addWidget(stepSizeSlider);
    stepSize->setLayout(stepSizeLayout);
    stepSize->setMaximumSize(300,60); // maximum size of step size
    mipLayout->addWidget(stepSize);
    mipview->setLayout(mipLayout);
    
    QGroupBox* selectFunction = new QGroupBox("transfer function");
    QHBoxLayout* advancedFunctionLayout = new QHBoxLayout;
    colorFunction = new QCheckBox("advanced function");
    advancedFunctionLayout->addWidget(colorFunction);
    selectFunction->setLayout(advancedFunctionLayout);
    
    paramsLayout->addWidget(mprview);
    paramsLayout->addWidget(selectFunction);
    paramsLayout->addWidget(mipview);
    params->setLayout(paramsLayout);
    
    
    QVBoxLayout* layoutLeft = new QVBoxLayout;

//    saveButton->setMinimumSize(100,20);// minimum size for the buttons
    loadButton->setMinimumSize(100,20);
//    layoutLeft->addWidget(saveButton);
    layoutLeft->addWidget(loadButton);
    mprview->setMaximumSize(250, 350); // maximum size for mpr view
    mipview->setMaximumSize(250, 220); // maximum size for mip view
    layoutLeft->addWidget(params);
    
    layoutLeft->addStretch(1);
    leftColumn->setLayout(layoutLeft);
    QSizePolicy fixIt;
    fixIt.setHorizontalPolicy(QSizePolicy::Fixed);
    fixIt.setVerticalPolicy(QSizePolicy::Expanding);
    leftColumn->setSizePolicy(fixIt);
    

    
    rightColumn = new QWidget(this);
    QHBoxLayout* layoutRight = new QHBoxLayout;
    ImageWidget = new ImageViewer;
    render = new Rendering;
    layoutRight->addWidget(ImageWidget);
    layoutRight->addWidget(render);
//    bottomButton = new QPushButton("visualize");
//    layoutRight->addWidget(bottomButton);
    rightColumn->setLayout(layoutRight);


    QHBoxLayout* layoutMain = new QHBoxLayout;
    layoutMain->addWidget(leftColumn);
    layoutMain->addWidget(rightColumn);
    setLayout(layoutMain);
}

void Visualization::linkWidgets()
{
    connect(loadButton, &QPushButton::clicked, this, &Visualization::load);

    connect(zoom, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setZoomslider);
    connect(zoomslider, &QSlider::valueChanged, this, &Visualization::setZoombox);
    connect(distance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setDistanceSlider);
    connect(distslider, &QSlider::valueChanged, this, &Visualization::setDistanceBox);

    /*
    connect(mipCirParam1, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setMinSliderMIP);
    connect(mipCirParam2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setMaxSliderMIP);
    connect(mipcirslider1, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Visualization::setMinBoxMIP);
    connect(mipcirslider2, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Visualization::setMaxBoxMIP);

    connect(cirParam1, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setMinSliderMPR);
    connect(cirParam2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Visualization::setMaxSliderMPR);
    connect(cirslider1, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Visualization::setMinBoxMPR);
    connect(cirslider2, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Visualization::setMaxBoxMPR);
    */
    //MPR
    connect(distance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), ImageWidget, &ImageViewer::distance);
    connect(zoom, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), ImageWidget, &ImageViewer::zoom);
    connect(cirslider1, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), ImageWidget, &ImageViewer::transMin);
    connect(cirslider2, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), ImageWidget, &ImageViewer::transMax);
    connect(normalParam1, &QSlider::valueChanged, ImageWidget, &ImageViewer::setX);
    connect(normalParam2, &QSlider::valueChanged, ImageWidget, &ImageViewer::setY);
    connect(normalParam3, &QSlider::valueChanged, ImageWidget, &ImageViewer::setZ);
    //MIP
    connect(stepSizeSlider, &QSlider::valueChanged, render, &Rendering::setStep);
    connect(rotaroundYSlider, &QSlider::valueChanged, render, &Rendering::setRotation);
    connect(mipcirslider1, &QSlider::valueChanged, render, &Rendering::transMin);
    connect(mipcirslider2, &QSlider::valueChanged, render, &Rendering::transMax);
    connect(colorFunction, &QCheckBox::clicked, ImageWidget, &ImageViewer::setTransferFunction);
}

void Visualization::load()
{
    QString fileName = QFileDialog::getOpenFileName(this,QDir::currentPath());
    autoload(fileName);
}

void Visualization::autoload(const QString &fileName) {
    try {
        volume = EDFHandler::read(fileName.toUtf8().constData());
        loaded = true;
        views();
    }   catch (std::invalid_argument e) {
        std::cerr << e.what() << std::endl;
    }
}

void Visualization::views() { //detaching those threads so reconstruction can directly go on and show slices
    std::thread mip(&Visualization::initMIP, this);
    mip.detach();
    std::thread mpr(&Visualization::initMPR, this);
    mpr.detach();
//    initMIP();
//    initMPR();
}

void Visualization::initMPR() {
    ImageWidget->clearImage();
    ImageWidget->setPlane(Eigen::Vector3f(1,0,0), 5);
//    std::cout << Eigen::Vector3f(normalParam1->value(),normalParam1->value(),normalParam1->value()) << std::endl << distance->value()<< std::endl;
    ImageWidget->setVolume(volume);
    ImageWidget->calculateImage();
    ImageWidget->update();
}

void Visualization::initMIP() {
    render->lock();
    render->setVolume(volume);
    render->setStep(stepSizeSlider->value());
    render->setRotation(rotaroundYSlider->value());
    render->setScreenPixels(200); //maximum recommendation from sheet
    render->setScreenWidth(0.25); //so everything fits on it real good
    render->setScreenDistance(0.5); //totally the perfect value
    render->setCenter(Eigen::Vector3f(0.075,0.075,0.125)); //center of box around volume
    render->free();
    render->calculateImage();
    render->update();
}

void Visualization::setZoombox(int v) {
    zoom->setValue(v);
}

void Visualization::setZoomslider(float v) {
    zoomslider->setValue((int) v);
}

void Visualization::setMinBoxMPR(int v) {
    cirParam1->setValue(v);
}

void Visualization::setMaxBoxMPR(int v) {
    cirParam2->setValue(v);
}

void Visualization::setMinSliderMPR(float v) {
    cirslider1->setValue((int) v);
}

void Visualization::setMaxSliderMPR(float v) {
    cirslider2->setValue((int) v);
}

void Visualization::setMinBoxMIP(int v) {
    mipCirParam1->setValue(v);
}

void Visualization::setMaxBoxMIP(int v) {
    mipCirParam2->setValue(v);
}

void Visualization::setMinSliderMIP(float v) {
    mipcirslider1->setValue((int) v);
}

void Visualization::setMaxSliderMIP(float v) {
    mipcirslider1->setValue((int) v);
}

void Visualization::setDistanceBox(int v) {
    distance->setValue(v - 5);
}

void Visualization::setDistanceSlider(float v) {
    distslider->setValue((int) v + 5);
}

