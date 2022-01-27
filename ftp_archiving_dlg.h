#ifndef FTP_ARCHIVING_DLG_H
#define FTP_ARCHIVING_DLG_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QFile>
#include <QMap>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QTextCodec>

#include "startup_data.h"
#include "qftp.h"

#include <QtCore/QThread>

QT_BEGIN_NAMESPACE
class worker_script;
QT_END_NAMESPACE


class ftp_archiving_dlg : public QDialog {
    Q_OBJECT
public:
    explicit ftp_archiving_dlg(startup_data *s_data,
                               QWidget *parent = nullptr);

    QFtp *ftp;
    QStringList ftp_file_list;

/*
    //params[0] - титул окна
    //params[1] - локальный каталог
    //params[2] - url FTP (с пользователем и паролем)
    //params[3] - длительность sleep между FTP командами
    explicit ftp_archiving_dlg(QStringList params,
                               QWidget *parent = nullptr);

    QFtp *ftp;
    QString curr_dir;
    QString str_url;
    QStringList ftp_file_list;
*/
signals:
    void curr_command_finished();

public slots:
    void downloadFile(const QString &fileName);
    void put(const QByteArray &data, const QString &file);
    void finished();

private slots:
    void connectOrDisconnect();
    void cancelDownload();
    void connectToFtp();
    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);

private:
    startup_data *s_d;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QProgressBar *progressBar;

    QFile *file;
    QNetworkSession *networkSession;
    QNetworkConfigurationManager manager;
    worker_script *workerThread;
    QTextCodec *loc_codec;

    //Формирование FTP комманд как результат рекурсивного
    //поиска каталогов/файлов начиная с "dean_data/"+s_d->curr_year.
    QStringList parse_commands();
};

class worker_script : public QThread
{
    Q_OBJECT
public:
    explicit worker_script(QStringList commands,
                           ftp_archiving_dlg *fad,
                           QObject *parent = nullptr) :
            QThread(parent),str_command(commands),
            ftp_a_dlg(fad),curr_command_activated(false){}

    void run() override;

public slots:
    void curr_command_finished(){ curr_command_activated = false; }

signals:
    void download(const QString &s);
    void mkdir(const QString &s);
    void cd(const QString &s);
    void put(const QByteArray &data, const QString &file);
    void set_progress_value(int);
    void finished();
    void msg(const QString &s);

private:
    QStringList str_command;
    ftp_archiving_dlg *ftp_a_dlg;
    bool curr_command_activated;
};

#endif // FTP_ARCHIVING_DLG_H
