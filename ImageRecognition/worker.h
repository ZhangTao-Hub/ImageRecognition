#pragma once

#include <QObject>
#include <QImage>
#include <QThread>
#include <QByteArray>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>

class Worker  : public QObject
{
	Q_OBJECT

public:
	Worker();
	Worker(QObject *parent);

	~Worker();

signals:
	void workReady(QByteArray, QThread*);

public slots:
	void doWork(QImage, QThread*);
};
