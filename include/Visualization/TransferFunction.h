#pragma once
#include <QWidget>
#include <QColor>

class TransferFunction{
    
    QColor *color;
    float imin=0;
    float imax=20;
    
public:
    TransferFunction();
    QColor classify(float intensity);
    QColor advancedFunction(float intensity);
    void setMin(float min);
    void setMax(float max);
};
