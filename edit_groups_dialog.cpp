#include "edit_groups_dialog.h"

void group_table_widget::remove_row(){
    QString gr_name = item(currentItem()->row(),0)->text();
    if(s_d->students[gr_name].size()!=0){
        QMessageBox::warning( this,
                              QString("Удаление группы"),
                              QString("Вы не можете удалить группу %1, "
                                      "т.к. в ней числятся %2 студентов.")
                              .arg(gr_name).arg(s_d->students[gr_name].size()),
                              QMessageBox::Ok);
    } else tt_widget::remove_row();
}

group_table_widget::group_table_widget(
        startup_data *s_data,
        QWidget *parent) : tt_widget(QStringList()<<"Наименование"<<"Курс"<<"Специальность"<<
                                                     "Форма обуч."<<"Код"<<"Факультет",
                                     &s_data->groups,parent), s_d(s_data){
    connect(this,&QTableWidget::itemDoubleClicked,
            this,&group_table_widget::itemDoubleClicked);
    connect(this,&QTableWidget::itemSelectionChanged,
            this,&group_table_widget::itemSelectionChanged);
}

void group_table_widget::itemSelectionChanged(){
    for(auto c:v_cb){
        QString curr_text = c.cb->currentText();
        removeCellWidget(c.row,c.column);
        delete c.cb;
        c.cb = nullptr;
        item(c.row,c.column)->setText(curr_text);
    }
    v_cb.clear();
}

void group_table_widget::itemDoubleClicked(QTableWidgetItem *item){
    //    <group name="1-Мз" kurs="1" spec="М" form="заочн" parent="38.03.02" dean="ИЭЖТ"/>
    QComboBox *cb = nullptr;
    if(item->column()==2){
        cb = new QComboBox();
        for(auto sp:s_d->specialization) cb->addItem(sp[1]);
    } else if(item->column()==3){
        cb = new QComboBox();
        for(auto f:s_d->forms) cb->addItem(f);
    } else if(item->column()==4){
            cb = new QComboBox();
            for(auto sp:s_d->specialization) cb->addItem(sp[2]);
    } else if(item->column()==5){
        cb = new QComboBox();
        for(auto d:s_d->deans) cb->addItem(d[0]);
    } else return;
    cb->setCurrentText(item->text());
    setCellWidget(item->row(),item->column(),cb);
    ROW_COLUMN_COMBOBOX rcc(item->row(), item->column(), cb);
    v_cb.append(rcc);
}

Edit_Groups_Dialog::Edit_Groups_Dialog(
        startup_data *s_data,
        QWidget *parent):R_Dialog(parent),
    table(new group_table_widget(s_data,this)),s_d(s_data){
    setWindowTitle("Редактирование групп студентов");
    resize(621, 300);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(250, 260, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    table->setGeometry(QRect(0, 0, 621, 251));

    connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void Edit_Groups_Dialog::save_table_to_file(){
    if(!check_change_data(table,&s_d->groups)) return;
    if(!yes_msg_save()) return;
    copy_table_to_data(table,&s_d->groups);
    s_d->xrw.groups_save_to_xml(s_d->xrw.curr_user,&s_d->groups);
}

