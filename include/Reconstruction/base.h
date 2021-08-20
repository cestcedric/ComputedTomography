#pragma once
#include <Eigen/Dense>
#include <list>
#include "volume.h"
#include "Raytracer.h"
#include "Gantry.h"
#include "include/Reconstruction/projection.h"

class base {

private:

    std::list<Gantry> poses;
    Raytracer *raytracer;
    Projection *projector;

public:

    virtual Eigen::VectorXf forwardProjection(Eigen::VectorXf* v) = 0;
    virtual Eigen::VectorXf backProjection(Eigen::VectorXf* v) = 0;
    virtual void setRaytracer(Raytracer* raytracer) = 0;
    virtual Eigen::VectorXf mult (Eigen::VectorXf* v) = 0;
    virtual Eigen::VectorXf multTrans (Eigen::VectorXf* v) = 0;

};
