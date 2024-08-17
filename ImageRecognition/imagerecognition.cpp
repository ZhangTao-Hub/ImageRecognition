#include "imagerecognition.h"

void ImageRecognition::setImageLayout()
{
    // ���ò��ֿؼ�
    label = new QLabel(this);
    label->setFixedSize(640,480);
    btnCapture = new QPushButton("Detect", this);
    textBrowser = new QTextBrowser(this);
    textBrowser->setFixedSize(250, 300);
    box = new QComboBox(this);

    // �鿴���е�����豸
    QList<QCameraInfo> allCameras= camera->getAllCameras();
    for (const auto& item : allCameras)
    {
        box->addItem(item.description(), QVariant(item.deviceName()));
    }

    // ����,�����Ҳ��֣������²���
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

    // ���ҳ�沼��
    setImageLayout();

    // change camera
    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImageRecognition::changeCamera);

    // ����ͷ�����ȡ
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &ImageRecognition::takePicture);
    refreshTimer->start(20);

    // ����ʱ����ʱ��õ���Ƭ,չʾ��label��
    connect(camera->getImageCapture(), &QCameraImageCapture::imageCaptured, this, &ImageRecognition::showPicture);

    // �����������
    // ����get�����ȡaccess_token
    handle = new NetWorkHandle(this);
    connect(handle->getTokenManager(), &QNetworkAccessManager::finished, this, &ImageRecognition::tokenReceived);

    // �õ�token�󣬶�ʱ׼��post���������
    tokenReceivedTimer = new QTimer(this);
    connect(tokenReceivedTimer, &QTimer::timeout, this, &ImageRecognition::preparePostData);
    
    // method2 using threadpool
    //connect(tokenReceivedTimer, &QTimer::timeout, this, &ImageRecognition::preparePostData2);

    // post������ϣ��������ص�ͼƬ��Ϣ����
    connect(handle->getImageManager(), &QNetworkAccessManager::finished, this, &ImageRecognition::showImageInfo);
}

void ImageRecognition::takePicture()
{
    // ����ǰͼƬ
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

    // ���Ա߱�ע����Ϣ
    p.drawText(left + width + 5, top, QString("%1").arg(age));
    p.drawText(left + width + 5, top + 40, QString("%1").arg(gender));
    p.drawText(left + width + 5, top + 80, QString("%1").arg(emotion));
    p.drawText(left + width + 5, top + 120, QString("%1").arg(beauty));
    label->setPixmap(QPixmap::fromImage(this->image));
}

void ImageRecognition::tokenReceived(QNetworkReply* reply)
{
    // �жϷ��������Ƿ����
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Token received error: " << reply->error();
        return;
    }

    // ��ȡ���е�����
    QByteArray info = reply->readAll();

    // json�������ص�����
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

    // access_token�õ���������ʱ����׼������������м��
    tokenReceivedTimer->start(1500);
}

void ImageRecognition::preparePostData()
{
    worker = new Worker;
    workThread = new QThread;
    worker->moveToThread(workThread);

    // �����̹߳���
    connect(this, QOverload<QImage, QThread*>::of(&ImageRecognition::beginWork), worker, &Worker::doWork);
    connect(worker, &Worker::workReady, this, &ImageRecognition::beginPost);
    
    // ������Դ
    connect(workThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workThread, &QThread::finished, workThread, &QObject::deleteLater);

    // �����̣߳������ź�
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
    * ���߳�Ƶ���Ĵ��������ٿ���ʹ���̳߳ؽ����Ż�
    */
    // ���߳�ֹͣ
    workThread->quit();
    workThread->wait();
    workThread = nullptr;
    worker = nullptr;

    // ����post���󣬽���ʶ��
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
    // ����post���󣬽���ʶ��
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query(url);
    query.addQueryItem("access_token", handle->getAccessToken());
    url.setQuery(query);

    qDebug() << url;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    handle->getImageManager()->post(request, postBodyData);
}

// ����ͼƬ��Ϣ����ȡ��Ϣ
void ImageRecognition::showImageInfo(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        LOG("replied image info error");
        return;
    }
    QByteArray replyData = reply->readAll();

    // ����json
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        LOG("Json Parse Error");
        return;
    }
    
    QJsonObject jsonObj = jsonDoc.object();
    // ��ȡʱ���
    if (jsonObj.contains("timestamp"))
    {
        int tempTime = jsonObj.take("timestamp").toInt();
        if (tempTime < lastTime)
        {
            return;
        }
        lastTime = tempTime;
    }
    
    // ��ȡ��һ��������Ϣ
    if (jsonObj.contains("result"))
    {
        QJsonObject resultObj = jsonObj.take("result").toObject();
        if (resultObj.contains("face_list"))
        {
            QJsonArray faceLists = resultObj.take("face_list").toArray();
            QJsonObject faceObj = faceLists.at(0).toObject();

            if (faceObj.contains("location"))
            {
                /*λ����Ϣ*/
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

// �л�����ͷ�豸
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


