#include "imagerecognition.h"

void ImageRecognition::setImageLayout()
{
    // 设置布局控件
    label = new QLabel(this);
    label->setFixedSize(640,480);
    btnCapture = new QPushButton("Detect", this);
    textBrowser = new QTextBrowser(this);
    textBrowser->setFixedSize(250, 300);
    box = new QComboBox(this);

    // 查看所有的相机设备
    QList<QCameraInfo> allCameras= camera->getAllCameras();
    for (const auto& item : allCameras)
    {
        box->addItem(item.description(), QVariant(item.deviceName()));
    }

    // 布局,先左右布局，再上下布局
    this->setFixedSize(1000, 600);
    vLeft = new QVBoxLayout();
    vLeft->addWidget(label);
    vLeft->addWidget(btnCapture);

    vRight = new QVBoxLayout();
    vRight->addWidget(box);
    vRight->addWidget(camera->getViewfinder());
    vRight->addWidget(textBrowser);

    hLayout = new QHBoxLayout();
    hLayout->addLayout(vLeft);
    hLayout->addLayout(vRight);
    this->setLayout(hLayout);
}

ImageRecognition::ImageRecognition(QWidget *parent)
    : QWidget(parent)
{

    // set gloabl threadpool ## method 2
    pool = QThreadPool::globalInstance();
    pool->setMaxThreadCount(4);

    ui.setupUi(this);
    camera = new Camera(this);

    // 设计页面布局
    setImageLayout();

    // change camera
    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImageRecognition::changeCamera);

    // 摄像头界面获取
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ImageRecognition::takePicture);
    refreshTimer->start(20);

    // 捕获定时器定时获得的照片,展示在label中
    connect(camera->getImageCapture(), &QCameraImageCapture::imageCaptured, this, &ImageRecognition::showPicture);

    // 网络请求对象
    // 发送get请求获取access_token
    handle = new NetWorkHandle(this);
    connect(handle->getTokenManager(), &QNetworkAccessManager::finished, this, &ImageRecognition::tokenReceived);

    // 拿到token后，定时准备post请求的数据
    tokenReceivedTimer = new QTimer(this);
    connect(tokenReceivedTimer, &QTimer::timeout, this, &ImageRecognition::preparePostData);
    
    // method2 using threadpool
    //connect(tokenReceivedTimer, &QTimer::timeout, this, &ImageRecognition::preparePostData2);

    // post请求完毕，解析返回的图片信息数据
    connect(handle->getImageManager(), &QNetworkAccessManager::finished, this, &ImageRecognition::showImageInfo);
}

void ImageRecognition::takePicture()
{
    // 捕获当前图片
    camera->getImageCapture()->capture();
}

void ImageRecognition::showPicture(int id, QImage image)
{
    Q_UNUSED(id);
    this->image = image;
    //label->setPixmap(QPixmap::fromImage(this->image));

    // draw rect
    QPen pen(Qt::red);
    pen.setWidth(5);
    QPainter p(&this->image);

    //QString loaction = QString("%1, %2, %3, %4").arg(left).arg(top).arg(width).arg(height);
    //textBrowser->setText(loaction);
    p.setPen(pen);
    p.drawRect(left, top, width, height);
    QFont font;
    font.setPixelSize(50);
    p.setFont(font);

    // 在旁边标注上信息
    p.drawText(left + width + 5, top, QString("%1").arg(age));
    p.drawText(left + width + 5, top + 40, QString("%1").arg(gender));
    p.drawText(left + width + 5, top + 80, QString("%1").arg(emotion));
    p.drawText(left + width + 5, top + 120, QString("%1").arg(beauty));
    label->setPixmap(QPixmap::fromImage(this->image));
}

void ImageRecognition::tokenReceived(QNetworkReply* reply)
{
    // 判断返回数据是否出错
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Token received error: " << reply->error();
        return;
    }

    // 读取所有的数据
    QByteArray info = reply->readAll();

    // json解析返回的数据
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(info, &error);
    if (error.error == QJsonParseError::NoError)
    {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains("access_token"))
        {
            handle->setAccessToken(jsonObj.take("access_token").toString());
            textBrowser->setText(handle->getAccessToken());
        }
    }
    else
    {
        LOG("Json Parse Error!");
        return;
    }

    reply->deleteLater();

    // access_token拿到后，启动定时器，准备发送请求进行检测
    tokenReceivedTimer->start(1500);
}

