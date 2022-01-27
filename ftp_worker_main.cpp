#include <QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QStyleFactory>
#include <QResource>

#include "ftp_worker.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QString sCurrDateTime="["+QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss")+"]";
    QString log_name = "ftp_worker_"+QDateTime::currentDateTime().toString("dd.MM.yy")+".log";
    QFile fMessFile(qApp->applicationDirPath() + "/log/"+log_name);
    if(!fMessFile.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream tsTextStream(&fMessFile);
    tsTextStream.setCodec("UTF-8");
    switch (type) {
        case QtDebugMsg:
            tsTextStream << QString("%1 Debug: %2\n").arg(sCurrDateTime).arg(msg);
            break;
        case QtInfoMsg:
            tsTextStream << QString("%1 Info: %2\n").arg(sCurrDateTime).arg(msg);
            break;
        case QtWarningMsg:
            tsTextStream << QString("%1 Warning: %2\n").arg(sCurrDateTime).arg(msg);
            break;
        case QtCriticalMsg:
            tsTextStream << QString("%1 Critical: %2\n").arg(sCurrDateTime).arg(msg);
            break;
        case QtFatalMsg: {
            tsTextStream << QString("%1 Fatal: %2\n").arg(sCurrDateTime).arg(msg);
            tsTextStream.flush();
            fMessFile.flush();
            fMessFile.close();
            exit(-1);
        }
    }
    tsTextStream.flush();
    fMessFile.flush();
    fMessFile.close();
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(edit_report);
    qInstallMessageHandler(myMessageOutput);

    //рендеринг экрана
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setStyle(QStyleFactory::create("Fusion")); // these lines before the next

    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_ru",QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    ftp_worker w;
    w.show();
    return a.exec();
}
