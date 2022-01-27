#ifndef OTHER_ACTIVITY_DIALOG_H
#define OTHER_ACTIVITY_DIALOG_H

#include "portfolio_dialog.h"
#include "tt_widget.h"

//учебная деятельность
class other_activity_dialog : public R_Dialog {
    Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_curr;
    tt_widget *table_curr;
    QLabel *label_kr;
    tt_widget *table_kr;
    QLabel *label_pract;
    tt_widget *table_pract;
    QLabel *label_dp;
    tt_widget *table_dp;

    explicit other_activity_dialog(Portfolio_Dialog *p_dlg, QWidget *parent=nullptr);

public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();

private:
    Portfolio_Dialog *p_d;
};

#endif // OTHER_ACTIVITY_DIALOG_H
