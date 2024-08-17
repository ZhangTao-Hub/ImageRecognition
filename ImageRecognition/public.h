#pragma once
#include <QDebug>

#define LOG(message)\
	qDebug() << __FILE__ << ":" << __LINE__ << ":"\
			 << message;