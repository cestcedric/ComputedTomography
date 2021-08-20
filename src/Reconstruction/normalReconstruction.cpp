#include <iterator>
#include <iostream>
#include "Reconstruction/normalReconstruction.h"

normalReconstruction::normalReconstruction(Raytracer* raytracer, std::list<Gantry> poses) {
    this->raytracer = raytracer;
    this->poses = poses;
    this->projector = new Projection(poses, raytracer);
}

//aT * a * x = aT * (a * x))
Eigen::VectorXf normalReconstruction::mult(Eigen::VectorXf *v) {
    Eigen::VectorXf ax = forwardProjection(v);
    Eigen::VectorXf aTax = backProjection(&ax);
    return aTax;
}

//(aT * a)T * x = aT * a * x ??? -> then just call mult
Eigen::VectorXf normalReconstruction::multTrans(Eigen::VectorXf *v) {
//    Eigen::VectorXf aTx = backProjection(v);
//    Eigen::VectorXf aaTx = forwardProjection((&aTx));
//    return aaTx;
    return mult(v);
}

void normalReconstruction::setRaytracer(Raytracer *raytracer) {
    this->raytracer = raytracer;
    this->projector->setRaytracer(raytracer);
}

Eigen::VectorXf normalReconstruction::forwardProjection(Eigen::VectorXf *v) {
    return projector->forwardProjection(v);
}

Eigen::VectorXf normalReconstruction::backProjection(Eigen::VectorXf *v) {
    return projector->backProjection(v);
}

/*
//v = volume data, returns a concatenated vector of all detectors
Eigen::VectorXf normalReconstruction::forwardProjection(Eigen::VectorXf *v) {
//    std::cout << "forwardProjection" << std::endl;
    Eigen::VectorXf projection = Eigen::VectorXf::Zero(25*poses.size());
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    std::list<int> indexes;
    int numIndex;
    int numPose = 0;
    int offset;
    Eigen::Vector3f source;
    for (std::list<Gantry>::iterator iterator = poses.begin(), end = poses.end(); iterator != end; ++iterator) {
        positions = iterator->getDetectorPositions();
        offset = numPose*25;
        source = iterator->getSourcePosition();
        for (int i {0}; i < 25; ++i) {//25 rays per detector
            indexes = raytracer->trace(source, positions.front());
            numIndex = indexes.size();
//            std::list<int> tmp = indexes;
//            std::cout << std::endl << "i : " << i << " => ";
//            for (int k{0}; k < numIndex; ++k) {
//                std::cout << tmp.front() << ", ";
//                tmp.pop_front();
//            }
            for (int j{0}; j < numIndex; ++j) {
//                std::cout << indexes.front() << " -> " << (*in)(indexes.front()) << "|";
                projection(offset + i) = projection(offset + i) + (*v)(indexes.front());
                indexes.pop_front();
            }
//            std::cout << std::endl;
            positions.pop_front();
        }
        numPose++;
    }
    return projection;
}

//v = result of a forward projection, returns a volume with all the voxels superheavy
Eigen::VectorXf normalReconstruction::backProjection(Eigen::VectorXf* v) {
//    std::cout << "backProjection" << std::endl;
    Eigen::VectorXf projection = Eigen::VectorXf::Zero(1000);//fixed value because I know the volume is finna be 1000 Voxels big
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    std::list<int> indexes;
    int numIndex;
    int numPose = 0;
    int offset;
    Eigen::Vector3f source;
    for (std::list<Gantry>::iterator iterator = poses.begin(), end = poses.end(); iterator != end; ++iterator) {
        positions = iterator->getDetectorPositions();
        offset = numPose*25;
        source = iterator->getSourcePosition();
        for (int i {0}; i < 25; ++i) {//25 rays per detector
            indexes = raytracer->trace(source, positions.front());
            numIndex = indexes.size();
//            std::list<int> tmp = indexes;
//            std::cout << std::endl << "i : " << i << " => ";
//            for (int k{0}; k < numIndex; ++k) {
//                std::cout << tmp.front() << ", ";
//                tmp.pop_front();
//            }
            for (int j{0}; j < numIndex; ++j) {
//                std::cout << indexes.front() << " -> " << (*in)(indexes.front()) << "|";
                projection(indexes.front()) = projection(indexes.front()) + (*v)(offset + i);
                indexes.pop_front();
            }
//            std::cout << std::endl;
            positions.pop_front();
        }
        numPose++;
    }
    return projection;
}
*/
