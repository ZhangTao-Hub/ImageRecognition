#include "imagerecognition.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageRecognition w;
    w.show();
    return a.exec();
}
