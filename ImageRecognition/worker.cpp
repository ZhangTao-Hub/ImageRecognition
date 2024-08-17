#include "worker.h"

Worker::Worker()
{

}

Worker::Worker(QObject *parent): QObject(parent)
{

}

// ��������ʵ��
void Worker::doWork(QImage image, QThread* workThread)
{
	// ����ǰͼƬתΪbase64����
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	image.save(&buffer, "png");
	QString base64Img = byteArray.toBase64();

	// ��װ������Body
	QJsonObject jsonObj;
	QJsonDocument jsonDoc;
	jsonObj.insert("image", base64Img);
	jsonObj.insert("image_type", "BASE64");
	jsonObj.insert("face_field", "age,expression,face_shape,gender,glasses,quality,eye_status,emotion,face_type,mask,beauty");
	
	jsonDoc.setObject(jsonObj);
	QByteArray postBodyData = jsonDoc.toJson(QJsonDocument::Compact);

	qDebug() << postBodyData;
	emit workReady(postBodyData, workThread);
}

Worker::~Worker()
{

}
