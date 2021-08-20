#pragma once
#include <Eigen/Dense>
#include <QWidget>
#include <volume.h>
#include <thread>
#include <Visualization/TransferFunction.h>

class Rendering : public QWidget {
    Q_OBJECT

public:
    Rendering(QWidget *parent = 0);
    void setVolume(Volume &volume);
    void setScreenPixels(size_t N);
    void setScreenWidth(float sw);
    void setScreenDistance(float cs);
    void setCenter(Eigen::Vector3f center);
    void calculateImage();
    void free();
    void lock();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    Eigen::Rotation2Df alpha;
    Eigen::MatrixXf content;
    float step;
    float cs; //distance from camera to screen
    float sw; //screen width
    size_t N; //screen width in pixels
    Eigen::Vector3f camera;
    Eigen::Vector3f center;
    bool good;


    std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> pixelPositions;
////    std::list<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> pixelPositions;

    Volume *volume;
    QImage Image;
    TransferFunction get;

    Eigen::Vector3f getCamera();
    Eigen::Vector3f getScreenCenter();

    void setPixelPositions();
    void threadsSetPixelPositions(Eigen::Vector3f start, Eigen::Vector3f stepHoe, Eigen::Vector3f stepLilUzi, int k, int iStart, int iEnd);
    void threadsCalculateImage(Eigen::Vector3f cam, int iMin, int iMax);
    void setImage();

    float trace(Eigen::Vector3f from, Eigen::Vector3f to);
    Eigen::Vector3f getEntry(Eigen::Vector3f from, Eigen::Vector3f to);

public slots:
    void setStep(float step);
    void setRotation(float angle);
    void transMin(float min);
    void transMax(float max);
};
