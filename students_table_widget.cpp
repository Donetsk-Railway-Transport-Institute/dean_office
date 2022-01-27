#include "students_table_widget.h"
#include "portfolio_dialog.h"
#include <QDir>

students_table_widget::students_table_widget(
        const QString &gr_name,
        startup_data *s_data,
        QWidget *parent) : tt_widget(parent), s_d(s_data), must_saved(false) {
    setObjectName(gr_name);
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList()<<""<<""<<"");
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QTabWidget *tabWidget = static_cast<QTabWidget *>(parent);
    tabWidget->addTab(this, gr_name);

    for(int i=0;i<s_d->students[gr_name].size();++i)
        insert_row(i,s_d->students[gr_name][i]);

    setCurrentItem(item(0, 0));
    setFocus();
}

bool students_table_widget::complete_files(const QString &student_name){
    QString id = s_d->xrw.get_student_id_by_name(student_name);
    if(id.isEmpty()) return false;
    QDir gr_dir(QString("dean_data/%1/%2").arg(s_d->xrw.curr_year).arg(objectName()));
    QStringList files = gr_dir.entryList(QStringList() << "*.*", QDir::Files);
    //    for(auto f:files) qInfo("%s",f.toUtf8().constData());
    if(!files.contains(id+"_portfolio.pdf")) return false;
    if(!files.contains(id+"_pix.png")) return false;
    if(!files.contains(id+"_data.xml")) return false;
    return true;
}

void students_table_widget::clicked(const QString &text){
//    qInfo("students_table_widget::clicked(%s)",text.toUtf8().constData());
    QPushButton *button = findChild<QPushButton*>(text);
    if(!button) return;
    if(!text.split(":")[1].isEmpty()){
        Portfolio_Dialog *pd = new Portfolio_Dialog(text.split(":")[1],s_d);
        pd->exec();
        change_botton_color(button,complete_files(text.split(":")[1]));
    }
}

void students_table_widget::check_change_data(){
//    qInfo("s_d->students[objectName()].size()=%d",s_d->students[objectName()].size());
    //количество студентов группы не изменилось
    if(rowCount()==s_d->students[objectName()].size()){
        for(int r=0;r<rowCount();++r){
            QPushButton *button = static_cast<QPushButton *>(cellWidget(r, 1));
            if(item(r,0)->text()!=s_d->students[objectName()][r][0]){
                s_d->students[objectName()][r][0] = item(r,0)->text();
                if(p_buttons[button]) s_d->students[objectName()][r][2] = "true";
                else s_d->students[objectName()][r][2] = "false";
                must_saved = true;
            }
        }
    }
    //добавление студента
    //была вставка строки
    if(rowCount()>s_d->students[objectName()].size()){
        QMap<QString,QStringList*> tmp_st_name;
        for(int i=0;i<s_d->students[objectName()].size();++i)
            tmp_st_name.insert(s_d->students[objectName()][i][0], &s_d->students[objectName()][i]);
        for(int r=0;r<rowCount();++r)
            if(!tmp_st_name.contains(item(r,0)->text())){
                QStringList sl = QStringList()<<item(r,0)->text()<<""<<objectName()<<"false";
                s_d->students[objectName()].append(sl);
            }
    }
    //было удаление строки
    if(s_d->deleted_students[objectName()].size()>0){
        s_d->xrw.xml_students_save( objectName(),
                                    &s_d->deleted_students[objectName()],
                                    &s_d->students[objectName()] );
    }
    if(must_saved) save_students_to_file();
}

