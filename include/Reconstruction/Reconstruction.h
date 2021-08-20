#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QFileDialog>
#include <Eigen/Dense>
#include "EDFhandler.h"
#include "volume.h"
#include "sliceViewer.h"
#include "Raytracer.h"
#include "Reconstruction/base.h"
#include "Reconstruction/normalReconstruction.h"
#include "Reconstruction/regularizedReconstruction.h"
#include "Gantry.h"

class Reconstruction : public QWidget {

    Q_OBJECT

private:
    QWidget* mainWidget;//show positions and resulting images
    QWidget* leftColumn;//save, load, settings
    QWidget* rightColumn;
    QWidget* bottomButton;//big ass simulate button

    QSize minimumSizeHint() const;
    QPushButton* reconstructButton;
    QPushButton* saveButton;
    QPushButton* loadButton;

    //slice to show
    SliceViewer* sliceViewWidget;
    QSlider* islide;

    //iterations for conjugate gradient algorithm
    QSpinBox* iterations;
    QProgressBar* progress;

    //use regularized reconstruction or not
    QCheckBox* regularized;
    QDoubleSpinBox* lambda;


    Volume volume;
    Volume results;
    Raytracer *raytracer;
    base* matrix;
    std::vector<Eigen::MatrixXf> reconstructedSlices;
    std::list<Gantry> poses;

    bool loaded;
    QString path;

    void linkWidgets();
    void setWidgets();
    void setPoses(int amount);
    void congrads();//conjugate gradient
    void boiii();//trying to reconstruct without loading a volume
    void autosave();//automatically saves results of reconstruction

public:
    Reconstruction(QWidget* parent = 0);
    void setTracer(Raytracer* raytracer);

signals:
    void outputFilePath(const QString&);//send .edf path to the other widgets as preset

public slots:
    void autoload(const QString& path);
    void save();
    void load();
    void showSlice(int index);

    void reconstruct();

};
