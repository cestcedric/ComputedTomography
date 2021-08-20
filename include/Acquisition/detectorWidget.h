#pragma once
#include "QWidget"
#include "Eigen/Dense"

class DetectorWidget : public QWidget {

    Q_OBJECT

public:
    DetectorWidget(QWidget * parent = 0);
    void setDetectorMap(Eigen::MatrixXf map);
    void setImageSize(QSize size);
    // handle 5x5 Matrices

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QImage Image;
    Eigen::MatrixXf detectorMap;
    const int horizontalOffset = 75;
    const QSize ImSize = QSize(400+horizontalOffset,400);

};
