#include "Visualization/rendering.h"
#include <QPainter>
#include <QPaintEvent>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>


Rendering::Rendering(QWidget *parent) : QWidget(parent){
    setAttribute(Qt::WA_StaticContents);
    QPixmap* pm = new QPixmap(QSize(200,200));
    pm->fill(Qt::black);
    Image = pm->toImage();

    N = 1;
    sw = 0;
    cs = 0;
    good = false;
    get = TransferFunction();

    minimumSizeHint();
}

void Rendering::paintEvent(QPaintEvent *event) {
    QPainter ptr(this);
    QRect screen = event->rect();

    QImage tmp = Image.scaled(200,200);
    ptr.drawImage(screen, tmp, screen);

}

void Rendering::setVolume(Volume &volume) {
    this->volume = &volume;
    /*Eigen::VectorXf t = Eigen::VectorXf(1000);
    for (int i = 0; i < 500; i++) {
        t(i) = 255;
    }
    for (int i = 500; i < 1000; i++) {
        t(i) = 255;
    }
    this->volume->setContent(t);*/
    QPixmap* pm = new QPixmap(QSize(200,200));
    pm->fill(Qt::black);
    Image = pm->toImage();

}

void Rendering::setScreenPixels(size_t N) {
    this->N = N;
    this->content = Eigen::MatrixXf(N, N);
}

void Rendering::setScreenWidth(float sw) {
    this->sw = sw;
}

void Rendering::setScreenDistance(float cs) {
    this->cs = cs;
}

void Rendering::setCenter(Eigen::Vector3f center) {
    this->center = center;
    camera = center - Eigen::Vector3f(3*cs, 0, 0);
}

Eigen::Vector3f Rendering::getCamera() {
    Eigen::Vector2f tmp_cam(camera(0), camera(1));//rotation only around one axis
    tmp_cam = alpha*tmp_cam;
    Eigen::Vector3f cam(tmp_cam(0), tmp_cam(1), camera(2));
    return cam;
}

Eigen::Vector3f Rendering::getScreenCenter() {
    return (getCamera() - 2*cs*(getCamera() - center));
}

void Rendering::setPixelPositions() {//basically the same as in Gantry
    Eigen::Vector3f screenCenter = getScreenCenter();
    //get boundaries
    Eigen::Vector3f normal = (getCamera() - screenCenter).normalized();
    Eigen::Vector2f leftDirection = Eigen::Rotation2Df(3*M_PI/2.0) * Eigen::Vector2f(normal(0), normal(1));
    Eigen::Vector2f leftBorder = Eigen::Vector2f(screenCenter(0), screenCenter(1)) + sw/2 * leftDirection;
    Eigen::Vector2f rightBorder = Eigen::Vector2f(screenCenter(0), screenCenter(1)) - sw/2 * leftDirection;

    Eigen::Vector3f upperLeft(leftBorder(0), leftBorder(1), screenCenter(2) + sw/2);
    Eigen::Vector3f lowerLeft(leftBorder(0), leftBorder(1), screenCenter(2) - sw/2);
    Eigen::Vector3f lowerRight(rightBorder(0), rightBorder(1), screenCenter(2) - sw/2);


    //vectors to step through the detector plane
    Eigen::Vector3f stepHoe = (lowerRight - lowerLeft)/N;
    Eigen::Vector3f stepLilUzi = (lowerLeft - upperLeft)/N;


    //first vector to add
    Eigen::Vector3f first = upperLeft;


    pixelPositions = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>(N*N);
////    pixelPositions = std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>();

/*
    int nthreads = 4; //assume 4 cores or 2
    int i = N/nthreads;
    std::vector<std::thread> pixelThreads(nthreads);
    for (int k {0}; k < nthreads - 1; ++k) {
        pixelThreads[k] = std::thread(&Rendering::threadsSetPixelPositions, this, first, stepHoe, stepLilUzi, k*N*i, k*i, (k+1)*i);
    }
    //extra assignment in case i*nthreads != N
    pixelThreads[nthreads - 1] = std::thread(&Rendering::threadsSetPixelPositions, this, first, stepHoe, stepLilUzi, (nthreads - 1)*N*i, (nthreads - 1)*i, (nthreads)*i);

    for (int j{0}; j < nthreads; ++j) {
        pixelThreads[j].join();
    }*/

    int k = 0;
    for (int i{0}; i < N ; i++) {
        for (int j{0}; j < N; j++) {
////            pixelPositions.push_back(current + (i + 0.5)*stepLilUzi + (j + 0.5)*stepHoe);
            pixelPositions[k] = first + (i + 0.5)*stepLilUzi + (j + 0.5)*stepHoe;
            ++k;

        }
    }
}

