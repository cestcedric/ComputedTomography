#include "Reconstruction/regularizedReconstruction.h"
#include "Reconstruction/projection.h"

regularizedReconstruction::regularizedReconstruction(Raytracer *raytracer, std::list<Gantry> poses, float lambda) {
    this->raytracer = raytracer;
    this->poses = poses;
    this->lambda = lambda;
    this->projector = new Projection(poses, raytracer);
}

void regularizedReconstruction::setLambda(float lambda) {
    this->lambda = lambda;
}

void regularizedReconstruction::setRaytracer(Raytracer *raytracer) {
    this->raytracer = raytracer;
    this->projector->setRaytracer(raytracer);
}

Eigen::VectorXf regularizedReconstruction::forwardProjection(Eigen::VectorXf *v) {
    return projector->forwardProjection(v);
}

Eigen::VectorXf regularizedReconstruction::backProjection(Eigen::VectorXf *v) {
    return projector->backProjection(v);
}

//(aT * a + h1) * x = aT * (a * x) + h * x
Eigen::VectorXf regularizedReconstruction::mult(Eigen::VectorXf *v) {
    Eigen::VectorXf x = *v;
    Eigen::VectorXf ax = forwardProjection(v);
    Eigen::VectorXf aTax = backProjection(&ax);
    Eigen::VectorXf aTahx = aTax + lambda*x;
    return aTahx;
}

//(aT * a + h1)T * x = ((aT * a)T + h1T) * x = (aT * a + h1) * x = aT * (a * x) + h * x
Eigen::VectorXf regularizedReconstruction::multTrans(Eigen::VectorXf *v) {
    return mult(v);
}




