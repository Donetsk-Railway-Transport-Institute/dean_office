#ifndef ORDER_RECOGNITION_H
#define ORDER_RECOGNITION_H

#include "portfolio_dialog.h"
#include "startup_data.h"

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>

class Change_Curr_Year : public QDialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *lineEdit;

    explicit Change_Curr_Year(startup_data *s_data, QWidget *parent = nullptr);

private:
    startup_data *s_d;
};

class Order_Recognition : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QString years;

    explicit Order_Recognition(startup_data *s_data, QWidget *parent = nullptr);

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file(){}

private:
    void open_all();
    void control_BD();

    startup_data *s_d;
    void open_doc(const QString &filename);
    QStringList rec_files;
    void rec_students(const QString &data, const QString &s_f_name);
};

#endif // ORDER_RECOGNITION_H