void ImageRecognition::preparePostData()
{
    worker = new Worker;
    workThread = new QThread;
    worker->moveToThread(workThread);

    // 工作线程工作
    connect(this, QOverload<QImage, QThread*>::of(&ImageRecognition::beginWork), worker, &Worker::doWork);
    connect(worker, &Worker::workReady, this, &ImageRecognition::beginPost);
    
    // 处理资源
    connect(workThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workThread, &QThread::finished, workThread, &QObject::deleteLater);

    // 开启线程，触发信号
    workThread->start();
    emit beginWork(this->image, workThread);
}

// use threadpool
void ImageRecognition::preparePostData2()
{
    worker2 = new Worker2(this->image);
    pool->start(worker2);
    connect(worker2, &Worker2::workReady, this, &ImageRecognition::beginPost2);
    connect(worker2, &Worker2::workReady, worker2, &QObject::deleteLater);
}

void ImageRecognition::beginPost(QByteArray postBodyData, QThread* workThread)
{
    /*
    * 子线程频繁的创建和销毁可以使用线程池进行优化
    */
    // 子线程停止
    workThread->quit();
    workThread->wait();
    workThread = nullptr;
    worker = nullptr;

    // 发送post请求，进行识别
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query(url);
    query.addQueryItem("access_token", handle->getAccessToken());
    url.setQuery(query);

    qDebug() << url;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    handle->getImageManager()->post(request, postBodyData);
}

// use threadpool 
void ImageRecognition::beginPost2(QByteArray postBodyData)
{
    // 发送post请求，进行识别
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query(url);
    query.addQueryItem("access_token", handle->getAccessToken());
    url.setQuery(query);

    qDebug() << url;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    handle->getImageManager()->post(request, postBodyData);
}

// 解析图片信息，提取信息
void ImageRecognition::showImageInfo(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        LOG("replied image info error");
        return;
    }
    QByteArray replyData = reply->readAll();

    // 解析json
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        LOG("Json Parse Error");
        return;
    }
    
    QJsonObject jsonObj = jsonDoc.object();
    // 提取时间戳
    if (jsonObj.contains("timestamp"))
    {
        int tempTime = jsonObj.take("timestamp").toInt();
        if (tempTime < lastTime)
        {
            return;
        }
        lastTime = tempTime;
    }
    
    // 提取第一张人脸信息
    if (jsonObj.contains("result"))
    {
        QJsonObject resultObj = jsonObj.take("result").toObject();
        if (resultObj.contains("face_list"))
        {
            QJsonArray faceLists = resultObj.take("face_list").toArray();
            QJsonObject faceObj = faceLists.at(0).toObject();

            if (faceObj.contains("location"))
            {
                /*位置信息*/
                QJsonObject locationObj = faceObj.take("location").toObject();
                //qDebug() << locationObj;
                if (locationObj.contains("left"))
                {
                    left = locationObj.take("left").toDouble();
                }

                if (locationObj.contains("top"))
                {
                    top = locationObj.take("top").toDouble();
                }

                if (locationObj.contains("width"))
                {
                    width = locationObj.take("width").toDouble();
                }

                if (locationObj.contains("height"))
                {
                    height = locationObj.take("height").toDouble();
                }
            }

            // age
            if (faceObj.contains("age"))
            {
                age = faceObj.take("age").toDouble();
            }

            // gender
            if (faceObj.contains("gender"))
            {
                QJsonObject genderObj = faceObj.take("gender").toObject();
                gender = genderObj.take("type").toString();
            }

            // beauty
            if (faceObj.contains("beauty"))
            {
                beauty = faceObj.take("beauty").toDouble();
            }

            // mask
            if (faceObj.contains("mask"))
            {
               QJsonObject maskObj = faceObj.take("mask").toObject();
               mask = maskObj.take("type").toInt();
            }

            // emotion
            if (faceObj.contains("emotion"))
            {
                QJsonObject emotionObj = faceObj.take("emotion").toObject();
                emotion = emotionObj.take("type").toString();
            }
        }
    }
}

// 切换摄像头设备
void ImageRecognition::changeCamera(int currentIndex)
{
    camera->stop();
    camera = nullptr;
    refreshTimer->stop();
    camera = new Camera(box->itemData(currentIndex).toByteArray());
    connect(camera->getImageCapture(), &QCameraImageCapture::imageCaptured, this, &ImageRecognition::showPicture);
    refreshTimer->start(20);
}

ImageRecognition::~ImageRecognition()
{
    pool->waitForDone();
}


