#pragma once
#include <QtWidgets>
#include "Raytracer.h"
#include "Acquisition/Acquisition.h"
#include "Reconstruction/Reconstruction.h"
#include "Visualization/Visualization.h"

class MainWindow : public QMainWindow {

    Q_OBJECT

private:

    QTabWidget* mainWidget;
    Acquisition* acWidget;
    Reconstruction* reWidget;
    Visualization* viWidget;


public:
    Raytracer* raytracer;

    MainWindow();
    void createWidgets();
    void linkWidgets();
    void cmdLoad(std::string path);

public slots:
    void updateCurrentWidget(int);

};
