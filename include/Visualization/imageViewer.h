#pragma once
#include <Eigen/Dense>
#include <QWidget>
#include <volume.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <Visualization/TransferFunction.h>

class ImageViewer : public QWidget{
    Q_OBJECT
public:   
    struct HNF{
       Eigen::Vector3f n;
       int d;
       HNF(Eigen::Vector3f ne, int de){ n=ne;d= de;}
       HNF(){}
    };
    std::vector<Eigen::Vector3i> &getIntersections();
    /*QImage*/ void calculateImage();
    ImageViewer(QWidget * parent = 0);
    void setVolume(Volume& v);
    void setPlane(Eigen::Vector3f n, int d);//d als float?
    void setImageSize(QSize size);
    Eigen::Vector2i getCoordinates(Eigen::Vector2f &v,int offsetX, int offsetY);
    Eigen::Vector3f getMean(std::vector<Eigen::Vector3i> &in);
    void clearImage();
    const Eigen::Rotation2Df rotation = Eigen::Rotation2Df(M_PI/4);

protected:

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
   Volume * Vol;
   HNF plane;
   float zoomFactor = 1.0;
   QImage Image;
   TransferFunction get;
   const QSize ImSize = QSize(200,200);
   bool advancedTransferFunction;
public slots:
//METHODEN FEHLEN////
////////////////////////
   void zoom(float z);
   void distance(float d);
///////////////////////
   void setX(float x);
   void setY(float y);
   void setZ(float z);
   void transMin(float min);
   void transMax(float max);
   void setTransferFunction(bool advanced);
};
