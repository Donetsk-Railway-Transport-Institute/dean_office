#ifndef RECOGNITION_WINDOW_H
#define RECOGNITION_WINDOW_H

#include "startup_data.h"
#include <QMainWindow>

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>

namespace Ui {
class ftp_worker;
}

class ftp_worker : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftp_worker(QWidget *parent = nullptr);
    ~ftp_worker();

public slots:
    void on_put_data_triggered();
    void on_download_triggered();
    void on_exit_triggered(){ close(); }

private:
    Ui::ftp_worker *ui;
    startup_data *s_d;
};

#endif // RECOGNITION_WINDOW_H
