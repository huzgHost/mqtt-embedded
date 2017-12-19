#include <QCoreApplication>
#include <signal.h>
#include "demo/sub.h"
#include "demo/pub.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //startSub(argc, argv);
    startPub(argc, argv);

    return a.exec();
}
