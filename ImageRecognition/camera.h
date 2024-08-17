#pragma once

#include <QObject>
#include <QWidget>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

class Camera  : public QWidget
{
	Q_OBJECT

private:
	QCamera* camera;									// 照相机
	QCameraViewfinder* viewfinder;						// 取景器
	QCameraImageCapture* imageCapture;					// 镜头捕捉

public:
	Camera(QWidget *parent);
	Camera(const QByteArray& deviceName, QWidget* parent = nullptr);
	~Camera();

	void stop();										// 停止当前相机
	QCameraViewfinder* getViewfinder();					// 获取取景器
	QCameraImageCapture* getImageCapture();				// 获取镜头捕捉
	QList<QCameraInfo> getAllCameras();					// 获取在线的所有相机设备

};
