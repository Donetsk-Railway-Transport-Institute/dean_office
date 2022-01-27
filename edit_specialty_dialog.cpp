#include "edit_specialty_dialog.h"

Edit_Specialty_Dialog::Edit_Specialty_Dialog(
        startup_data *s_data,
        QWidget *parent):
    R_Dialog(parent),table(nullptr),s_d(s_data){
    setWindowTitle("Редактирование специальностей");
    resize(463, 300);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(90, 260, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    table = new tt_widget(QStringList()<<"Код"<<"Специальность", &s_d->specialty, this);
    table->setGeometry(QRect(0, 0, 461, 251));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(table,&tt_widget::save_table_to_file,this,&Edit_Specialty_Dialog::save_table_to_file);

    connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void Edit_Specialty_Dialog::save_table_to_file(){
    if(!check_change_data(table,&s_d->specialty)) return;
    if(!yes_msg_save()) return;
    copy_table_to_data(table,&s_d->specialty);
    s_d->xrw.xml_specialties_save(&s_d->specialty,&s_d->specialization);
}

