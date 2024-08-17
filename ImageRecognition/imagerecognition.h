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

    Camera* camera;                                         // 相机对象
    QLabel* label;                                          // 展示摄像头画面
    QPushButton* btnCapture;                                // 检测按钮
    QTextBrowser* textBrowser;                              // 展示检测的内容信息
    QComboBox* box;                                         // 切换摄像头
    QHBoxLayout* hLayout;                                   // 水平布局
    QVBoxLayout* vLeft, *vRight;                            // 左右垂直布局
    QTimer* refreshTimer;                                   // 刷新画面的定时器
    QImage image;                                           // 保存摄像头的照片
    QTimer* tokenReceivedTimer;                             // 拿到access_token后定时器，定时准备发送当前图片进行识别

    Worker* worker;                                         // 工人
    QThread* workThread;                                    // 工作线程

    Worker2* worker2;

    /*脸部信息*/
    int lastTime;
    double age;
    double beauty;
    QString emotion;
    QString gender;
    int mask;

    /*人脸位置*/
    double height;
    double left;
    double width;
    double top;

    QThreadPool* pool;
    NetWorkHandle* handle;                                  //处理网络请求
    void setImageLayout();                                  // 设置页面布局

public:
    ImageRecognition(QWidget *parent = nullptr);
    ~ImageRecognition();

signals:
    void beginWork(QImage);
    void beginWork(QImage, QThread*);                       // 通知工人干活的信号

private slots:
    void takePicture();                                     // 获取照相机图片的槽函数
    void showPicture(int, QImage);                          // 展示获取到的图片的槽函数
    void tokenReceived(QNetworkReply*);                     // 发送请求返回reply,解析得到accessToken的槽函数
    void preparePostData();                                 // 准备检测前的数据准备
    void preparePostData2();                                // 

    void beginPost(QByteArray, QThread*);                   // 开始识别
    void beginPost2(QByteArray);                            // method threadpool
    void showImageInfo(QNetworkReply*);                     // 解析图片信息
    void changeCamera(int);                                 // 切换摄像头
};
