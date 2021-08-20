#include <QWidget>
#include <QColor>
#include "Visualization/TransferFunction.h"
#include <iostream>

TransferFunction::TransferFunction(){}

QColor TransferFunction::classify(float intensity){
    color= new QColor;
    if(intensity<=imin){
        color->setAlphaF(0.0);
    }
    else if(intensity>=imax){
        color->setAlphaF(1.0);
    }
    else{
        float alpha = (intensity-imin) / (imax-imin);
        color->setAlphaF(alpha);
    }
    return *color;
}

QColor TransferFunction::advancedFunction(float intensity){
    color = new QColor;
    if(intensity<=imin){
        color->setAlphaF(0.0);
    }
    else if(intensity>=imax){
        color->setAlphaF(1.0);
    }
    else{
        //int colorfun = (int)((intensity*100.0)*(25500.0/((imax*100.0-imin*100)/2.0))/100.0);
        if(intensity<imax-((imax-imin)/2.0)){
            color->setRed(intensity*12);
            color->setGreen(10);
            color->setBlue(100);
        }else{
            color->setRed(100);
            color->setGreen(10);
            color->setBlue(255 - intensity*12);
        }
        
        float alpha = (intensity-imin) / (imax-imin);
        color->setAlphaF(alpha);
    }
    return *color;
}

void TransferFunction::setMin(float min) {
    imin = min;
}

void TransferFunction::setMax(float max) {
    imax = max;
}

