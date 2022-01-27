#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QCryptographicHash>
#include <QMessageBox>

#include "xml_read_write.h"

QString xml_read_write::randx(int size) {
    QByteArray result;
    static bool rndt;
    if (!rndt) { qsrand(static_cast<uint>(time(nullptr))); rndt = true; }

    char key = static_cast<char>(qrand());

    for (int i = 0; i < size / static_cast<int>(sizeof(key)); i++){
        key += ((key << 2) | (key << (sizeof(key) * 8 - 2))) ^ qrand();
        result.append(&key, sizeof(key));
    }
    return QString(QCryptographicHash::hash(result,QCryptographicHash::Md5).toHex());
}

void xml_read_write::create_students_xml_blank(const QString &file_name){
    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("Error create file %s",file_name.toUtf8().constData());
    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    out << "<!DOCTYPE list_of_students>\n";
    out << "<list_of_students>\n";
    out << "</list_of_students>\n";
    file.close();
}

QMap<QString,QVector<QStringList>> xml_read_write::load_all_students_from_dirs(
        const QString &year_dir, QVector<QStringList> *groups){
    QMap<QString,QVector<QStringList>> gr_students;
    for(auto gr:*groups){
        QString file_name = QString("%1/%2/list_of_students.xml").arg(year_dir).arg(gr[0]);
        gr_students.insert(gr[0],load_students_group(file_name));
    }
    return gr_students;
}

QVector<QStringList> xml_read_write::load_students_group(const QString &file_name){
    QVector<QStringList> res_students;
    QFile xml_file(file_name);
    if (!xml_file.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s",file_name.toUtf8().constData());
    auto xml = new QXmlStreamReader(&xml_file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("list_of_students"))) {
            while (!(xml->isEndElement() && name == QString("list_of_students"))){
                xml->readNext();
                name = xml->name().toString();
                if ((xml->isStartElement()) && (name == QString("student"))){
                    QString s_name = xml->attributes().value("name").toString();
                    QString id = xml->attributes().value("id").toString();
                    if(id.isEmpty())
                        qFatal("Некорректное ID студента %s",s_name.toUtf8().constData());
                    QStringList s = QStringList()<<s_name<<id<<"false";
                    res_students.append(s);
                }
            }
        }
    }
    xml_file.close();
    return res_students;
}

void xml_read_write::xml_students_save(
        const QString &gr_name,
        QVector<QStringList> *deleted_students,
        QVector<QStringList> *students){    
//    qInfo("deleted_students.size()=%d",deleted_students->size());
    if(deleted_students->size()>0){
        QString deleted_dir = QString("dean_data/%1/$deleted_students_%2").arg(curr_year).arg(curr_user[1]);
        for(auto st:*deleted_students){
            //Переносим все файлы студента в директорию $deleted_students
            QDir from_dir(QString("dean_data/%1/%2").arg(curr_year).arg(gr_name));
            QStringList files = from_dir.entryList(QStringList() << QString("%1_*.*").arg(st[1]),QDir::Files);
            foreach(QString filename, files) {
                QString from_name = QString("dean_data/%1/%2/%3")
                        .arg(curr_year).arg(gr_name).arg(filename);
                QString to_name = QString("%1/%2").arg(deleted_dir).arg(filename);
//                qInfo("copy from=%s, to=%s", from_name.toUtf8().constData(), to_name.toUtf8().constData());
                if(!QFile::copy(from_name, to_name))
                    qWarning("Ошибка копирования файла из %s в %s",
                             from_name.toUtf8().constData(),
                             to_name.toUtf8().constData());
                if(!QFile::remove(from_name))
                    qWarning("Ошибка удаления файла %s",
                             from_name.toUtf8().constData());
            }
            QString del_key; int del_index = -1;
            for(int i=0;i<students->size();++i)
                if(students->at(i)[0]==st[0]){
                    del_key = students->at(i)[1];
                    del_index = i;
                }
            if(del_index!=-1){
                students->remove(del_index);                
                students_with_key.remove(del_key);
            }
            save_students_to_file(students,
                                  QString("dean_data/%1/%2/list_of_students.xml")
                                  .arg(curr_year).arg(gr_name));
        }
    }
    QVector<QStringList> del_st = *deleted_students+load_students_group(
                QString("dean_data/%1/$deleted_students_%2/"
                        "list_of_students.xml").arg(curr_year).arg(curr_user[1]));
    save_students_to_file(&del_st,
                QString("dean_data/%1/$deleted_students_%2/"
                        "list_of_students.xml").arg(curr_year).arg(curr_user[1]));
    deleted_students->clear();
}

