/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "tkt_mainwindow.h"
#include <QApplication>

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

int main(int argc, char *argv[])
{
    vlg::syslog_load_config();
    QApplication a(argc, argv);
    qApp->setApplicationVersion(QUOTE(APP_VERSION));
    qApp->setApplicationName(QUOTE(APP_NAME));
    qApp->setOrganizationName("Vulgaris");
    qApp->setOrganizationDomain("www.vlg-tech.net");
    vlg_toolkit_MainWindow w;
    w.setWindowTitle(qApp->applicationName() + QString(" - ") +
                     qApp->applicationVersion() + " - " + __DATE__);
    w.show();
    return a.exec();
}
