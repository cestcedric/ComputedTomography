#include "Raytracer.h"
#include <iostream>

Raytracer::Raytracer() {
}

void Raytracer::setVolume(Volume *volume) {
    this->volume = volume;
}

//returns a list of indexes of the voxels the ray is passing
//start = source; entry point into the volume is computed automatically
//end = detector pixel; exit point is computed automatically
std::list<int> Raytracer::trace(Eigen::Vector3f start, Eigen::Vector3f finish) {
    //pretty much from http://www.cs.yorku.ca/~amana/research/grid.pdf
    //also pretty much http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf page 327
    //but also with a load of stuff I had to invent
    Eigen::Vector3f begin = getIntersection(start, finish);
    Eigen::Vector3f end = getIntersection(finish, start);

    std::list<int> indexes;

    //see if begin is (-1, -1, -1), which would mean there is no intersection
    if (begin(0) == -1) {
        return indexes;
    }

    if ((begin - end).norm() == 0) {//only touching a corner
        return indexes;
    }

    int NforY = volume->getNumVoxels()(0);
    int NforZ = NforY * volume->getNumVoxels()(0);

//    NforY = 3;
//    NforZ = 9;

    float gridWidthX = volume->getSpacing()(0);
    float gridWidthY = volume->getSpacing()(1);
    float gridWidthZ = volume->getSpacing()(2);

//    gridWidthX = 1;
//    gridWidthY = 1;
//    gridWidthZ = 1;

    //initialization
    //coordinates of entry and exit of the bounding box in box space
    //=> distance when increasing z by 1 is bigger than increasing x by 1 in volume example
    int x = (int) (begin(0)/gridWidthX);
    int y = (int) (begin(1)/gridWidthY);
    int z = (int) (begin(2)/gridWidthZ);

    //direction of ray; left/right, up/down, front/back
    int stepX = ((begin(0) < end(0)) ? 1 : ((begin(0) > end(0)) ? -1 : 0));
    int stepY = ((begin(1) < end(1)) ? 1 : ((begin(1) > end(1)) ? -1 : 0));
    int stepZ = ((begin(2) < end(2)) ? 1 : ((begin(2) > end(2)) ? -1 : 0));

    //when starting on the surface of a voxel, see if the ray goes thru it
    //->set coordinates according to that, because start voxel is automtically
    //added to the resulting list
    if (stepX < 0 && x == begin(0)) --x;
    if (stepY < 0 && y == begin(1)) --y;
    if (stepZ < 0 && z == begin(2)) --z;

    //length of ray
    float dX = std::abs(end(0) - begin(0));
    float dY = std::abs(end(1) - begin(1));
    float dZ = std::abs(end(2) - begin(2));

    //amount of t it takes to traverse cell
    float tDeltaX = gridWidthX/dX;
    float tDeltaY = gridWidthY/dY;
    float tDeltaZ = gridWidthZ/dZ;

    //find distance to limits of current voxel
    float minX = gridWidthX * floorf(begin(0)/gridWidthX);
    if (stepX < 0 && (int) begin(0) == begin(0)) --minX;//starting on the surface of a voxel in negative direction
    float maxX = minX + gridWidthX;
    float tMaxX = ((begin(0) > end(0)) ? (begin(0) - minX) : (maxX - begin(0))/dX);

    float minY = gridWidthY * floorf(begin(1)/gridWidthY);
    if (stepY < 0 && (int) begin(1) == begin(1)) --minY;
    float maxY = minY + gridWidthY;
    float tMaxY = ((begin(1) > end(1)) ? (begin(1) - minY) : (maxY - begin(1))/dY);

    float minZ = gridWidthZ * floorf(begin(2)/gridWidthZ);
    if (stepZ < 0 && (int) begin(2) == begin(2)) --minZ;
    float maxZ = minZ + gridWidthZ;
    float tMaxZ = ((begin(2) > end(2)) ? (begin(2) - minZ) : (maxZ - begin(2))/dZ);

    bool xFinished = (stepX == 0);
    bool yFinished = (stepY == 0);
    bool zFinished = (stepZ == 0);
    bool goX = (!xFinished && tMaxX <= tMaxY && tMaxX <= tMaxZ);//a movement in x direction is done
    bool goY = (!yFinished && tMaxY <= tMaxX && tMaxY <= tMaxZ);
    bool goZ = (!zFinished && tMaxZ <= tMaxX && tMaxZ <= tMaxY);

    //if this condition is true the ray is only sliding on the surface of voxels, not actually going thru it
    if ((stepX == 0 && (fmod(begin(0), gridWidthX) == 0)) || (stepY == 0 && (fmod(begin(1), gridWidthY) == 0)) || (stepZ == 0 && (fmod(begin(2), gridWidthZ) == 0))) {
        return indexes;
    }

    int newIndex = x + y*NforY + z*NforZ;
    indexes.push_back(newIndex);//start voxel



    while (!xFinished || !yFinished || !zFinished) {//loop while inside the volume

        newIndex = x + y*NforY + z*NforZ;
        if (indexes.back() != newIndex) indexes.push_back(newIndex);//only add new voxels to the list
        //this is necessary, because sometimes extra cycles are made to set all of the _Finished variables
        //obviously one voxel can only be visited by one ray once

        if (goX) {//make a move in x direction
            tMaxX += tDeltaX;
            if ((stepX > 0 && (x + stepX)*gridWidthX < end(0))//increasing x still in box
                    || (stepX < 0 && (x + stepX)*gridWidthX >= end(0) && (x + stepX >= 0))) {//decreasing x still in box
            x += stepX;
            }   else    {
                xFinished = true;
            }
        }

        if (goY) {
            tMaxY += tDeltaY;
            if ((stepY > 0 && (y + stepY)*gridWidthY < end(1))
                    || (stepY < 0 && (y + stepY)*gridWidthY >= end(1) && (y + stepY >= 0))) {
                y += stepY;
            }   else    {
                yFinished = true;
            }
        }

        if (goZ) {
            tMaxZ += tDeltaZ;
            if ((stepZ > 0 && (z + stepZ)*gridWidthZ < end(2))
                    || (stepZ < 0 && (z + stepZ)*gridWidthZ >= end(2) && (z + stepZ >= 0))){
                z += stepZ;
            }   else    {
                zFinished = true;
            }
        }

        goX = (!xFinished && (tMaxX <= tMaxY || yFinished) && (tMaxX <= tMaxZ || zFinished));
        goY = (!yFinished && (tMaxY <= tMaxX || xFinished) && (tMaxY <= tMaxZ || zFinished));
        goZ = (!zFinished && (tMaxZ <= tMaxX || xFinished) && (tMaxZ <= tMaxY || yFinished));

    }
    return indexes;
}

