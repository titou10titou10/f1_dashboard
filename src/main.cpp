#include "F1Status.h"
#include <QApplication>

const QString &VERSION = "1.2";

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setApplicationName("F1Dashboard");
    a.setApplicationVersion(VERSION);

    F1Status w;
    w.show();
    return a.exec();
}
