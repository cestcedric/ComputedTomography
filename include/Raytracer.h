#pragma once
#include <Eigen/Dense>
#include <list>
#include "volume.h"

class Raytracer {

private:
    Volume *volume;
    Eigen::Vector3f getIntersection(Eigen::Vector3f from, Eigen::Vector3f to);

public:
    Raytracer();
    void setVolume(Volume* volume);
    std::list<int> trace(Eigen::Vector3f start, Eigen::Vector3f finish);
};
