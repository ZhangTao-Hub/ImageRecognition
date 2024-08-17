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
	QCamera* camera;									// �����
	QCameraViewfinder* viewfinder;						// ȡ����
	QCameraImageCapture* imageCapture;					// ��ͷ��׽

public:
	Camera(QWidget *parent);
	Camera(const QByteArray& deviceName, QWidget* parent = nullptr);
	~Camera();

	void stop();										// ֹͣ��ǰ���
	QCameraViewfinder* getViewfinder();					// ��ȡȡ����
	QCameraImageCapture* getImageCapture();				// ��ȡ��ͷ��׽
	QList<QCameraInfo> getAllCameras();					// ��ȡ���ߵ���������豸

};
