#pragma once
#include "QWidget"
#include "Eigen/Dense"

class Pose : public QWidget {

    Q_OBJECT

public:
    Pose(QWidget * parent = 0);
    void setNumPoses(int numPoses);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    int numberOfPoses;
    QImage Im;
    Eigen::Rotation2Df rotation;
    Eigen::Vector2i center;
    Eigen::Vector2i sourcePosition;
    int horizontalOffset = width() /2 - 200;
    int verticalOffset = height() /2 -200;
    QSize ImSize = QSize(400+horizontalOffset,400+verticalOffset);
};
