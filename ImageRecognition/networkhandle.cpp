#include "networkhandle.h"

NetWorkHandle::NetWorkHandle(QWidget *parent): QWidget(parent)
{
	tokenManager = new QNetworkAccessManager(this);
	imageManager = new QNetworkAccessManager(this);
	
	// url
	QUrl url("https://aip.baidubce.com/oauth/2.0/token");
	// url query
	QUrlQuery query;
	query.addQueryItem("grant_type", "client_credentials");
	query.addQueryItem("client_id", "UDaAukqhH18EbHxOsOPH9yv1");
	query.addQueryItem("client_secret", "nqvCukx63StxjBzF0c1bUA21X7uB7V1a");
	url.setQuery(query);
	
	// ÅäÖÃsslÐÅÏ¢
	QSslConfiguration sslConfiguration = QSslConfiguration::defaultConfiguration();
	sslConfiguration.setPeerVerifyMode(QSslSocket::QueryPeer);
	sslConfiguration.setProtocol(QSsl::TlsV1_2);

	QNetworkRequest request(url);
	request.setSslConfiguration(sslConfiguration);
	tokenManager->get(request);
}

NetWorkHandle::~NetWorkHandle()
{
	
}

QNetworkAccessManager* NetWorkHandle::getTokenManager()
{
	return tokenManager;
}

QNetworkAccessManager* NetWorkHandle::getImageManager()
{
	return imageManager;
}

void NetWorkHandle::setAccessToken(QString accessToken)
{
	this->accesToken = accessToken;
}

QString NetWorkHandle::getAccessToken()
{
	return accesToken;
}
