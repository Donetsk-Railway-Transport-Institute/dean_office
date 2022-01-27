#include "prev_edu_dialog.h"

prev_edu_dialog::prev_edu_dialog(
        Portfolio_Dialog *p_dlg,
        QWidget *parent):
    R_Dialog(parent),table(nullptr),p_d(p_dlg){
    setWindowTitle(p_d->name_prev_edu_table);
    resize(582, 539);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(210, 490, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    table = new tt_widget(p_d->prev_edu_table, &p_d->prevs_edu, this);
    table->setGeometry(QRect(0, 0, 581, 471));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table,&tt_widget::save_table_to_file,this,&prev_edu_dialog::save_table_to_file);

    connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void prev_edu_dialog::save_table_to_file(){
    if(check_change_data(table,&p_d->prevs_edu))
        if(yes_msg_save("со сведениями о предыдущем образовании"))
            copy_table_to_data(table,&p_d->prevs_edu);
    p_d->save_data_to_xml();
}
