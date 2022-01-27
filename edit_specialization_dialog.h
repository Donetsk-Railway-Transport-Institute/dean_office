#ifndef EDIT_SPECIALIZATION_DIALOG_H
#define EDIT_SPECIALIZATION_DIALOG_H

#include <QtWidgets/QComboBox>

#include "startup_data.h"
#include "tt_widget.h"

class specialization_table_widget : public tt_widget {
Q_OBJECT
public:
    explicit specialization_table_widget(startup_data *s_data,
                                         QWidget *parent = nullptr);

public slots:
    void itemDoubleClicked(QTableWidgetItem *item);
    void itemSelectionChanged();


private:
    startup_data *s_d;
    struct EDIT_ITEM{
        int num_row; //номер строки в которой изменился код специальности
        int dean_row; //номер строки в которой изменился деканат
        QComboBox *comboBox_specialty;
        QComboBox *comboBox_dean;
        EDIT_ITEM():
            num_row(-1),
            dean_row(-1),
            comboBox_specialty(nullptr),
            comboBox_dean(nullptr){}
    } edit_item;
};

class Edit_Specialization_Dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    specialization_table_widget *table;

    explicit Edit_Specialization_Dialog(startup_data *s_data, QWidget *parent = nullptr);

public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    startup_data *s_d;
};

#endif // EDIT_SPECIALIZATION_DIALOG_H
