#include "Visualization/imageViewer.h"
#include "QPainter"
#include "QPaintEvent"
#include <iostream>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

ImageViewer::ImageViewer(QWidget *parent):
QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    QPixmap* pm = new QPixmap(ImSize);
    pm->fill(Qt::black);
    Image = pm->toImage();
    delete pm;
    minimumSizeHint();
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    QPainter ptr(this);
    QRect screen = event->rect();
    ptr.drawImage(screen, Image, screen);
}

std::vector<Vector3i> &ImageViewer::getIntersections()
{
    vector<Vector3i>* ret = new vector<Vector3i>();
    int e = 0;
    for(int z = 0; z < 10; z++)
    {
        for(int y = 0; y < 10; y++)
        {
            for(int x = 0; x < 10; x++)
            {
                if( x * plane.n(0) + y * plane.n(1) + z * plane.n(2) == plane.d)
                {
                    ret->push_back(Vector3i(x*20,y*20,z*20));
                    e++;
                }
            }
        }
    }
    //std::cout << e << std::endl;
    return *ret;
}

Vector3f ImageViewer::getMean(vector<Vector3i> &in)
{
    Vector3f tmp = Vector3f(0,0,0);
    for(Vector3i e : in)
    {
        tmp += Vector3f(e(0),e(1),e(2));
    }
    return tmp/in.size();
}

Vector2i ImageViewer::getCoordinates(Vector2f &v,int offsetX, int offsetY)
{
    Vector2f tmp(v(0) - offsetX,v(1)-offsetY);
    tmp = rotation * tmp;
    return Vector2f(tmp(0)+100,tmp(1)+100).cast<int>();
}

/*QImage*/ void  ImageViewer::calculateImage()
{

    vector<Vector3i>* intersecs = &getIntersections();


    clearImage();
    if(!intersecs->empty())
    {
    QImage alpha = Image.alphaChannel();
    int size = intersecs->size();


    // defining a new orthonormal basis: <X_basis,Y_basis>
    Vector3f origin = getMean(*intersecs);
    Vector3f X_basis = (intersecs->front().cast<float>() - origin).normalized();
    Vector3f Y_basis = (X_basis.cross(plane.n)).normalized();
    MatrixXf A = MatrixXf(3,2);
    A << X_basis,Y_basis;
    Vector3f b = origin;
    Vector2f m = A.colPivHouseholderQr().solve(b);

    for(int y= 0; y < 200; y++)
    {
        for(int x = 0; x < 200; x++)
        {
            Vector2f v((x -100)/zoomFactor,(y -100)/zoomFactor);
            v = rotation * v;
            b = A*v + origin;
           // std::cout << b(0) << "," << b(1) << "," << b(2) << std::endl;
            if(!advancedTransferFunction)
                alpha.setPixel(x,y, 255 - get.classify(Vol->getVoxelLinear(b(0)/20, b(1)/20, b(2)/20)).alpha());
            else
                Image.setPixel(x, y, get.advancedFunction(Vol->getVoxelLinear(b(0)/20, b(1)/20, b(2)/20)).rgb());
        }
    }


    Image.setAlphaChannel(alpha);
    }
    delete intersecs;

}

void ImageViewer::setVolume(Volume& v)
{
    Vol = &v;
}

void ImageViewer::setPlane(Vector3f n, int d)
{
    plane =  HNF(n,d);
}
void ImageViewer::clearImage()
{
    QPixmap* pm = new QPixmap(ImSize);
    pm->fill(Qt::black);
    Image = pm->toImage();
    delete pm;
}

void ImageViewer::zoom(float z) {//vielleicht machen lol
    zoomFactor = z/5;
    if(!Image.isNull())
    if(Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    update();
}

void ImageViewer::distance(float d) {//auch ganz cool; Distance Parameter auch float?
    plane.d = int(d);
    if(Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    update();
}

void ImageViewer::setX(float x) {
    Eigen::Vector3f normal = plane.n;
    int dist = plane.d; //<- d als float?
    normal(0) = x;
    setPlane(normal, dist); //<- Distance als float?
    if(Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    else clearImage();
    update();
}

void ImageViewer::setY(float y) {
    Eigen::Vector3f normal = plane.n;
    int dist = plane.d; //<- d als float?
    normal(1) = y;
    setPlane(normal, dist); //<- Distance als float?
    if(Vol != nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    else clearImage();
    update();
}

void ImageViewer::setZ(float z) {
    Eigen::Vector3f normal = plane.n;
    int dist = plane.d; //<- d als float?
    normal(2) = z;
    setPlane(normal, dist); //<- Distance als float?
    if(Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    else clearImage();
    update();
}

void ImageViewer::transMin(float min) {
    get.setMin(min);
    if (Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    else clearImage();
    update();
}

void ImageViewer::transMax(float max) {
    get.setMax(max);
    if(Vol!=nullptr && plane.n != Vector3f(0,0,0))
    calculateImage();
    else clearImage();
    update();
}

void ImageViewer::setTransferFunction(bool advanced){
    advancedTransferFunction = advanced;
}






