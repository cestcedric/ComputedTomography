#pragma once
#include "Reconstruction/base.h"

class normalReconstruction : public base {

private:

    std::list<Gantry> poses;
    Raytracer *raytracer;
    Projection *projector;

public:

    normalReconstruction(Raytracer* raytracer, std::list<Gantry> poses);

    Eigen::VectorXf forwardProjection(Eigen::VectorXf* v);
    Eigen::VectorXf backProjection(Eigen::VectorXf* v);

    Eigen::VectorXf mult (Eigen::VectorXf* v);
    Eigen::VectorXf multTrans (Eigen::VectorXf* v);

    void setRaytracer(Raytracer* raytracer);
};
