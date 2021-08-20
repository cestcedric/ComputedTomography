#pragma once
#include "Reconstruction/base.h"

class regularizedReconstruction : public base {

private:

    std::list<Gantry> poses;
    Raytracer *raytracer;
    Projection *projector;
    float lambda;

public:

    regularizedReconstruction(Raytracer* raytracer, std::list<Gantry> poses, float lambda);

    Eigen::VectorXf forwardProjection(Eigen::VectorXf* v);
    Eigen::VectorXf backProjection(Eigen::VectorXf* v);

    Eigen::VectorXf mult (Eigen::VectorXf* v);
    Eigen::VectorXf multTrans (Eigen::VectorXf* v);

    void setRaytracer(Raytracer* raytracer);
    void setLambda(float lambda);
};
