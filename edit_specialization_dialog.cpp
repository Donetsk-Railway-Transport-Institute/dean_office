#include "edit_specialization_dialog.h"

// специализации
specialization_table_widget::specialization_table_widget(
        startup_data *s_data,
        QWidget *parent) : tt_widget(QStringList()<<"Наименование"<<"Сокр."<<"Код"<<"Факультет",
                                     &s_data->specialization, parent), s_d(s_data){
    connect(this,&QTableWidget::itemDoubleClicked,
            this,&specialization_table_widget::itemDoubleClicked);
    connect(this,&QTableWidget::itemSelectionChanged,
            this,&specialization_table_widget::itemSelectionChanged);
}

void specialization_table_widget::itemSelectionChanged(){
    if(edit_item.num_row!=-1){
        QString curr_text = edit_item.comboBox_specialty->currentText();
        removeCellWidget(edit_item.num_row,2);
        delete edit_item.comboBox_specialty;
        item(edit_item.num_row,2)->setText(curr_text);
        edit_item.num_row=-1;
    }
    if(edit_item.dean_row!=-1){
        QString curr_text = edit_item.comboBox_dean->currentText();
        removeCellWidget(edit_item.dean_row,3);
        delete edit_item.comboBox_dean;
        item(edit_item.dean_row,3)->setText(curr_text);
        edit_item.dean_row=-1;
    }
}

void specialization_table_widget::itemDoubleClicked(QTableWidgetItem *item){
    if(item->column()==2){ //код специальности
        edit_item.comboBox_specialty = new QComboBox();
        for(auto s:s_d->specialty)
            edit_item.comboBox_specialty->addItem(s[0]);
        edit_item.num_row = item->row();
        edit_item.comboBox_specialty->setCurrentText(item->text());
        setCellWidget(item->row(),2,edit_item.comboBox_specialty);
    }
    if(item->column()==3){ //деканат
        edit_item.comboBox_dean = new QComboBox();
        for(auto d:s_d->deans)
            edit_item.comboBox_dean->addItem(d[0]);
        edit_item.dean_row = item->row();
        edit_item.comboBox_dean->setCurrentText(item->text());
        setCellWidget(item->row(),3,edit_item.comboBox_dean);
    }
}

Edit_Specialization_Dialog::Edit_Specialization_Dialog(
        startup_data *s_data,
        QWidget *parent):R_Dialog(parent),table(nullptr),s_d(s_data){
    setWindowTitle("Редактирование специализаций");
    resize(622, 300);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(240, 260, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    table = new specialization_table_widget(s_d, this);
    table->setGeometry(QRect(0, 0, 621, 251));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    connect(table,&tt_widget::save_table_to_file,this,&Edit_Specialization_Dialog::save_table_to_file);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void Edit_Specialization_Dialog::save_table_to_file(){
    if(!check_change_data(table,&s_d->specialization)) return;
    if(!yes_msg_save()) return;
    copy_table_to_data(table,&s_d->specialization);
    s_d->xrw.xml_specialties_save(&s_d->specialty,&s_d->specialization);
}