void xml_read_write::save_students_to_file(QVector<QStringList> *students,
                                           const QString &file_name){
    QFile xml_file(file_name);
    if(!xml_file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи id",file_name.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&xml_file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("list_of_students");   // Записываем первый элемент с его именем
    for(int i=0;i<students->size();++i){
        xmlWriter.writeStartElement("student");
        xmlWriter.writeAttribute("name", students->at(i)[0]);
        if(students->at(i)[1].isEmpty()){
            QString tmp_id = randx();
            while(students_with_key.contains(tmp_id)) tmp_id = randx();
            xmlWriter.writeAttribute("id",tmp_id);
            students->replace(i,QStringList()<<students->at(i)[0]<<tmp_id);
//            qInfo("%s %s",students->at(i)[0].toUtf8().constData(),tmp_id.toUtf8().constData());
        }
        else xmlWriter.writeAttribute("id",students->at(i)[1]);
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    xml_file.close();
}

void xml_read_write::save_students_to_xml_group(const QString &group,QVector<QStringList> *students){
    QString gr_dir(QString("dean_data/%1/%2").arg(curr_year).arg(group));
    QString file_name = QString("%1/list_of_students.xml").arg(gr_dir);
    QDir c_dir(gr_dir);
    if(c_dir.exists()){
        save_students_to_file(students,file_name);
    } else {
        qFatal("Ошибка записи файла list_of_students.xml,"
               " т.к. отсутствует директория %s",gr_dir.toUtf8().constData());
    }
}

void xml_read_write::xml_specialties_save(
        QVector<QStringList> *specialty,
        QVector<QStringList> *specialization){
    QFile file(xml_file_specialties);
    if(!file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",xml_file_specialties.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("specialties");   // Записываем первый элемент с его именем

    for(int i=0;i<specialty->size();++i){
        // <specialty num="38.03.01" name="Экономика"/>
        xmlWriter.writeStartElement("specialty");
        xmlWriter.writeAttribute("num",specialty->at(i)[0]);
        xmlWriter.writeAttribute("name",specialty->at(i)[1]);
        xmlWriter.writeEndElement();        // Закрываем тег
        for(int s=0;s<specialization->size();++s){
            if(specialty->at(i)[0]==specialization->at(s)[2]){
                // <specialization name="Учет и аудит" cut_name="УА" parent="38.03.01" dean="ИЭЖТ"/>
                xmlWriter.writeStartElement("specialization");
                xmlWriter.writeAttribute("name",specialization->at(s)[0]);
                xmlWriter.writeAttribute("cut_name",specialization->at(s)[1]);
                xmlWriter.writeAttribute("parent",specialization->at(s)[2]);
                xmlWriter.writeAttribute("dean",specialization->at(s)[3]);
                xmlWriter.writeEndElement();        // Закрываем тег
            }
        }
    }

    xmlWriter.writeEndElement(); // Закрываем тег "specialties"
    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

void xml_read_write::xml_specialties_read(
        QVector<QStringList> *specialty,
        QVector<QStringList> *specialization){
    QFile xml_file(xml_file_specialties);
    if (!xml_file.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s для чтения",
               xml_file_specialties.toUtf8().constData());
    auto xml = new QXmlStreamReader(&xml_file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("specialties"))) {
            while (!(xml->isEndElement() && name == QString("specialties"))) {
                xml->readNext();
                name = xml->name().toString();
                if (xml->isStartElement()){
                    if(name == QString("specialty"))
                        specialty->append(QStringList()<<xml->attributes().value("num").toString()
                                         <<xml->attributes().value("name").toString());
                    if(name == QString("specialization"))
                        specialization->append(QStringList()<<xml->attributes().value("name").toString()
                                              <<xml->attributes().value("cut_name").toString()
                                              <<xml->attributes().value("parent").toString()
                                              <<xml->attributes().value("dean").toString());
                }
            }
        }
    }
    xml_file.close();
}

void xml_read_write::change_password_at_xml(const QString &new_pass,
                                            const QStringList &curr_user,
                                            QVector<QStringList> *users){
    QFile file(xml_file_users);
    if(!file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",xml_file_users.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("users");   // Записываем первый элемент с его именем
    for(int i=0;i<users->size();++i){
        xmlWriter.writeStartElement("user");
        xmlWriter.writeAttribute("name",users->at(i)[0]);
        xmlWriter.writeAttribute("dean",users->at(i)[1]);
        if(users->at(i)[0]==curr_user[0]) xmlWriter.writeAttribute("password",new_pass);
        else xmlWriter.writeAttribute("password",users->at(i)[2]);
        xmlWriter.writeEndElement();        // Закрываем тег
    }
    xmlWriter.writeEndElement(); // Закрываем тег "users"
    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

void xml_read_write::xml_data_save(QMap<QString,QString> par,
                                   QMap<QString,QString> *prog,
                                   QVector<QStringList> *deans){
    QFile file(xml_file_name);
    if(!file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",xml_file_name.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("dean_office");   // Записываем первый элемент с его именем

    xmlWriter.writeTextElement("institut_name",par["institut_name"]);
    xmlWriter.writeTextElement("institut_cut_name",par["institut_cut_name"]);
    xmlWriter.writeTextElement("institut_litle_name",par["institut_litle_name"]);
    xmlWriter.writeTextElement("curr_year",par["curr_year"]);
    int i = 0;
    while(par.contains(QString("form_%1").arg(i)))
        xmlWriter.writeTextElement("form",par[QString("form_%1").arg(i++)]);
    i = 0;
    while(par.contains(QString("budget_contract_%1").arg(i)))
        xmlWriter.writeTextElement("budget_contract",par[QString("budget_contract_%1").arg(i++)]);

    QMapIterator<QString, QString> it(*prog);
    while (it.hasNext()) {
        it.next();
        xmlWriter.writeStartElement("prog");
        xmlWriter.writeAttribute("level",it.key());
        xmlWriter.writeAttribute("name",it.value());
        xmlWriter.writeEndElement(); // Закрываем тег "prog"
    }

    for(auto d:*deans){
        xmlWriter.writeStartElement("dean");
        xmlWriter.writeAttribute("name",d[1]);
        xmlWriter.writeAttribute("cut_name",d[0]);
        xmlWriter.writeEndElement(); // Закрываем тег "dean"
    }

    xmlWriter.writeStartElement("specialties");
    xmlWriter.writeAttribute("file_name",xml_file_specialties);
    xmlWriter.writeEndElement(); // Закрываем тег "specialties"

    xmlWriter.writeStartElement("users");
    xmlWriter.writeAttribute("file_name",xml_file_users);
    xmlWriter.writeEndElement(); // Закрываем тег "users"

    xmlWriter.writeStartElement("adobe");
    xmlWriter.writeAttribute("file_name",xml_adobe_key);
    xmlWriter.writeEndElement(); // Закрываем тег "adobe"

    xmlWriter.writeStartElement("ftp_access");
    xmlWriter.writeAttribute("user",xml_ftp_data[0]);
    xmlWriter.writeAttribute("password",xml_ftp_data[1]);
    xmlWriter.writeAttribute("url",xml_ftp_data[2]);
    xmlWriter.writeEndElement(); // Закрываем тег "ftp_access"

    xmlWriter.writeEndElement(); // Закрываем тег "dean_office"
    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

QMap<QString,QString> xml_read_write::xml_data_read(QVector<QString> *forms,
                                                    QVector<QString> *budget_contract,
                                                    QMap<QString,QString> *prog,
                                                    QVector<QStringList> *deans){

    QFile xml_file(xml_file_name);
    if (!xml_file.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s для чтения",xml_file_name.toUtf8().constData());
    QMap<QString,QString> m_d;
    auto xml = new QXmlStreamReader(&xml_file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("dean_office"))) {
            while (!(xml->isEndElement() && name == QString("dean_office"))) {
                xml->readNext();
                name = xml->name().toString();
                if (xml->isStartElement()){
                    if(name == QString("institut_name"))
                        m_d.insert("institut_name",xml->readElementText());
                    if(name == QString("institut_cut_name"))
                        m_d.insert("institut_cut_name",xml->readElementText());
                    if(name == QString("institut_litle_name"))
                        m_d.insert("institut_litle_name",xml->readElementText());
                    if(name == QString("curr_year"))
                        m_d.insert("curr_year",xml->readElementText());
                    if(name == QString("form"))
                        forms->append(xml->readElementText());
                    if(name == QString("budget_contract"))
                        budget_contract->append(xml->readElementText());
                    if(name == QString("dean"))
                        deans->append(QStringList()
                                      <<xml->attributes().value("cut_name").toString()
                                      <<xml->attributes().value("name").toString());
                    if(name == QString("prog")) {
                        QString level = xml->attributes().value("level").toString();
                        QString prog_name = xml->attributes().value("name").toString();
                        prog->insert(level,prog_name);
                    }
                    if(name == QString("specialties"))
                        xml_file_specialties = xml->attributes().value("file_name").toString();
                    if(name == QString("users"))
                        xml_file_users = xml->attributes().value("file_name").toString();
                    if(name == QString("adobe"))
                        xml_adobe_key = xml->attributes().value("key").toString();
                    if(name == QString("ftp_access")){
                        //[0]-user; [1]-password; [2]-url
                        xml_ftp_data.append(xml->attributes().value("user").toString());
                        xml_ftp_data.append(xml->attributes().value("password").toString());
                        xml_ftp_data.append(xml->attributes().value("url").toString());
                    }
                }
            }
        }
    }
    xml_file.close();
    return m_d;
}

QVector <QStringList> xml_read_write::xml_users_read(){
    QFile xml_users(xml_file_users);
    if (!xml_users.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s для чтения",xml_file_users.toUtf8().constData());
    QVector <QStringList> users;
    auto xml_u = new QXmlStreamReader(&xml_users);
    while (!xml_u->atEnd() && !xml_u->hasError()) {
        xml_u->readNext();
        QString name = xml_u->name().toString();
        if ((xml_u->isStartElement()) && (name == QString("users"))) {
            while (!(xml_u->isEndElement() && name == QString("users"))) {
                xml_u->readNext();
                name = xml_u->name().toString();
                if ((xml_u->isStartElement())&&(name == QString("user")))
                    users.append( QStringList() << xml_u->attributes().value("name").toString()
                                    << xml_u->attributes().value("dean").toString()
                                    << xml_u->attributes().value("password").toString());
            }
        }
    }
    xml_users.close();
    return users;
}

void xml_read_write::groups_load_from_xml(const QStringList &c_user, QVector<QStringList> *groups){
    QString xml_file_groups = QString("dean_data/%1/groups_%2.xml").arg(curr_year).arg(c_user[1]);
    if(!QFileInfo::exists(xml_file_groups)){
        QString msg = QString("Отсутствует база данных деканата %1").arg(c_user[1]);
        QMessageBox::critical(nullptr, QString("Ошибка БД"), msg);
        qFatal("%s",msg.toUtf8().constData());
    }
    QFile xml_file(xml_file_groups);
    if (!xml_file.open(QIODevice::ReadOnly))
        qFatal("Ошибка открытия файла %s",xml_file_groups.toUtf8().constData());
    auto xml = new QXmlStreamReader(&xml_file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("groups"))) {
            while (!(xml->isEndElement() && name == QString("groups"))) {
                xml->readNext();
                name = xml->name().toString();
                if ((xml->isStartElement()) && (name == QString("group"))){
                    QStringList gr = QStringList()
                            <<xml->attributes().value("name").toString()
                           <<xml->attributes().value("kurs").toString()
                          <<xml->attributes().value("spec").toString()
                         <<xml->attributes().value("form").toString()
                        <<xml->attributes().value("parent").toString()
                       <<xml->attributes().value("dean").toString();
                    groups->append(gr);
                }
            }
        }
    }
    xml_file.close();
}

void xml_read_write::groups_save_to_xml(const QStringList &c_user, QVector<QStringList> *groups){
    QString xml_file_groups = QString("dean_data/%1/groups_%2.xml").arg(curr_year).arg(c_user[1]);
//    qInfo("%s",xml_file_groups.toUtf8().constData());
    QFile file(xml_file_groups);
    if(!file.open(QIODevice::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",xml_file_groups.toUtf8().constData());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);      // Устанавливаем автоформатирование текста
    xmlWriter.writeStartDocument();         // Запускаем запись в документ
    xmlWriter.writeStartElement("groups");   // Записываем первый элемент с его именем
    for(auto gr:*groups){
        xmlWriter.writeStartElement("group");
        xmlWriter.writeAttribute("name",gr[0]);
        xmlWriter.writeAttribute("kurs",gr[1]);
        xmlWriter.writeAttribute("spec",gr[2]);
        xmlWriter.writeAttribute("form",gr[3]);
        xmlWriter.writeAttribute("parent",gr[4]);
        xmlWriter.writeAttribute("dean",gr[5]);
        xmlWriter.writeEndElement();        // Закрываем тег
    }
    xmlWriter.writeEndElement(); // Закрываем тег "groups"
    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл

    //Проверяем была ли вставка новой группы.
    //Если да, делаем каталог и пустой файл студентов
    for(auto gr:*groups){
        QString group_dir = QString("dean_data/%1/%2").arg(curr_year).arg(gr[0]);;
        QDir dir(group_dir);
        if (!dir.exists()){
            dir.mkdir(".");
            create_students_xml_blank(group_dir+"/list_of_students.xml");
        }
    }
}
