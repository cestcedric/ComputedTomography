#define _USE_MATH_DEFINES
#include "Acquisition/pose.h"
#include "QPainter"
#include "QPaintEvent"
#include <iostream>
#include <cmath>

Pose::Pose(QWidget *parent) :
QWidget(parent)
{
    numberOfPoses = 1;
    center = Eigen::Vector2i(ImSize.width()/2,ImSize.height()/2);
    rotation = Eigen::Rotation2Df(0);
    sourcePosition = center + Eigen::Vector2i(-150,0);
    setAttribute(Qt::WA_StaticContents);
    minimumSizeHint();

}
void Pose::paintEvent(QPaintEvent *event)
{
    //draw the window in the widget
    horizontalOffset = this->width()/2 - 200;
    verticalOffset = this->height()/2 -200;
    ImSize = QSize(400 + horizontalOffset, 400 + verticalOffset);
    QPixmap* pm = new QPixmap(ImSize);
    pm->fill(Qt::white);
    Im = pm->toImage();
    center = Eigen::Vector2i(ImSize.width()/2,ImSize.height()/2);
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    dirtyRect.adjust(horizontalOffset,verticalOffset,ImSize.width()+horizontalOffset,ImSize.height()+verticalOffset);
    QRect q = QRect(QPoint((ImSize.width()+horizontalOffset)/2 -50,(ImSize.height()+verticalOffset)/2-50),QSize(100,100));
    painter.drawImage(dirtyRect,Im,dirtyRect);
    //draw the volume box
    Eigen::Vector2i detectorPosLeft, detectorPosRight;
    painter.setPen(Qt::red);
    painter.drawRect(q);

    for(int i = 0; i < numberOfPoses; ++i)
    {
        //draw the source as a circle
        painter.setPen(Qt::green);
        rotation = Eigen::Rotation2Df (i*M_PI/numberOfPoses);
        detectorPosLeft = center + (rotation * Eigen::Vector2f(150,75)).cast<int>();
        detectorPosRight = center + (rotation *Eigen::Vector2f(150,-75)).cast<int>();
        sourcePosition = center + (rotation *Eigen::Vector2f(-150,0)).cast<int>();
        painter.drawEllipse(QPoint(sourcePosition(0)+horizontalOffset/2,sourcePosition(1)+verticalOffset/2),10,10);
        //draw the detector as a line
        painter.setPen(Qt::blue);
        painter.drawLine(QPoint(detectorPosLeft(0)+horizontalOffset/2,detectorPosLeft(1) +verticalOffset/2),QPoint(detectorPosRight(0)+horizontalOffset/2,detectorPosRight(1)+verticalOffset/2));
        //draw the rays
        painter.setPen(Qt::gray);
        for(int k = 0; k < 5; k++)
        {
            Eigen::Vector2i detElem = center + (rotation * Eigen::Vector2f(150,75-((k + 0.5)*150/5))).cast<int>();
            painter.drawLine(QPoint(sourcePosition(0)+horizontalOffset/2,sourcePosition(1)+verticalOffset/2),QPoint(detElem(0)+horizontalOffset/2,detElem(1)+verticalOffset/2));
        }
    }
}
void Pose::setNumPoses(int numPoses){
    numberOfPoses = numPoses;
}



