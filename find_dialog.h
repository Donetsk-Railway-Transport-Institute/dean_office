#ifndef FIND_DIALOG_H
#define FIND_DIALOG_H

#include "startup_data.h"

class Find_Dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLabel *label_gr;
    QComboBox *comboBox;
    QLineEdit *lineEdit;

    explicit Find_Dialog(startup_data *s_data, QWidget *parent = nullptr);
    //возвращает название группы выбранного студента
    QString get_choice_group_name(){
        return label_gr->text().right(l_gr.size()-1);
    }

public slots:
    void textChanged(const QString &text);
    void activated(int index);

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file(){}

private:
    startup_data *s_d;
    //копия списка студентов из startup_data
    //в списке st[0]-имя студента,st[1]-група
    QVector<QStringList> f_students;
    //вывод надписи группы по имени студента
    void gr_to_label(const QString &st);
    const QString l_gr = QString("Группа: ");
};

#endif // FIND_DIALOG_H
