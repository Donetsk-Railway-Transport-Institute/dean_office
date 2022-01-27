#include <QIcon>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>

#include <QDirIterator>
#include <QFileInfo>

#include <QMessageBox>

#include "deanofficewindow.h"
#include "ui_deanofficewindow.h"
#include "title_data.h"
#include "edit_specialty_dialog.h"
#include "edit_specialization_dialog.h"
#include "edit_groups_dialog.h"
#include "find_dialog.h"
#include "choice_group_dialog.h"
#include "ftp_archiving_dlg.h"
#include "report_editor.h"
#include "print_preview.h"
#include "transfer_to_next_year.h"
#include "order_recognition.h"

void DeanOfficeWindow::run_report(const QString &file_name){
    print_preview pp(s_d,this);
    pp.preview(dir_reports+"/"+file_name+".html");
}

void DeanOfficeWindow::on_edit_reports_triggered(){
    report_editor *re = new report_editor(s_d, dir_reports, this);
    re->show();
}

void DeanOfficeWindow::on_arx_to_ftp_triggered(){
    ftp_archiving_dlg *fad = new ftp_archiving_dlg(s_d,this);
    fad->exec();
}

void DeanOfficeWindow::on_find_triggered(){
    Find_Dialog *f_d = new Find_Dialog(s_d, this);
    if(f_d->exec())
        view_students_group(f_d->get_choice_group_name(),f_d->lineEdit->text());
}

void DeanOfficeWindow::on_edit_groups_triggered(){
    Edit_Groups_Dialog *egd = new Edit_Groups_Dialog(s_d,this);
    egd->exec();
}

void DeanOfficeWindow::on_edit_students_triggered(){
    Choice_Group_Dialog *c_g_d = new Choice_Group_Dialog(s_d->get_gruops_by_dean(s_d->xrw.curr_user[1]));
    if(c_g_d->exec())
        view_students_group(c_g_d->comboBox->currentText());
}

void DeanOfficeWindow::on_edit_specialty_triggered(){
    Edit_Specialty_Dialog *esd = new Edit_Specialty_Dialog(s_d,this);
    esd->exec();
}

void DeanOfficeWindow::on_edit_specialization_triggered(){
    Edit_Specialization_Dialog *esd = new Edit_Specialization_Dialog(s_d);
    esd->exec();
}

void DeanOfficeWindow::on_change_curr_year_triggered(){
    Change_Curr_Year *chcy = new Change_Curr_Year(s_d,this);
    if(chcy->exec()){
        QString curr_year = s_d->xrw.curr_year;
        s_d->xrw.curr_year = chcy->lineEdit->text();
        s_d->xml_curr_data_save();
        QApplication::exit(EXIT_CODE_REBOOT);
    }
}

void DeanOfficeWindow::on_edit_transfer_table_triggered(){
    Transfer_To_Next_Year *ttny = new Transfer_To_Next_Year(s_d, s_d->xrw.curr_year, this);
    if(ttny->exec()){
    }
}

void DeanOfficeWindow::on_transfer_to_next_year_triggered(){
    Order_Recognition *ord = new Order_Recognition(s_d,this);
    if(!ord->exec()) return;
    Transfer_To_Next_Year *ttny = new Transfer_To_Next_Year(s_d, ord->years, this);
    if(ttny->exec()){
/*
        QStringList no_gr;
        for(auto d_gr:ttny->data)
            if (s_d->get_gruops_by_name(d_gr[2]).size()==0) no_gr.append(d_gr[2]);
        if(no_gr.size()>0){
            No_Groups_Transfer *ngt = new No_Groups_Transfer(no_gr,this);
            ngt->exec();
            return;
        }
*/
        ttny->transfer_groups();
        QApplication::exit(EXIT_CODE_REBOOT);
    }
}

void DeanOfficeWindow::on_deleted_students_triggered(){
    if(s_d->deleted_students_from_dir.isEmpty())
        QMessageBox::information(this, QString("Удаленные студенты"), QString("Отсутствуют данные об удаленных студентах"));
//    view_students_group(QString("$deleted_students_%1").arg(s_d->xrw.curr_user[1]));
}

void DeanOfficeWindow::on_change_password_triggered(){
    Change_Password_Dialog *c_p_d = new Change_Password_Dialog(s_d->xrw.curr_user);
    if(c_p_d->exec()){
        QString blah = QString(QCryptographicHash::hash(
                                   c_p_d->old_password->text().toUtf8(),
                                   QCryptographicHash::Md5).toHex());
        if(blah==s_d->xrw.curr_user[2])
            if(c_p_d->new_password_first->text()==c_p_d->new_password_second->text())
                s_d->change_password_at_xml(QString(QCryptographicHash::hash(
                                                   c_p_d->new_password_first->text().toUtf8(),
                                                   QCryptographicHash::Md5).toHex()));
            else QMessageBox::warning(this, QString("Ошибка"), QString("Несовпадение новых паролей"));
        else QMessageBox::warning(this, QString("Ошибка"), QString("Ошибка ввода старого пароля"));
    }
}

