#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QFileDialog>
#include <Eigen/Dense>
#include <list>
#include <string>
#include "EDFhandler.h"
#include "volume.h"
#include "pose.h"
#include "detectorWidget.h"
#include "Gantry.h"
#include "Raytracer.h"

class Acquisition : public QWidget {

    Q_OBJECT

private:
    QWidget* mainWidget;//whole widget
    QWidget* leftColumn;//save, load, settings
    QWidget* rightColumn;//show stuff + simulate button
    QWidget* bottomButton;//big ass simulate button

    QSize minimumSizeHint() const override;

    QPushButton* simulateButton;
    QPushButton* saveButton;
    QPushButton* loadButton;

    QSpinBox* posesSpinBox;
    QProgressBar* progress;
    QSlider* islide;//slider to choose the pose to show the result of
    QComboBox* toggle;//show top or bottom part of position

    //display parametrized line of certain pose and detector pixel (source -> direction)
    QPushButton* shower;
    QSpinBox* rayPose;
    QSpinBox* rayX;
    QSpinBox* rayY;

    //noise
    QCheckBox* noiseCheck;

    //actual useful stuff
    Pose* poseWidget;
    DetectorWidget* detectorWidget;
    Volume volume;
    Volume result;//not a volume, I just the need the EDFhandler to work w/o thinking too much
    QImage img;
    QString path;//path to where the volume .edf was stored
    int numberOfPoses;//number of poses, duh
    bool top;
    bool loaded;


    std::list<Gantry> AcquisitionPoses;
    Raytracer *raytracer;


    void setWidgets();
    void linkWidgets();
    void autosave();
    void ask();//simulating measurements on the empty stock volume

public:
    Acquisition(QWidget* parent = 0);
    void setTracer(Raytracer* raytracer);

    //actual methods that calculate things
    std::list<int> raytrace(Eigen::Vector3f, Eigen::Vector3f);//call with source as first parameter
    void forwardProjection();

    void showPositions();//draws positions on mainWidget

    void cmdLoad(std::string path);
signals:
    void outputFilePath(const QString&);//send .edf path to the other widgets as preset

public slots:
    void save();
    void load();
    void setPoses(int amount);
    //read chosen pose and pixel, send getProjectionRay signal
    void showRay();

    //actual methods that calculate things
    void simulate();
    void showSimulation(int);//shows simulated result of scan from pose #index, index from slider
    void toggleSimulation(int);//show top or bottom scan

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

};
