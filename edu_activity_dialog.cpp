#include "edu_activity_dialog.h"

edu_activity_dialog::edu_activity_dialog(Portfolio_Dialog *p_dlg, QWidget *parent) :
    R_Dialog(parent), p_d(p_dlg){
    setWindowTitle("Учебная деятельность");
    resize(812, 641);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(450, 590, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    label_curr = new QLabel(this);
    label_curr->setGeometry(QRect(50, 20, 711, 16));
    label_curr->setText(p_d->name_result_control);
    QFont l_font = label_curr->font();
    l_font.setBold(true);
    label_curr->setFont(l_font);
    table_curr = new tt_widget(p_d->head_result_control, &p_d->result_control, this);
    table_curr->setGeometry(QRect(10, 40, 791, 171));
    table_curr->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_curr,&tt_widget::save_table_to_file,this,&edu_activity_dialog::save_table_to_file);

    label_kr = new QLabel(this);
    label_kr->setGeometry(QRect(60, 220, 711, 16));
    label_kr->setText(p_d->name_kr_control);
    label_kr->setFont(l_font);
    table_kr = new tt_widget(p_d->head_kr_control, &p_d->kr_control, this);
    table_kr->setGeometry(QRect(10, 240, 791, 91));
    table_kr->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_kr,&tt_widget::save_table_to_file,this,&edu_activity_dialog::save_table_to_file);

    label_pract = new QLabel(this);
    label_pract->setGeometry(QRect(60, 340, 711, 16));
    label_pract->setText(p_d->name_pract_control);
    label_pract->setFont(l_font);
    table_pract = new tt_widget(p_d->head_pract_control, &p_d->pract_control, this);
    table_pract->setGeometry(QRect(10, 360, 791, 91));
    table_pract->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_pract,&tt_widget::save_table_to_file,this,&edu_activity_dialog::save_table_to_file);

    label_dp = new QLabel(this);
    label_dp->setGeometry(QRect(60, 460, 711, 16));
    label_dp->setText(p_d->name_dp_control);
    label_dp->setFont(l_font);
    table_dp = new tt_widget(p_d->head_dp_control, &p_d->dp_control, this);
    table_dp->setGeometry(QRect(10, 480, 791, 91));
    table_dp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table_dp,&tt_widget::save_table_to_file,this,&edu_activity_dialog::save_table_to_file);

    connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void edu_activity_dialog::save_table_to_file(){
    if(check_change_data(table_curr,&p_d->result_control))
        if(yes_msg_save("с результатами текущего контроля"))
            copy_table_to_data(table_curr,&p_d->result_control);
    if(check_change_data(table_kr,&p_d->kr_control))
        if(yes_msg_save("с результатами защиты курсовых работ"))
            copy_table_to_data(table_kr,&p_d->kr_control);
    if(check_change_data(table_pract,&p_d->pract_control))
        if(yes_msg_save("со сведениями о прохождении практики"))
            copy_table_to_data(table_pract,&p_d->pract_control);
    if(check_change_data(table_dp,&p_d->dp_control))
        if(yes_msg_save("с результатами выполнения выпускной квалификационной работы"))
            copy_table_to_data(table_dp,&p_d->dp_control);
    p_d->save_data_to_xml();
}
