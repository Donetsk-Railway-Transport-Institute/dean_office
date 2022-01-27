#ifndef EDIT_SPECIALTY_DIALOG_H
#define EDIT_SPECIALTY_DIALOG_H

#include "startup_data.h"
#include "tt_widget.h"

class Edit_Specialty_Dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    tt_widget *table;

    explicit Edit_Specialty_Dialog(startup_data *s_data, QWidget *parent = nullptr);

public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    startup_data *s_d;
};

#endif // EDIT_SPECIALTY_DIALOG_H