DeanOfficeWindow::DeanOfficeWindow(QWidget *parent) :
    QMainWindow(parent),tabWidget(nullptr), s_d(new startup_data(this)),
    dir_reports(QDir::currentPath()+"/reports"),ui(new Ui::DeanOfficeWindow),curr_tab(nullptr){
    ui->setupUi(this);
    if(!QDir(dir_reports).exists())
        qFatal("Directory \"%s\" not exist.",dir_reports.toUtf8().constData());
    QDirIterator it(dir_reports,QStringList() << "*.html",QDir::Files,QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString html_name = it.next().remove(dir_reports+"/").split(".html")[0];
        QAction *action = ui->menu_reports->addAction(html_name);
        connect(action, &QAction::triggered, [this, action](){run_report(action->text());});
    }

    connect(ui->about, &QAction::triggered, this, &DeanOfficeWindow::about_box);
    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,&DeanOfficeWindow::itemClicked);
    connect(ui->treeWidget,&QTreeWidget::itemDoubleClicked,this,&DeanOfficeWindow::itemDoubleClicked);

    if(s_d->xrw.curr_user[0].isEmpty()) qFatal("Cancel or bad password");
    setWindowTitle(QString("%1 %2 учебный год").arg(title_name).arg(s_d->xrw.curr_year));
    QPixmap pix = QPixmap(title_pix);
    setWindowIcon(QIcon(QPixmap(title_pix)));

    QStringList d_s;
    for(auto d:s_d->deans)
        if(d[0]==s_d->xrw.curr_user[1]) d_s.append(d[1]);
    ui->treeWidget->setHeaderLabels(d_s);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view_tree_widget();
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    connect(tabWidget,&QTabWidget::tabCloseRequested,this,&DeanOfficeWindow::tabCloseRequested);
    connect(tabWidget,&QTabWidget::currentChanged,this,&DeanOfficeWindow::currentChanged);
    ui->layout->addWidget(tabWidget);
}

DeanOfficeWindow::~DeanOfficeWindow(){
    for(int i=0;i<tabWidget->count();++i){
        students_table_widget* tw = static_cast<students_table_widget*>(tabWidget->widget(i));
        tw->check_change_data();
        tabWidget->removeTab(i);
    }
    delete tabWidget;
    delete ui;
}

void DeanOfficeWindow::view_tree_widget(){
    ui->treeWidget->clear();
    QList<QTreeWidgetItem *> items;
    for (auto sp:s_d->specialization){      //по специализациям
        if(sp[3]==s_d->xrw.curr_user[1]){   //для данного деканата
            QString sp_form = s_d->prog[sp[2].split(".")[1]];
            QString sp_name = QString("%1 (%2)").arg(sp[0]).arg(sp_form);
            QTreeWidgetItem *curr_sp = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList()<<sp_name);
            for(auto f:s_d->forms){
                //специализации очной и заочной формы
                QTreeWidgetItem *it = new QTreeWidgetItem(curr_sp, QStringList()<<f);
                QList<QTreeWidgetItem *> groups;
                for(int i=1;i<9;++i){
                    QStringList gr = s_d->get_gruops_by_form(i, sp[3], sp[1], sp[2], f);
                    for(auto g:gr)
                        groups.append(new QTreeWidgetItem(it, QStringList()<<g));
                }
                if(groups.isEmpty()) delete it;
            }
            items.append(curr_sp);
        }
    }
    ui->treeWidget->insertTopLevelItems(0, items);
}

void DeanOfficeWindow::itemClicked(QTreeWidgetItem *item, int column){
    QStringList items;
    items.push_front(item->text(column));
    QTreeWidgetItem *curr_item = item;
    while(curr_item->parent()){
        curr_item = curr_item->parent();
        items.push_front(curr_item->text(0));
    }
    ui->statusBar->showMessage(items.join(" "));
}

void DeanOfficeWindow::itemDoubleClicked(QTreeWidgetItem *item, int column){
    view_students_group(item->text(column));
}

void DeanOfficeWindow::view_students_group(const QString &group_name, const QString &selected){
    QStringList gr = s_d->get_gruops_by_name(group_name);
    if(gr.isEmpty()) return;
    students_table_widget *tw = nullptr;
    for(int i=0;i<tabWidget->count();++i){
        if(gr[0] == tabWidget->tabText(i)){
            tabWidget->setCurrentIndex(i);
            tw = static_cast<students_table_widget*>(tabWidget->widget(i));
        }
    }

    if(tw==nullptr)
        tw = new students_table_widget(gr[0], s_d, tabWidget);

    tabWidget->setCurrentWidget(tw);
    if(!selected.isEmpty()){
        for(int i=0;i<tw->rowCount();++i){
            if(tw->item(i, 0)->text()==selected){
                tw->item(i, 0)->setSelected(true);
                tw->scrollToItem(tw->item(i, 0));
            } else tw->item(i, 0)->setSelected(false);
        }
    }
}

void DeanOfficeWindow::tabCloseRequested(int index){
    students_table_widget *tw = static_cast<students_table_widget*>(tabWidget->widget(index));
    tw->check_change_data();
    tabWidget->removeTab(index);
    if(tabWidget->count()==0) curr_tab = nullptr;
}

void DeanOfficeWindow::currentChanged(int index){
    if(index!=-1){
        if(curr_tab!=nullptr)
            curr_tab->check_change_data();
        curr_tab = static_cast<students_table_widget*>(tabWidget->widget(index));
    };
}
