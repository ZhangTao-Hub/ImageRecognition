#include "worker.h"

Worker::Worker()
{

}

Worker::Worker(QObject *parent): QObject(parent)
{

}

// 工作函数实现
void Worker::doWork(QImage image, QThread* workThread)
{
	// 将当前图片转为base64编码
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	image.save(&buffer, "png");
	QString base64Img = byteArray.toBase64();

	// 封装请求体Body
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
