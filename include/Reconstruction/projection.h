#pragma once
#include "Gantry.h"
#include "Raytracer.h"
#include <list>
#include <Eigen/Dense>

class Projection {

private:
    std::list<Gantry> poses;
    Raytracer *raytracer;

public:
    Projection(std::list<Gantry> poses, Raytracer* raytracer);

    Eigen::VectorXf forwardProjection(Eigen::VectorXf *v);
    Eigen::VectorXf backProjection(Eigen::VectorXf *v);

    void setPoses(std::list<Gantry> poses);
    void setRaytracer(Raytracer* raytracer);
};
