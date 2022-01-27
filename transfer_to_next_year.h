#ifndef TRANSFER_TO_NEXT_YEAR_H
#define TRANSFER_TO_NEXT_YEAR_H

#include "portfolio_dialog.h"
#include "startup_data.h"
#include "tt_widget.h"

class No_Groups_Transfer : public QDialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    tt_widget *tableWidget;
    QLabel *label;
    QLabel *label_2;

    explicit No_Groups_Transfer(const QStringList &gr, QWidget *parent = nullptr);
};

class Transfer_To_Next_Year : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    tt_widget *ttw;

    QLabel *label;
    QLineEdit *lineEdit_year;
    QLabel *label_2;
    QLabel *label_b;
    QLabel *label_k;
    QLabel *label_o;
    QLabel *label_z;
    QLineEdit *lineEdit_ob;
    QLineEdit *lineEdit_ok;
    QLineEdit *lineEdit_zb;
    QLineEdit *lineEdit_zk;

    QVector<QStringList> data;

    explicit Transfer_To_Next_Year(startup_data *s_data, const QString &new_year, QWidget *parent = nullptr);
    void transfer_groups();

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    startup_data *s_d;

    void load_data_from_xml();
    void save_data_to_xml(const QString &xml_file_name, const student_data &sd);
};

#endif // TRANSFER_TO_NEXT_YEAR_H