//computes point where the ray from start to end enters the bounding box
Eigen::Vector3f Raytracer::getIntersection(Eigen::Vector3f from, Eigen::Vector3f to) {
    //pretty much http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf page 181

        Eigen::AlignedBox3f testBox = volume->getBoundingBox();
        Eigen::Vector3f direction = to - from;

        float tmin = 0.0;
        float tmax = std::numeric_limits<float>::max();

        for (int i{0}; i < 3; ++i) {
            if (std::abs(direction(i)) == 0.0) {//ray parallel to slab, e.g. horizontal ray
                if (from(i) < testBox.min()(i) || from(i) > testBox.max()(i)) {//also not inside of the area
                    return Eigen::Vector3f(-1, -1, -1);
                }
            }   else    {
                //intersection with this side and the one on the other side
                float ood = 1.0/direction(i);
                float t1 = (testBox.min()(i) - from(i))*ood;
                float t2 = (testBox.max()(i) - from(i))*ood;
                //t1 is the intersection when entering the volume
                if (t1 > t2) std::swap(t1, t2);
                //are the intersections in the same intervall as the ones with the other planes
                if (t1 > tmin) tmin = t1;
                if (t2 > tmax) tmax = t2;
                //if the intersections don't happen in the same intervall return no intersection
                if (tmin > tmax) return Eigen::Vector3f(-1,-1,-1);
            }
        }
        //ray intersects the volume, return point of entry into volume
        return (from + direction*tmin);
}
