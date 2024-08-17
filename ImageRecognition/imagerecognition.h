#pragma once

#include <QtWidgets/QWidget>
#include "ui_imagerecognition.h"
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QComboBox>
#include <QVariant>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>
#include <QThread>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <QString>
#include <QUrlQuery>
#include <QJsonObject>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QThreadPool>
#include <QRunnable>
#include "public.h"
#include "worker.h"
#include "camera.h"
#include "worker2.h"
#include "networkhandle.h"

class ImageRecognition : public QWidget
{
    Q_OBJECT

private:   
    Ui::ImageRecognitionClass ui;

    Camera* camera;                                         // �������
    QLabel* label;                                          // չʾ����ͷ����
    QPushButton* btnCapture;                                // ��ⰴť
    QTextBrowser* textBrowser;                              // չʾ����������Ϣ
    QComboBox* box;                                         // �л�����ͷ
    QHBoxLayout* hLayout;                                   // ˮƽ����
    QVBoxLayout* vLeft, *vRight;                            // ���Ҵ�ֱ����
    QTimer* refreshTimer;                                   // ˢ�»���Ķ�ʱ��
    QImage image;                                           // ��������ͷ����Ƭ
    QTimer* tokenReceivedTimer;                             // �õ�access_token��ʱ������ʱ׼�����͵�ǰͼƬ����ʶ��

    Worker* worker;                                         // ����
    QThread* workThread;                                    // �����߳�

    Worker2* worker2;

    /*������Ϣ*/
    int lastTime;
    double age;
    double beauty;
    QString emotion;
    QString gender;
    int mask;

    /*����λ��*/
    double height;
    double left;
    double width;
    double top;

    QThreadPool* pool;
    NetWorkHandle* handle;                                  //������������
    void setImageLayout();                                  // ����ҳ�沼��

public:
    ImageRecognition(QWidget *parent = nullptr);
    ~ImageRecognition();

signals:
    void beginWork(QImage);
    void beginWork(QImage, QThread*);                       // ֪ͨ���˸ɻ���ź�

private slots:
    void takePicture();                                     // ��ȡ�����ͼƬ�Ĳۺ���
    void showPicture(int, QImage);                          // չʾ��ȡ����ͼƬ�Ĳۺ���
    void tokenReceived(QNetworkReply*);                     // �������󷵻�reply,�����õ�accessToken�Ĳۺ���
    void preparePostData();                                 // ׼�����ǰ������׼��
    void preparePostData2();                                // 

    void beginPost(QByteArray, QThread*);                   // ��ʼʶ��
    void beginPost2(QByteArray);                            // method threadpool
    void showImageInfo(QNetworkReply*);                     // ����ͼƬ��Ϣ
    void changeCamera(int);                                 // �л�����ͷ
};
