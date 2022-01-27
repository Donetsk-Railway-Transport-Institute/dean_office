#ifndef RECOGNITION_WINDOW_H
#define RECOGNITION_WINDOW_H

#include "startup_data.h"
#include <QMainWindow>

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>

namespace Ui {
class recognition_window;
}

class recognition_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit recognition_window(QWidget *parent = nullptr);
    ~recognition_window();

public slots:
    void on_Open_triggered();
    void on_control_BD_triggered();
    void on_Exit_triggered(){ close(); }

private:
    Ui::recognition_window *ui;
    startup_data *s_d;
    void open_doc(const QString &filename);
    QStringList rec_files;
    void rec_students(const QString &data, const QString &s_f_name);
};

#endif // RECOGNITION_WINDOW_H
