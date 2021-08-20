#include "volume.h"
#include <iostream>
using namespace Eigen;

Volume::Volume(Eigen::Vector3f lowerLeft, Eigen::Vector3f upperRight, Eigen::Vector3f sp):
    boundingBox{AlignedBox3f(lowerLeft,upperRight)}, spacing{sp}
{
    //initialization of bounding box with min/max and of the spacing
}

void Volume::setContent(const Eigen::VectorXf &c)
{
  //setting content to the
    content = c;
}

Eigen::VectorXf Volume::getContent() const
{
    return content;
}

Eigen::AlignedBox3f Volume::getBoundingBox() const
{
    return boundingBox;
}

Eigen::Vector3i Volume::getNumVoxels() const
{
    // @return: (NX,NY,NZ) according to the definition in ex2
    AlignedBox3f box = getBoundingBox();
    Vector3f leftDown = box.min();
    Vector3f RightUp = box.max();
    return Vector3i((RightUp(0)-leftDown(0))/spacing(0),(RightUp(1)-leftDown(1))/spacing(1),(RightUp(2)-leftDown(2))/spacing(2));
}
Eigen::Vector3f Volume::getSpacing() const
{
    return spacing;
}

void Volume::computeMatrix()
{
    // can be tested with the cout statements
    // output looks fine for me
    Vector3i num = getNumVoxels();
    contentMatrix.clear();
    //std::cout << "num"<<std::endl;
    for(int i = 0; i < num(2); ++i)
    {
        contentMatrix.push_back(MatrixXf::Zero(num(0),num(1)));
        //std::cout << "content " << i << std::endl;
        for (int k = 0; k < num(0); ++k)
        {
            for(int j=0; j < num(1); ++j)
            {
              //  std::cout << "matrix " << k << "," << j << ","<< i <<std::endl;
                 contentMatrix[i](k,j) = content(num(0)*num(1)*i + j * num(0) + k);
            }
        }
    }
}
std::vector<Eigen::MatrixXf> Volume::getMatrix()
{
    return contentMatrix;
}

float Volume::getVoxel(int x, int y, int z)
{
    Vector3i num = getNumVoxels();
    return content(num(0)*num(1)*z + num(0)*y +x);
}

float Volume::getVoxelLinear(float x, float y, float z)
{
    if(x > 9.0 || y > 9.0 || z > 9.0 || x < 0 || y < 0 || z<0)
        
        return 0;
    //sufficient for our interpolation task, haters gonna hate anyway => if x0 = 9 then x0+1 would be 10 and causing index out of bounds
    if(x == 9.0) x = 8.999999;
    if(y == 9.0) y = 8.999999;
    if(z == 9.0) z = 8.999999;
    //cutting off the fractional digits
    int x0 = (int) x;
    int y0 = (int) y;
    int z0 = (int) z;
    // very clear and understandable version of trilinear interpolation => if you like complicated stuff you can put it into one lambda expression ^^
    Vector3i C000 = Vector3i(x0,y0,z0);
    Vector3i C100 = Vector3i(x0+1,y0,z0);
    Vector3i C010 = Vector3i(x0,y0+1,z0);
    Vector3i C110 = Vector3i(x0+1,y0+1,z0);

    // bottom xy-plane linear interpolation in x-direction
    Vector3f C00 = Vector3f(x,y0,z0);
    float tx1 = (x-C000(0))/(C100(0)-C000(0));
    float C00_value = (1-tx1)*getVoxel(C000(0),C000(1),C000(2)) + tx1*getVoxel(C100(0),C100(1),C100(2));

    float tx2 = (x-C010(0))/(C110(0)-C010(0));
    float C10_value = (1-tx2)*getVoxel(C010(0),C010(1),C010(2)) + tx2*getVoxel(C110(0),C110(1),C110(2));

    // bottom xy-plane linear interpolation in y-direction
    Vector3f C10 = Vector3f(x,y0+1,z0);
    Vector3f C0 = Vector3f(x,y,z0);

    float ty1 = (y-C00(1))/(C10(1)-C00(1));
    float C0_value = (1-ty1)*C00_value + ty1*C10_value;

    Vector3i C001 = Vector3i(x0,y0,z0+1);
    Vector3i C101 = Vector3i(x0+1,y0,z0+1);
    Vector3i C011 = Vector3i(x0,y0+1,z0+1);
    Vector3i C111 = Vector3i(x0+1,y0+1,z0+1);

    //top xy-plane linear interpolation in x-direction
    Vector3f C01 = Vector3f(x,y0,z0+1);
    Vector3f C11 = Vector3f(x,y0+1,z0+1);
    float tx3 = (x-C001(0))/(C101(0)-C001(0));
    float C01_value = (1-tx3)*getVoxel(C001(0),C001(1),C001(2)) + tx3*getVoxel(C101(0),C101(1),C101(2));

    float tx4 = (x-C011(0))/(C111(0)-C011(0));
    float C11_value = (1-tx4)*getVoxel(C011(0),C011(1),C011(2)) + tx4*getVoxel(C111(0),C111(1),C111(2));
    Vector3f C1 = Vector3f(x,y,z0+1);

    // top xy-plane linear interpolation in y-direction
    float ty2 = (y-C01(1))/(C11(1)-C01(1));
    float C1_value = (1-ty2)*C01_value + ty2*C11_value;

    // interpolation in z-direction
    float tz = (z-C0(2))/(C1(2)-C0(2));
    float C_value = (1-tz)*C0_value + tz*C1_value;

    return C_value;
}






