#ifndef PREV_EDU_DIALOG_H
#define PREV_EDU_DIALOG_H

#include "portfolio_dialog.h"
#include "tt_widget.h"

class Portfolio_Dialog;

class prev_edu_dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    tt_widget *table;

    explicit prev_edu_dialog(Portfolio_Dialog *p_dlg, QWidget *parent=nullptr);

public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    Portfolio_Dialog *p_d;
};

#endif // PREV_EDU_DIALOG_H
