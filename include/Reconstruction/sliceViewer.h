#pragma once
#include <Eigen/Dense>
#include <QWidget>

class SliceViewer : public QWidget{
    Q_OBJECT
public:
    SliceViewer(QWidget * parent = 0);
    void setReconstructedImageMatrix(Eigen::MatrixXf matrix);
    void setImageSize(QSize size);

protected:

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
   Eigen::MatrixXf reconstructedImageMatrix;
   QImage Image;
   const int horizontalOffset = 100;
   const QSize ImSize = QSize(400+horizontalOffset,400);
};
