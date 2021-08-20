#pragma once
#include "volume.h"
#include "Eigen/Dense"
#include <vector>
#include <list>

class Gantry{

public:
    // Guess we don't need any classes for source and detector
    Gantry(Eigen::Vector3f center, float angle,float s2dd, float dw, size_t N);
    void setAngle(float alpha);
    float getAngle();
    Eigen::Vector3f getSourcePosition();
    Eigen::Vector3f getDetectorCenter();
    Eigen::Vector3f getDetectorNormal();
    Eigen::ParametrizedLine<float, 3> getRay(int);
    void setDetectorPositions();
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> getDetectorPositions();
    
    void setDetector(Eigen::Matrix2f newdetector);
    Eigen::MatrixXf getDetector();
    void addValuesToDetector(int x, int y, int value);
    void makeNoise();

    /*
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> gitGud();
    */

private:
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> DetectorPositions;
    /*
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vectorPositions;
    */
    Volume volume;
    Eigen::Vector3f center;
    float s2dd;
    float dw;
    size_t N;
    Eigen::Rotation2Df alpha;
    Eigen::MatrixXf detector;
    
    /*
     * Detector Pixels
     * ---------------------------------
     * |  0  |  1  |  2  |  3  |  4  |
     * ---------------------------------
     * |  5  |  6  |  7  |  8  |  9  |
     * ---------------------------------
     * | 10 | 11 | 12 | 13 | 14 |
     * ---------------------------------
     * | 15 | 16 | 17 | 18 | 19 |
     * ---------------------------------
     * | 20 | 21 | 22 | 23 | 24 |
     * ---------------------------------
     */
};
