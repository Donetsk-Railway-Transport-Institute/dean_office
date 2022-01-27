#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "transfer_to_next_year.h"

No_Groups_Transfer::No_Groups_Transfer(
        const QStringList &gr,
        QWidget *parent):QDialog(parent){
    setWindowTitle("Ошибка перевода");
    resize(195, 374);
    label = new QLabel(this);
    label->setGeometry(QRect(10, 10, 171, 16));
    label->setText("Вам сначала необходимо");
    label_2 = new QLabel(this);
    label_2->setGeometry(QRect(10, 30, 171, 16));
    label_2->setText("создать следующие группы:");

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(10, 330, 161, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    QVector<QStringList> vgr;
    for(auto sl:gr) vgr.append(QStringList()<<sl);
    tableWidget = new tt_widget(QStringList()<<"Группы",&vgr,this);
    tableWidget->setGeometry(QRect(10, 70, 171, 231));
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

Transfer_To_Next_Year::Transfer_To_Next_Year(
        startup_data *s_data,const QString &new_year,
        QWidget *parent):R_Dialog(parent), s_d(s_data){
    bool edit_table_only = s_d->xrw.curr_year==new_year;
    if(edit_table_only) setWindowTitle("Редактирование таблицы перевода");
    else setWindowTitle("Перевод студентов на следующий курс");
    resize(342, 542);

    QStringList sl = QStringList()<<"Из группы"<<"с курса"<<"В группу"<<"на курс";
    load_data_from_xml();

    ttw = new tt_widget(sl, &data, this);
    ttw->setGeometry(QRect(10, 140, 321, 331));
    ttw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ttw,&tt_widget::save_table_to_file,this,&Transfer_To_Next_Year::save_table_to_file);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(70, 500, 161, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    label = new QLabel(this);
    label->setGeometry(QRect(10, 10, 81, 16));
    label->setText("Учебный год");
    lineEdit_year = new QLineEdit(this);
    lineEdit_year->setGeometry(QRect(100, 10, 113, 20));
    lineEdit_year->setText(new_year);
    if(edit_table_only) lineEdit_year->setReadOnly(true);
    label_2 = new QLabel(this);
    label_2->setGeometry(QRect(10, 40, 281, 16));
    label_2->setText("Номер приказа о переводе на следующий курс:");
    label_b = new QLabel(this);
    label_b->setGeometry(QRect(130, 60, 51, 16));
    label_b->setText("бюджет");
    label_k = new QLabel(this);
    label_k->setGeometry(QRect(250, 60, 51, 16));
    label_k->setText("контракт");
    label_o = new QLabel(this);
    label_o->setGeometry(QRect(10, 80, 71, 16));
    label_o->setText("Очная форма");
    label_z = new QLabel(this);
    label_z->setGeometry(QRect(10, 110, 81, 16));
    label_z->setText("Заочная форма");

    lineEdit_ob = new QLineEdit(this);
    lineEdit_ob->setGeometry(QRect(100, 80, 101, 20));
    if(edit_table_only) lineEdit_ob->setReadOnly(true);
    lineEdit_ok = new QLineEdit(this);
    lineEdit_ok->setGeometry(QRect(230, 80, 101, 20));
    if(edit_table_only) lineEdit_ok->setReadOnly(true);
    lineEdit_zb = new QLineEdit(this);
    lineEdit_zb->setGeometry(QRect(100, 110, 101, 20));
    if(edit_table_only) lineEdit_zb->setReadOnly(true);
    lineEdit_zk = new QLineEdit(this);
    lineEdit_zk->setGeometry(QRect(230, 110, 101, 20));
    if(edit_table_only) lineEdit_zk->setReadOnly(true);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void Transfer_To_Next_Year::transfer_groups(){
    if(s_d->xrw.curr_year==lineEdit_year->text()){
        QMessageBox::warning(this,
                             "Ошибка при переводе на следующий курс",
                             "Неправильно введен новый учебный год!");
        return;
    }        

    QVector<QStringList> groups;

    QString dir_year_from = QDir::currentPath()+"/dean_data/"+s_d->xrw.curr_year;
    QString dir_year_to = QDir::currentPath()+"/dean_data/"+lineEdit_year->text();
    QDir dir(dir_year_to);
    if(!dir.exists()){
        dir.mkdir(".");
    } else {
        QString curr_year = s_d->xrw.curr_year;
        s_d->xrw.curr_year = lineEdit_year->text();
        s_d->xrw.groups_load_from_xml(s_d->xrw.curr_user, &groups);
        s_d->xrw.curr_year = curr_year;
    }
    dir = QDir(dir_year_to+"/$deleted_students_"+s_d->xrw.curr_user[1]);

    if(!dir.exists()) {
        dir.mkdir(".");
        s_d->xrw.create_students_xml_blank(dir_year_to+"/$deleted_students_"+s_d->xrw.curr_user[1]+"/list_of_students.xml");        
    }
    QFile::copy(dir_year_from+"/transfer_"+s_d->xrw.curr_user[1]+".xml", dir_year_to+"/transfer_"+s_d->xrw.curr_user[1]+".xml");

    for(auto d:data){
        dir = QDir(dir_year_to+"/"+d[2]);
//        qInfo("%s ---- %s ----",lineEdit_year->text().toUtf8().constData(),d[2].toUtf8().constData());
        bool yes_dir = false;
        for(auto gr:groups){
            if(d[2]!=gr[0]) dir.mkdir(".");
            else yes_dir = true;
        }

        QDir mDir(dir_year_from+"/"+d[0]);
        QStringList f_sl;
        if(yes_dir) f_sl = QStringList({"*_pix.png","*_portfolio.pdf"});
        else {
            f_sl = QStringList({"list_of_students.xml","*_pix.png","*_portfolio.pdf"});
            //Добавляем группу
            QStringList tmp_gr = s_d->get_gruops_by_name(d[0]);
            tmp_gr[0] = d[2]; tmp_gr[1] = d[3];
            groups.append(tmp_gr);
        }

        foreach (const QFileInfo &mitm, mDir.entryInfoList(f_sl))
            QFile::copy(mitm.absoluteFilePath(), dir_year_to+"/"+d[2]+"/"+mitm.fileName());

        for(auto s:s_d->students[d[0]]){
            student_data sd(s[0],s_d);
            QString pr_num;
            if(sd.budget_contract=="Республиканского бюджета"){
                if(s_d->get_gruops_by_name(d[0])[3]=="очная"){
                    pr_num = lineEdit_ob->text();
                } else {
                    pr_num = lineEdit_zb->text();
                }
            } else {
                if(s_d->get_gruops_by_name(d[0])[3]=="очная"){
                    pr_num = lineEdit_ok->text();
                } else {
                    pr_num = lineEdit_zk->text();
                }
            }            
            QStringList pa = {d[1],pr_num,QString("Перевод на %1 курс").arg(d[3])};
            if((sd.perevods.isEmpty())||(sd.perevods.last()[2]!=pa[2])) sd.perevods.append(pa);
//            else qInfo("sd.perevods.last()[2]==pa[2] = %s",pa[2].toUtf8().constData());

            save_data_to_xml(dir_year_to+"/"+d[2]+"/"+s[1]+"_data.xml", sd);
            if(yes_dir){
                //добавляем студента в список группы
                QString s_f_name = dir_year_to+"/"+d[2]+"/"+"/list_of_students.xml";
                QVector<QStringList> tmp_stud_gr = s_d->xrw.load_students_group(s_f_name);
                tmp_stud_gr.append(s);
                s_d->xrw.save_students_to_file(&tmp_stud_gr,s_f_name);
            }
        }

    }
    s_d->xrw.curr_year = lineEdit_year->text();

    s_d->xrw.groups_save_to_xml(s_d->xrw.curr_user,&groups);
    s_d->xml_curr_data_save();
}

void Transfer_To_Next_Year::load_data_from_xml(){
    QString xml_file_transfer = QString("dean_data/%1/transfer_%2.xml").arg(s_d->xrw.curr_year).arg(s_d->xrw.curr_user[1]);
    QFile xml_file(xml_file_transfer);
    if (!xml_file.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s",xml_file_transfer.toUtf8().constData());
    auto xml = new QXmlStreamReader(&xml_file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("transfer_year"))){
            while (!(xml->isEndElement() && name == QString("transfer_year"))) {
                xml->readNext();
                name = xml->name().toString();
                if ((xml->isStartElement())&&(name == QString("group"))){
                    data.append(QStringList()<<xml->attributes().value("from_name").toString()
                                <<xml->attributes().value("from_kurs").toString()
                                <<xml->attributes().value("to_name").toString()
                                <<xml->attributes().value("to_kurs").toString());
                }
            }
        }
    }
    xml_file.close();
}

void Transfer_To_Next_Year::save_table_to_file(){
    if(!check_change_data(ttw,&data)) return;
    if(!yes_msg_save()) return;
    copy_table_to_data(ttw,&data);
    QString xml_file_transfer = QString("dean_data/%1/transfer_%2.xml").arg(s_d->xrw.curr_year).arg(s_d->xrw.curr_user[1]);
    QFile file(xml_file_transfer);
    if(!file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",xml_file_transfer.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("transfer_year");   // Записываем первый элемент с его именем
    for(auto d:data){
        xmlWriter.writeStartElement("group");
        xmlWriter.writeAttribute("from_name",d[0]);
        xmlWriter.writeAttribute("from_kurs",d[1]);
        xmlWriter.writeAttribute("to_name",d[2]);
        xmlWriter.writeAttribute("to_kurs",d[3]);
        xmlWriter.writeEndElement();        // Закрываем тег
    }
    xmlWriter.writeEndElement(); // Закрываем тег "transfer_year"
    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

void Transfer_To_Next_Year::save_data_to_xml(
        const QString &xml_file_name,
        const student_data &sd){
    QFile file(xml_file_name);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,
                            "Сохранение данных",
                            QString("Ошибка открытия файла %1").arg(xml_file_name),
                            QMessageBox::Ok);
        return;
    }
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("data_student");   // Записываем первый элемент с его именем

    xmlWriter.writeStartElement("name_engl");
    xmlWriter.writeCharacters(sd.name_engl);
    xmlWriter.writeEndElement(); // Закрываем тег "name_engl"

    xmlWriter.writeStartElement("name_code");
    xmlWriter.writeCharacters(sd.name_code);
    xmlWriter.writeEndElement(); // Закрываем тег "name_code"

    xmlWriter.writeStartElement("data_born");
    xmlWriter.writeCharacters(sd.data_born);
    xmlWriter.writeEndElement(); // Закрываем тег "data_born"

    xmlWriter.writeStartElement("prikaz");
    xmlWriter.writeCharacters(sd.prikaz);
    xmlWriter.writeEndElement(); // Закрываем тег "prikaz"

    xmlWriter.writeStartElement("budget_contract");
    xmlWriter.writeCharacters(sd.budget_contract);
    xmlWriter.writeEndElement(); // Закрываем тег "prikaz"

    for(auto p:sd.perevods){
        xmlWriter.writeStartElement("perevod");
        xmlWriter.writeAttribute("kurs",p[0]);
        xmlWriter.writeAttribute("prikaz",p[1]);
        xmlWriter.writeAttribute("soderg",p[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "perevod"
    }
    for(auto p:sd.prevs_edu){
        xmlWriter.writeStartElement("prev_edu");
        xmlWriter.writeAttribute("period",p[0]);
        xmlWriter.writeAttribute("name_org",p[1]);
        xmlWriter.writeAttribute("level_edu",p[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "prev_edu"
    }
    for(auto r:sd.result_control){
        xmlWriter.writeStartElement("result_control");
        xmlWriter.writeAttribute("disciplina",r[0]);
        xmlWriter.writeAttribute("ze",r[1]);
        xmlWriter.writeAttribute("kurs",r[2]);
        xmlWriter.writeAttribute("semestr",r[3]);
        xmlWriter.writeAttribute("data",r[4]);
        xmlWriter.writeAttribute("form_control",r[5]);
        xmlWriter.writeAttribute("ball",r[6]);
        xmlWriter.writeAttribute("ball_100",r[7]);
        xmlWriter.writeAttribute("ball_7",r[8]);
        xmlWriter.writeEndElement(); // Закрываем тег "result_control"
    }
    for(auto r:sd.kr_control){
        xmlWriter.writeStartElement("kr_control");
        xmlWriter.writeAttribute("disciplina",r[0]);
        xmlWriter.writeAttribute("semestr",r[1]);
        xmlWriter.writeAttribute("tema",r[2]);
        xmlWriter.writeAttribute("ball",r[3]);
        xmlWriter.writeAttribute("ball_100",r[4]);
        xmlWriter.writeAttribute("ball_7",r[5]);
        xmlWriter.writeEndElement(); // Закрываем тег "kr_control"
    }
    for(auto r:sd.pract_control){
        xmlWriter.writeStartElement("pract_control");
        xmlWriter.writeAttribute("vid",r[0]);
        xmlWriter.writeAttribute("basa",r[1]);
        xmlWriter.writeAttribute("work",r[2]);
        xmlWriter.writeAttribute("data",r[3]);
        xmlWriter.writeAttribute("ball",r[4]);
        xmlWriter.writeAttribute("ball_100",r[5]);
        xmlWriter.writeAttribute("ball_7",r[6]);
        xmlWriter.writeEndElement(); // Закрываем тег "pract_control"
    }
    for(auto r:sd.dp_control){
        xmlWriter.writeStartElement("dp_control");
        xmlWriter.writeAttribute("tema",r[0]);
        xmlWriter.writeAttribute("ball",r[1]);
        xmlWriter.writeAttribute("ball_100",r[2]);
        xmlWriter.writeAttribute("ball_7",r[3]);
        xmlWriter.writeEndElement(); // Закрываем тег "dp_control"
    }
    for(auto r:sd.nir_control){
        xmlWriter.writeStartElement("nir_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeEndElement(); // Закрываем тег "nir_control"
    }
    for(auto r:sd.extra_edu_control){
        xmlWriter.writeStartElement("extra_edu_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeEndElement(); // Закрываем тег "extra_edu_control"
    }
    for(auto r:sd.add_edu_control){
        xmlWriter.writeStartElement("add_edu_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeAttribute("value",r[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "add_edu_control"
    }

    xmlWriter.writeStartElement("add_data");
    xmlWriter.writeCharacters(sd.add_data);
    xmlWriter.writeEndElement(); // Закрываем тег "add_data"

    xmlWriter.writeStartElement("kontakts");
    xmlWriter.writeCharacters(sd.kontakts);
    xmlWriter.writeEndElement(); // Закрываем тег "kontakts"

    xmlWriter.writeEndElement(); // Закрываем тег "data_student"

    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

