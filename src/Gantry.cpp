#include "Gantry.h"
#include <iostream>
#include <list>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace Eigen;

Gantry::Gantry(Eigen::Vector3f center, float angle, float s2dd, float dw, size_t N)
{
    detector = Eigen::MatrixXf::Zero(5,5);
    this->center = center;
    alpha = Rotation2Df(angle);
    this->s2dd = s2dd;
    this->dw = dw;
    this->N = N;
    setDetectorPositions();
}

std::list<Vector3f, aligned_allocator<Vector3f>> Gantry::getDetectorPositions(){
    return DetectorPositions;
}

void Gantry::setDetectorPositions(){
    //get boundaries
    Vector3f normal = getDetectorNormal();
    Vector2f leftDirection = Rotation2Df(3*M_PI/2.0) * Vector2f(normal(0), normal(1));
    Vector2f leftBorder = Vector2f(getDetectorCenter()(0), getDetectorCenter()(1)) + dw/2 * leftDirection;
    Vector2f rightBorder = Vector2f(getDetectorCenter()(0), getDetectorCenter()(1)) - dw/2 * leftDirection;

    Vector3f upperLeft(leftBorder(0), leftBorder(1), center(2) + dw/2);
    Vector3f lowerLeft(leftBorder(0), leftBorder(1), center(2) - dw/2);
    Vector3f lowerRight(rightBorder(0), rightBorder(1), center(2) - dw/2);

    //vectors to step through the detector plane
    Vector3f stepHoe = (lowerRight - lowerLeft)/N;//not a typo^^, horizontal
    Vector3f stepLilUzi = (lowerLeft - upperLeft)/N;//not a typo either, vertical

    //vector to add to list
    Vector3f current = upperLeft;

    //iterate over detector and add position-vectors to the list
    /*
    int k = 0;
    vectorPositions = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>(25);
    */

    for (int i{0}; i < N ; i++) {
        for (int j{0}; j < N; j++) {
            DetectorPositions.push_back(current + (i + 0.5)*stepLilUzi + (j + 0.5)*stepHoe);
            /*
            vectorPositions[k] = current + (i + 0.5)*stepLilUzi + (j + 0.5)*stepHoe;
            ++k;
            */
        }
    }
}

//only used to get positions in a vector for conjugate gradient, which may or may not be faster
/*
std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> Gantry::gitGud() {
    return vectorPositions;
}
*/

void Gantry::setAngle(float alpha)
{
    this->alpha = Rotation2Df(alpha);
}

//we only rotate horizontally, the heights of detector center, rotation center and source are the same
Vector3f Gantry::getSourcePosition()
{
    Vector2f tmp_source(-s2dd/2, 0);//rotation happens only in this plane
    tmp_source = alpha*tmp_source;
    Vector3f source(tmp_source(0) + center(0), tmp_source(1) + center(1), center(2));
    return source;

}

Vector3f Gantry::getDetectorCenter()
{
    return (getSourcePosition() - s2dd*getDetectorNormal());
}

Vector3f Gantry::getDetectorNormal()
{
    return ((getSourcePosition() - center).normalized());
}

ParametrizedLine<float, 3> Gantry::getRay(int pixel) {
    Vector3f origin = getSourcePosition();
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>list = getDetectorPositions();
    for (int i{0}; i < pixel; ++i) {
        list.pop_front();
    }
    Vector3f direction = (list.front() - origin).normalized();
    return ParametrizedLine<float, 3>(origin,  direction);
}

void Gantry::setDetector(Eigen::Matrix2f newdetector){
    detector=newdetector;
}

Eigen::MatrixXf Gantry::getDetector(){
    return detector;
}

void Gantry::addValuesToDetector(int x, int y, int value){
    //detector(y,x) = detector(y,x) + value; *y tho*
    detector(x,y) = detector(x,y) + value;
}

void Gantry::makeNoise() {
    float min = detector(0,0);
    float max = detector(0,0);
    for (int i{0}; i < 24; ++i) {
        if (min > detector(i%5, i/5)) min = detector(i%5, i/5);
        if (max < detector(i%5, i/5)) max = detector(i%5, i/5);
    }
    Eigen::MatrixXf random = Eigen::MatrixXf::Random(5,5);
    random = random * 0.02 * (max - min);
    detector = detector + random;
}
