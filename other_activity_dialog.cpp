#include "other_activity_dialog.h"

other_activity_dialog::other_activity_dialog(Portfolio_Dialog *p_dlg, QWidget *parent) :
    R_Dialog(parent), p_d(p_dlg){
    setWindowTitle("Другие виды деятельности");
    resize(812, 521);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(450, 470, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    label_curr = new QLabel(this);
    label_curr->setGeometry(QRect(50, 20, 711, 16));
    label_curr->setText(p_d->name_nir_control);
    QFont l_font = label_curr->font();
    l_font.setBold(true);
    label_curr->setFont(l_font);
    table_curr = new tt_widget(p_d->head_nir_control, &p_d->nir_control, this);
    table_curr->setGeometry(QRect(10, 40, 791, 171));
    table_curr->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_curr,&tt_widget::save_table_to_file,this,&other_activity_dialog::save_table_to_file);

    label_kr = new QLabel(this);
    label_kr->setGeometry(QRect(60, 220, 711, 16));
    label_kr->setText(p_d->name_extra_edu_control);
    label_kr->setFont(l_font);
    table_kr = new tt_widget(p_d->head_extra_edu_control, &p_d->extra_edu_control, this);
    table_kr->setGeometry(QRect(10, 240, 791, 91));
    table_kr->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_kr,&tt_widget::save_table_to_file,this,&other_activity_dialog::save_table_to_file);

    label_pract = new QLabel(this);
    label_pract->setGeometry(QRect(60, 340, 711, 16));
    label_pract->setText(p_d->name_add_edu_control);
    label_pract->setFont(l_font);
    table_pract = new tt_widget(p_d->head_add_edu_control, &p_d->add_edu_control, this);
    table_pract->setGeometry(QRect(10, 360, 791, 91));
    table_pract->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_pract,&tt_widget::save_table_to_file,this,&other_activity_dialog::save_table_to_file);

    connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void other_activity_dialog::save_table_to_file(){
    if(check_change_data(table_curr,&p_d->nir_control))
        if(yes_msg_save("с результатами научно-исследовательской деятельности"))
            copy_table_to_data(table_curr,&p_d->nir_control);
    if(check_change_data(table_kr,&p_d->extra_edu_control))
        if(yes_msg_save("с результатами внеучебной деятельности"))
            copy_table_to_data(table_kr,&p_d->extra_edu_control);
    if(check_change_data(table_pract,&p_d->add_edu_control))
        if(yes_msg_save("со сведениями о дополнительном образовании"))
            copy_table_to_data(table_pract,&p_d->add_edu_control);
    p_d->save_data_to_xml();
}
