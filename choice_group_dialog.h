#ifndef CHOICE_GROUP_DIALOG_H
#define CHOICE_GROUP_DIALOG_H

#include "startup_data.h"

class Choice_Group_Dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QComboBox *comboBox;

    explicit Choice_Group_Dialog(QStringList groups, QWidget *parent = nullptr);

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file(){}

};

#endif // CHOICE_GROUP_DIALOG_H
