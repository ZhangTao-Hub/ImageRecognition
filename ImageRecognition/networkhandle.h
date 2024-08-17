#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QString>
#include <QUrlQuery>
#include <QNetWorkRequest>
#include <QSslConfiguration>

class NetWorkHandle  : public QWidget
{
	Q_OBJECT

private:
	QNetworkAccessManager* tokenManager;		
	QNetworkAccessManager* imageManager;		

	QString accesToken;							//��Ȩ���ͨ��֤

public:
	NetWorkHandle(QWidget *parent);
	~NetWorkHandle();

	QNetworkAccessManager* getTokenManager();
	QNetworkAccessManager* getImageManager();

	void setAccessToken(QString);
	QString getAccessToken();
};
