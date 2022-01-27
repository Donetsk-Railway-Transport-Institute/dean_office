#ifndef EDIT_GROUPS_DIALOG_H
#define EDIT_GROUPS_DIALOG_H

#include <QtWidgets/QComboBox>

#include "startup_data.h"
#include "tt_widget.h"

class group_table_widget : public tt_widget {
Q_OBJECT
public:
    explicit group_table_widget(startup_data *s_data, QWidget *parent = nullptr);

public slots:
    virtual void remove_row();
    void itemDoubleClicked(QTableWidgetItem *item);
    void itemSelectionChanged();

private:
    startup_data *s_d;

    struct ROW_COLUMN_COMBOBOX{
        int row;
        int column;
        QComboBox *cb;
        ROW_COLUMN_COMBOBOX():row(-1),column(-1),cb(nullptr){}
        ROW_COLUMN_COMBOBOX(int p_row, int p_column, QComboBox *p_cb){
            row = p_row; column = p_column; cb=p_cb;
        }
    };

    QVector<ROW_COLUMN_COMBOBOX> v_cb;
};


class Edit_Groups_Dialog: public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    group_table_widget *table;

    explicit Edit_Groups_Dialog(startup_data *s_data, QWidget *parent = nullptr);

public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    startup_data *s_d;
};

#endif // EDIT_GROUPS_DIALOG_H
