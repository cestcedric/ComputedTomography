#include "Reconstruction/sliceViewer.h"
#include "QPainter"
#include "QPaintEvent"

SliceViewer::SliceViewer(QWidget *parent):
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    reconstructedImageMatrix = Eigen::MatrixXf::Zero(10,10);
    QPixmap* pm = new QPixmap(ImSize);
    pm->fill(Qt::black);
    Image = pm->toImage();
    minimumSizeHint();
}


void SliceViewer::paintEvent(QPaintEvent *event)
{
    QPainter ptr(this);
    QRect screen = event->rect();//was done like this in a qt example
    screen.adjust(horizontalOffset,0,horizontalOffset+ImSize.width(),ImSize.height());
    for(int x = horizontalOffset; x < ImSize.width(); ++x)
    {
        for(int y = 0; y < ImSize.height(); ++y)
        {
           //
           int gray = qGray(reconstructedImageMatrix((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/10),y/(ImSize.height()/10))*10,reconstructedImageMatrix((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/10),y/(ImSize.height()/10))*10,reconstructedImageMatrix((x-horizontalOffset)/((ImSize.width()-horizontalOffset)/10),y/(ImSize.height()/10))*10);
           if(gray > 255) gray = 255;
           if(gray < 0) gray = 0;
           Image.setPixel(x,y,QColor(gray,gray,gray).rgb());
        }
    }
     ptr.drawImage(screen, Image, screen);
}

void SliceViewer::setReconstructedImageMatrix(Eigen::MatrixXf matrix)
{
    if(matrix.cols()==10 && matrix.rows()==10)
    reconstructedImageMatrix = matrix;
}


