#ifndef VOLUME_H
#define VOLUME_H

#include <Eigen/Geometry>
#include <string>
#include <vector>


class Volume {
public:
    Volume(Eigen::Vector3f lowerLeft, Eigen::Vector3f upperRight, Eigen::Vector3f sp);
    Volume(){}
    Eigen::AlignedBox3f getBoundingBox() const;
    Eigen::Vector3f getSpacing() const;

    Eigen::Vector3i getNumVoxels() const;

    Eigen::VectorXf getContent() const;
    void setContent(const Eigen::VectorXf& c);
    void computeMatrix(); // computes the Matrix
    std::vector<Eigen::MatrixXf> getMatrix();

    float getVoxel(int x, int y, int z);
    float getVoxelLinear(float x, float y, float z);

    // ... other useful methods ...

private:
    Eigen::AlignedBox3f boundingBox;
    Eigen::Vector3f spacing;

    std::vector<Eigen::MatrixXf> contentMatrix; // contains computed Matrix out of content (given in linearized format)
    //     ^------(z)    ^--------(x,y)     vector determines the z coordinate .... maybe we look for a better format than vector
    Eigen::VectorXf content;
};

#endif // VOLUME_H
