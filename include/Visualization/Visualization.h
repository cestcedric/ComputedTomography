#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QFileDialog>
#include <Eigen/Dense>
#include "EDFhandler.h"
#include "volume.h"
#include "Visualization/imageViewer.h"
#include "Visualization/rendering.h"

class Visualization : public QWidget {

    Q_OBJECT

private:
    QWidget* mainWidget;//show positions and resulting images
    QWidget* leftColumn;//save, load, settings
    QWidget* rightColumn;
    QWidget* bottomButton;//big ass simulate button
//    QWidget* saveButton;
    QPushButton* loadButton;
    QDoubleSpinBox* distance;
    QSlider* distslider;
    QDoubleSpinBox* zoom;
    QSlider* zoomslider;

    QDoubleSpinBox* cirParam1; // first parameter of classification intensity range
    QDoubleSpinBox* cirParam2;
    QSlider* cirslider1;
    QSlider* cirslider2;
    //vielleicht nicht Normale und Distanz verwenden sondern links/rechts, oben/unten (0 -> 360), vor/hinter (-5 -> 5)
    QSlider* normalParam1; // first parameter of normal
    QSlider* normalParam2;
    QSlider* normalParam3;

    QDoubleSpinBox* mipCirParam1;// first parameter of classification intensity range in MIP view
    QDoubleSpinBox* mipCirParam2;
    QSlider* mipcirslider1;
    QSlider* mipcirslider2;
    QSlider* rotaroundYSlider; // slider for rotation around Y
    QSlider* stepSizeSlider;

    QCheckBox* colorFunction; //transferfunction with color
    
    Volume volume;
    bool loaded;

    void views();
    void initMPR();
    void initMIP();
    //QSize minimumSizeHint() const;
    Rendering* render;
    ImageViewer* ImageWidget;
public:

    Visualization(QWidget* parent = 0);
    void setWidgets();
    void linkWidgets();


public slots:
    void autoload(const QString&);
    void load();

    void setMinBoxMPR(int v);
    void setMaxBoxMPR(int v);
    void setMinSliderMPR(float v);
    void setMaxSliderMPR(float v);

    void setMinBoxMIP(int v);
    void setMaxBoxMIP(int v);
    void setMinSliderMIP(float v);
    void setMaxSliderMIP(float v);

    void setZoomslider(float v);
    void setZoombox(int v);
    void setDistanceBox(int v);
    void setDistanceSlider(float v);
};
