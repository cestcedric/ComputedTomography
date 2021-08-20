#include "Reconstruction/projection.h"
#include <chrono>
#include <iostream>

Projection::Projection(std::list<Gantry> poses, Raytracer *raytracer) {
    this->poses = poses;
    this->raytracer = raytracer;
}

void Projection::setPoses(std::list<Gantry> poses) {
    this->poses = poses;
}

void Projection::setRaytracer(Raytracer *raytracer) {
    this->raytracer = raytracer;
}

/*
 *
 * greyed-out parts use a vector of positions instead of a list
 * which might be faster when there are, like, a lot of positions
 * because of, idk, not being able to cache or shit, idc
 *
 */

//v = volume data, returns a concatenated vector of all detectors
Eigen::VectorXf Projection::forwardProjection(Eigen::VectorXf *v) {
    Eigen::VectorXf projection = Eigen::VectorXf::Zero(25*poses.size());
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    /*
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    */
    std::list<int> indexes;
    int numPose = 0;
    int offset;
    Eigen::Vector3f source;
    for (std::list<Gantry>::iterator iterator = poses.begin(), end = poses.end(); iterator != end; ++iterator) {
        int i = 0;
        /*
        positions = iterator->gitGud();
        */
        positions = iterator->getDetectorPositions();
        offset = numPose*25;
        source = iterator->getSourcePosition();
        /*
        for (int i {0}; i < 25; i++) {
            indexes = raytracer->trace(source, positions[i]);
            for (std::list<int>::iterator it2 = indexes.begin(), end2 = indexes.end(); it2 != end2; ++it2) {
                projection[offset + i] = projection[offset + i] + (*v)[*it2];
            }
        }
        */
        for (std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>::iterator it1 = positions.begin(), end1 = positions.end(); it1 != end1; ++it1) {
            indexes = raytracer->trace(source, *it1);
            for (std::list<int>::iterator it2 = indexes.begin(), end2 = indexes.end(); it2 != end2; ++it2) {
                projection(offset + i) = projection(offset + i) + (*v)(*it2);
            }
            ++i;
        }
        numPose++;
    }
    return projection;
}

//v = result of a forward projection, returns a volume with all the voxels superheavy
Eigen::VectorXf Projection::backProjection(Eigen::VectorXf* v) {
    Eigen::VectorXf projection = Eigen::VectorXf::Zero(1000);//fixed value because I know the volume is finna be 1000 Voxels big
    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    /*
    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> positions;
    */
    std::list<int> indexes;
    int numPose = 0;
    int offset;
    Eigen::Vector3f source;
    for (std::list<Gantry>::iterator iterator = poses.begin(), end = poses.end(); iterator != end; ++iterator) {
        int i = 0;
        positions = iterator->getDetectorPositions();
        /*
        positions = iterator->gitGud();
        */
        offset = numPose*25;
        source = iterator->getSourcePosition();
        /*
        for (int i{0}; i < 25; ++i) {
            indexes = raytracer->trace(source, positions[i]);
            for (std::list<int>::iterator it2 = indexes.begin(), end2 = indexes.end(); it2 != end2; ++it2) {
                projection(*it2) = projection(*it2) + (*v)(offset + i);
            }
        }
        */
        for (std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>::iterator it1 = positions.begin(), end1 = positions.end(); it1 != end1; ++it1) {
            indexes = raytracer->trace(source, *it1);
            for (std::list<int>::iterator it2 = indexes.begin(), end2 = indexes.end(); it2 != end2; ++it2) {
                projection(*it2) = projection(*it2) + (*v)(offset + i);
            }
            ++i;
        }

        numPose++;
    }
    return projection;
}
