#pragma once

#include <QObject>
#include <QRunnable>
#include <QImage>

class Worker2  : public QObject, public QRunnable
{
	Q_OBJECT

public:
	Worker2() = default;
	Worker2(QImage);
	~Worker2();

	void run() override;

private:
	QImage image;

signals:
	void workReady(QByteArray);
};
