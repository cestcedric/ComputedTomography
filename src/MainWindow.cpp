#include "MainWindow.h"

MainWindow::MainWindow() {
    mainWidget = new QTabWidget;
    createWidgets();
    linkWidgets();
    this->setWindowTitle("Tomograph 3000");
}

void MainWindow::createWidgets() {
    raytracer = new Raytracer;
    mainWidget = new QTabWidget;
    acWidget = new Acquisition();
    acWidget->setTracer(raytracer);
    reWidget = new Reconstruction();
    reWidget->setTracer(raytracer);
    viWidget = new Visualization();

    mainWidget->addTab(acWidget, "Acquisition");
    mainWidget->addTab(reWidget, "Reconstruction");
    mainWidget->addTab(viWidget, "Visualization");

    setCentralWidget(mainWidget);
}

void MainWindow::linkWidgets() {
    //send filepaths
    connect(acWidget, &Acquisition::outputFilePath, reWidget, &Reconstruction::autoload);
    connect(reWidget, &Reconstruction::outputFilePath, viWidget, &Visualization::autoload);
}

void MainWindow::updateCurrentWidget(int index) {
    if (index == 1) acWidget->update();
    if (index == 2) reWidget->update();
    if (index == 3) viWidget->update();
}

void MainWindow::cmdLoad(std::string path) {
    acWidget->cmdLoad(path);
}

