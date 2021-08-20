#define _USE_MATH_DEFINES
#include "Acquisition/detectorWidget.h"
#include "QPainter"
#include "QPaintEvent"
#include <iostream>
#include <cmath>

DetectorWidget::DetectorWidget(QWidget *parent):
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    detectorMap = Eigen::MatrixXf::Zero(5,5);
    QPixmap* pm = new QPixmap(ImSize);
    pm->fill(Qt::black);
    Image = pm->toImage();
    minimumSizeHint();
}
void DetectorWidget::paintEvent(QPaintEvent *event)
{
    QPainter ptr(this);
    QRect screen = event->rect();//was done like this in a qt example
    screen.adjust(horizontalOffset,0,ImSize.width()+horizontalOffset,ImSize.height());
    for(int x = horizontalOffset; x < ImSize.width(); ++x)
    {
        for(int y = 0; y < ImSize.height(); ++y)
        {
           //detectorMap(x,y) grayscale is from 0 to 25.5 => detectorMap(x,y) = 0 is minimum; detector(x,y) = 255 is maximum
           float g = qGray(detectorMap((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/5),y/(ImSize.height()/5))*10,detectorMap((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/5),y/(ImSize.height()/5))*10,detectorMap((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/5),y/(ImSize.height()/5))*10);
           int gray = g/5;
           if(gray > 255) gray = 255;
           if(gray < 0) gray = 0;
           Image.setPixel(x,y,QColor(gray,gray,gray).rgb());
        }
    }
     ptr.drawImage(screen, Image, screen);
}
void DetectorWidget::setDetectorMap(Eigen::MatrixXf map)
{
    if(map.cols() != 5 || map.rows() != 5) {
        throw new std::invalid_argument("wrong dimensions: matrix has to be 5x5");
    }   else {
        detectorMap = map;
        QPixmap* pm = new QPixmap(ImSize);
        pm->fill(Qt::black);
        Image = pm->toImage();
        update();
    }
}