void Rendering::threadsSetPixelPositions(Eigen::Vector3f start, Eigen::Vector3f stepHoe, Eigen::Vector3f stepLilUzi, int k, int iStart, int iEnd) {
    for (int i = iStart; i < iEnd; ++i) {
        for (int j {0}; j < N; ++j) {
            pixelPositions[k] = start + (i + 0.5)*stepLilUzi + (j + 0.5)*stepHoe;
            ++k;
        }
    }
}

void Rendering::setImage() {//call the transfer function here
    QPixmap* pm = new QPixmap(Image.size());
    pm->fill(Qt::black);
    Image = pm->toImage();
    QImage alpha = Image.alphaChannel();
    for (int i {0}; i < N*N; ++i) {
        float g = content(i%N, i/N);
        QColor color = get.classify(g);
        alpha.setPixel(i%N, i/N, 255 - color.alpha());
    }
    Image.setAlphaChannel(alpha);
    update();
}

void Rendering::calculateImage() {
    //create NxN rays and iterate over them
    //the biggest density is returned and the pixel set accordingly
    QPixmap* pm = new QPixmap(QSize(N,N));
    pm->fill(Qt::black);
    Image = pm->toImage();

    setPixelPositions();
    Eigen::Vector3f entry;
    Eigen::Vector3f exit;
    Eigen::Vector3f cam = getCamera();
    Eigen::Vector3f way;
////    int i = 0;
////    for (std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>::iterator start = pixelPositions.begin(), end = pixelPositions.end(); start != end; ++start) {
////        way = (*start - cam).normalized();
///
//   for (int i {0}; i < N*N; ++i) {
//       way = (pixelPositions[i] - cam).normalized();
//       entry = getEntry(cam, cam + 4*cs*way);
//       exit = getEntry(cam + 4*cs*way, cam);
//       content(i%N, i/N) = trace(entry, exit);
//    //////        ++i;
//    }
    //array of threads
    std::vector<std::thread> calcThreads(16);
    std::cout << "creating" << std::endl;
    for (int i {0}; i < 16; ++i) {
        calcThreads[i] = std::thread(&Rendering::threadsCalculateImage, this, cam, i*N*N/16, (i+1)*N*N/16);
    }

    for (int i {0}; i < 16; ++i) {
        calcThreads[i].join();
    }
    setImage();
}

void Rendering::threadsCalculateImage(Eigen::Vector3f cam, int iMin, int iMax) {
    Eigen::Vector3f entry;
    Eigen::Vector3f exit;
    Eigen::Vector3f way;
    for (int i = iMin; i < iMax; ++i) {
        way = (pixelPositions[i] - cam).normalized();
        entry = getEntry(cam, cam + 4*cs*way);
        exit = getEntry(cam + 4*cs*way, cam);
        content(i%N, i/N) = trace(entry, exit);
    }
    std::cout << "finished : " << iMin*16/(N*N) << std::endl;
}

float Rendering::trace(Eigen::Vector3f from, Eigen::Vector3f to) {
    if (from == Eigen::Vector3f(-1,-1,-1)) return 0;

    float val = volume->getVoxelLinear(from(0), from(1), from(2));
    float tmp;
    Eigen::Vector3f way = (to - from).normalized();
    int steps = (int) ((to - from).norm()/step);
    for (int i{0}; i < steps + 1; ++i) {
        from = from + way*step;
        tmp = volume->getVoxelLinear(from(0)/0.015, from(1)/0.015, from(2)/0.025);
        if (val < tmp) {
            val = tmp;
        }
    }
    return val;
}

//this is the same code as in Raytracer.cpp, I copied it to be able to parallelize it
//with no interference with the other parts of the project
Eigen::Vector3f Rendering::getEntry(Eigen::Vector3f from, Eigen::Vector3f to) {
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

void Rendering::setStep(float step) {
    this->step = step/1000.0;

    if (good) {
        calculateImage();
        update();
    }
}

void Rendering::setRotation(float angle) {

    this->alpha = Eigen::Rotation2Df(angle/180 * M_PI);

    if (good) {
        calculateImage();
        update();
    }
}

void Rendering::free() {
    good = true;
}

void Rendering::lock() {
    good = false;
}

void Rendering::transMin(float min) {
    get.setMin(min);
    if(volume!=nullptr)
    setImage();
}

void Rendering::transMax(float max) {
    get.setMax(max);
    if(volume!=nullptr)
    setImage();
}
