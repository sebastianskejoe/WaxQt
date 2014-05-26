#include "waxqt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("WaxQt");
    QCoreApplication::setOrganizationName("au");

    WaxQt w;

    w.show();

    return a.exec();
}
