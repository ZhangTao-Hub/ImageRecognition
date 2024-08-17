#include "camera.h"

Camera::Camera(QWidget* parent): QWidget(parent)
{
	// 创建相机，取景器；设置相机取景器
	camera = new QCamera(this);
	viewfinder = new QCameraViewfinder(this);
	camera->setViewfinder(viewfinder);

	// 设置相机的图像捕捉
	imageCapture = new QCameraImageCapture(camera, this);

	// 设置相机的捕捉方式，以及捕捉地址
	camera->setCaptureMode(QCamera::CaptureStillImage);
	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	// 启动相机
	camera->start();
}

Camera::Camera(const QByteArray& deviceName, QWidget* parent)
	: QWidget(parent)
{
	// 根据设备名称创建相机，取景器；设置相机取景器
	camera = new QCamera(deviceName, parent);
	viewfinder = new QCameraViewfinder(this);
	camera->setViewfinder(viewfinder);

	// 设置相机的图像捕捉
	imageCapture = new QCameraImageCapture(camera, this);

	// 设置相机的捕捉方式，以及捕捉地址
	camera->setCaptureMode(QCamera::CaptureStillImage);
	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	// 启动相机
	camera->start();
}

Camera::~Camera()
{

}



void Camera::stop()
{
	camera->stop();
}

QCameraViewfinder* Camera::getViewfinder()
{
	return viewfinder;
}

QCameraImageCapture* Camera::getImageCapture()
{
	return imageCapture;
}

QList<QCameraInfo> Camera::getAllCameras()
{
	return QCameraInfo::availableCameras();
}