void students_table_widget::insert_row(int iserted_row, const QStringList &student){
    insertRow(iserted_row);
    QTableWidgetItem *item = new QTableWidgetItem(student[0]);
    setItem(iserted_row,0, item);
    QPushButton *button = new QPushButton("Редакт.",this);
    p_buttons.insert(button,false);

    button->setObjectName(QString("%1:%2").arg(objectName()).arg(student[0]));
    setCellWidget(iserted_row, 1, button);
    if(!student[0].isEmpty())
        connect(button, &QPushButton::clicked, [this, button](){ clicked(button->objectName()); });
    change_botton_color(button, complete_files(student[0]));

//    qInfo("students_table_widget::insert_row() button->objectName()=%s",button->objectName().toUtf8().constData());
}

void students_table_widget::insert_after_row(){
//    qInfo("students_table_widget::insert_after_row() rowCount()=%d",rowCount());
    if(rowCount()==0) insert_row(0,QStringList()<<""<<""<<"");
    else insert_row(currentItem()->row()+1,QStringList()<<""<<""<<"");
    if(rowCount()==1) setCurrentItem(item(1, 0));
    else setCurrentItem(item(currentItem()->row()+1, 0));

    //ФИО студента пока нет. Коннектим с изменением этой ячейки
    connect(this,&students_table_widget::itemChanged,this,&students_table_widget::item_change_data);
    must_saved = true;    
}

void students_table_widget::item_change_data(QTableWidgetItem *item){
    //Был закончен ввод ФИО студента
    QPushButton *button = static_cast<QPushButton *>(item->tableWidget()->cellWidget(item->row(), 1));
    button->setObjectName(QString("%1:%2").arg(objectName()).arg(item->text()));
    connect(button, &QPushButton::clicked, [this, button](){ clicked(button->objectName()); });
//    disconnect(this,&students_table_widget::itemChanged,this,&students_table_widget::item_change_data);
    check_change_data();
//    qInfo("students_table_widget::item_change_data button->objectName()=%s",button->objectName().toUtf8().constData());
}

void students_table_widget::insert_before_row(){
//    qInfo("students_table_widget::insert_before_row()");
    if(rowCount()==0) return;
    else insert_row(currentItem()->row(),QStringList()<<""<<""<<"");
    setCurrentItem(item(currentItem()->row(), 0));
    connect(this,&students_table_widget::itemChanged,this,&students_table_widget::item_change_data);
    must_saved = true;
}

void students_table_widget::remove_row(){
    if(rowCount()==0) return;
    for(auto st:s_d->students[objectName()]){
        if(st[0]==item(currentItem()->row(),0)->text()){
            if(QMessageBox::question( this,QString("Отчисление студента"),
                                     QString("Необходимо указать причину отчисления %1 "
                                             "и номер приказа. Продолжить?").arg(st[0]),
                                     QMessageBox::Yes | QMessageBox::Cancel)==QMessageBox::Cancel) return;
            Portfolio_Dialog *pd = new Portfolio_Dialog(st[0],s_d);
            if(!pd->exec())
                if(QMessageBox::question( this,
                                          QString("Отмена удаления"),
                                          QString("Отменить удаление студента %1?").arg(st[0]),
                                          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) return;

//            qInfo("students_table_widget::remove_row() "
//                  "s_d->deleted_students[%s].append(%s)",
//                  objectName().toUtf8().constData(),st[0].toUtf8().constData());
            s_d->deleted_students[objectName()].append(st);
        }
    }
    removeRow(currentItem()->row());
    must_saved = true;
}

void students_table_widget::change_botton_color(QPushButton *button, bool red_color){
    QPalette pal = button->palette();
    if(!red_color) {
        pal.setColor(QPalette::Button, QColor(Qt::red));
        p_buttons[button]=false;
    } else {
        pal.setColor(QPalette::Button, QColor(Qt::white));
        p_buttons[button]=true;
    }
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->update();
}

void students_table_widget::save_students_to_file(){
    QString file_name = QString("dean_data/%1/%2/list_of_students.xml")
            .arg(s_d->xrw.curr_year).arg(objectName());
    if(message_about_save(file_name)){
        s_d->xrw.save_students_to_file(&s_d->students[objectName()],file_name);
        must_saved = false;
    }
}

