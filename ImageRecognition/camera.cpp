#include "camera.h"

Camera::Camera(QWidget* parent): QWidget(parent)
{
	// ���������ȡ�������������ȡ����
	camera = new QCamera(this);
	viewfinder = new QCameraViewfinder(this);
	camera->setViewfinder(viewfinder);

	// ���������ͼ��׽
	imageCapture = new QCameraImageCapture(camera, this);

	// ��������Ĳ�׽��ʽ���Լ���׽��ַ
	camera->setCaptureMode(QCamera::CaptureStillImage);
	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	// �������
	camera->start();
}

Camera::Camera(const QByteArray& deviceName, QWidget* parent)
	: QWidget(parent)
{
	// �����豸���ƴ��������ȡ�������������ȡ����
	camera = new QCamera(deviceName, parent);
	viewfinder = new QCameraViewfinder(this);
	camera->setViewfinder(viewfinder);

	// ���������ͼ��׽
	imageCapture = new QCameraImageCapture(camera, this);

	// ��������Ĳ�׽��ʽ���Լ���׽��ַ
	camera->setCaptureMode(QCamera::CaptureStillImage);
	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	// �������
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
