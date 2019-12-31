#include "F1Status.h"
#include <QApplication>

const QString &VERSION = "0.9";

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setApplicationName("F1Status");
    a.setApplicationVersion(VERSION);

    F1Status w;
    w.show();
    return a.exec();
}
