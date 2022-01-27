#include "recognition_window.h"
#include "ui_recognition_window.h"
#include <QFileDialog>
#include <QTextCodec>
#include <QTextDecoder>
#include <QClipboard>
#include <QRegExp>
#include <QXmlStreamWriter>

void recognition_window::on_control_BD_triggered(){
    int counter = 0;
    ui->textEdit->append("Проверка целостности баз данных.");
    ui->textEdit->append(s_d->xrw.curr_year+" учебный год.\n");
    for(auto gr:s_d->groups){
        int count_gr = 0;
        ui->textEdit->append(gr[0]);
        for(auto st:s_d->students[gr[0]]){
            if(st[0].indexOf(" ")==0){
                ui->textEdit->append(st[0]+" пробел");
            } else ui->textEdit->append(st[0]);
            counter++;
            count_gr++;
        }
        ui->textEdit->append(QString("\t\tИтого - %1 студентов\n").arg(count_gr));
    }
    ui->textEdit->append(QString("Общее количество - %1 студентов").arg(counter));
}

void recognition_window::on_Open_triggered(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "",QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty()){
        //УЖТ(2020-2021)
        QDir d(dir);
        QString dean = d.dirName().split("(")[0];
        if(s_d->deans_indexOf(dean)==-1){
            QMessageBox::warning(this,
                                 tr("Ошибка импорта"),
                                 QString("%1 - деканат с таким названием не найден").arg(dean));
            return;
        }
        if(s_d->xrw.curr_user[1]!=dean){
            QMessageBox::warning(this,
                                 tr("Ошибка импорта"),
                                 QString("У Вас отсутствует допуск для выполнения импорта данных!"));
            return;
        }
        QString years = d.dirName().split("(")[1].split(")")[0];
        QDir data_dir(s_d->xrw.data_dir);
        QDir years_dir(s_d->xrw.data_dir+"\\"+years);
        QStringList dirs = data_dir.entryList(QDir::Dirs);
        if(dirs.contains(years)){
            if(QMessageBox::warning(this,
                                    tr("Внимание!"),
                                    QString("Каталог с данными за %1 "
                                            "учебный год уже существует. "
                                            "Удалить и перезаписать данные?").arg(years),
                                    QMessageBox::Ok | QMessageBox::Cancel)==QMessageBox::Cancel) return;
            if(!years_dir.removeRecursively()){
                QMessageBox::warning(this,
                                     tr("Ошибка импорта"),
                                     QString("Невозможно удалить каталог %s").arg(years_dir.absolutePath()));
                return;
            }
        }
        if(!data_dir.mkdir(years)){
            QMessageBox::warning(this,
                                 tr("Ошибка импорта"),
                                 QString("Ошибка создания каталога %s").arg(years));
            return;
        }
        QDir files_dir(dir);
        rec_files = files_dir.entryList(QStringList() << "*.doc" << "*.docx"<< "*.DOC" << "*.DOCX", QDir::Files);
        QVector<QStringList> new_groups;
        for(auto f:rec_files){
            //1-ПСЖД;1;ПСЖД;очная.docx
            QStringList fl = f.split(";");
            fl[3] = fl[3].split(".")[0];
            fl<<s_d->get_code_by_cut_name(fl[2])<<s_d->xrw.curr_user[1];
            new_groups.append(fl);
            if(!years_dir.mkdir(fl[0])){
                QString add_msg;
                if(years_dir.exists(fl[0]))
                    add_msg="Каталог уже существует.";
                QMessageBox::warning(this,
                                     tr("Ошибка импорта"),
                                     QString("Ошибка создания каталога %1. %2").arg(fl[0]).arg(add_msg));
                return;
            }
            s_d->xrw.create_students_xml_blank(years_dir.absolutePath()+"\\"+fl[0]+"\\list_of_students.xml");
        }
        s_d->xrw.curr_year = years;

        s_d->xrw.groups_save_to_xml(s_d->xrw.curr_user,&new_groups);
        s_d->xml_curr_data_save();

        for(auto f:rec_files){
//            if(f.contains("1-ЭЖД")) qFatal(" ");
            open_doc(dir+"/"+f);
        }
    }
}

void recognition_window::open_doc(const QString &filename){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QFileInfo fi(filename);
//    qInfo("%s",fi.path().toUtf8().constData());
    QDir tmp_dir(fi.path()+"/"+xml_read_write::randx());
    if (!tmp_dir.mkdir(".")) qFatal("Error mkdir %s",tmp_dir.absolutePath().toUtf8().constData());
//    qInfo("mkdir %s",tmp_dir.absolutePath().toUtf8().constData());
    QString tmp_doc(tmp_dir.absolutePath()+"/tmp.doc");
    QString tmp_html(tmp_dir.absolutePath()+"/tmp.html");
    QFile::copy(filename, tmp_doc);

    QAxObject *wordApplication = new QAxObject("Word.Application");
    QAxObject *documents = wordApplication->querySubObject("Documents");
    QAxObject *document = documents->querySubObject("Open(const QVariant&, bool)", tmp_doc, true);
    wordApplication->setProperty("Visible", true);

    document->querySubObject("SaveAs(const QVariant&, int)", tmp_html, 0x0000000A);
    document->dynamicCall("Close (boolean)", false);
    wordApplication->dynamicCall("Quit()");
    delete wordApplication;

    QFile file(tmp_html);
    if (!file.open(QFile::ReadOnly))
        qFatal("Error open %s",tmp_html.toUtf8().constData());

    QTextDecoder decoder(QTextCodec::codecForName("Windows-1251"));
    QString text = decoder.toUnicode(file.readAll());
    ui->textEdit->setText(text);
    file.close();
    QApplication::restoreOverrideCursor();

    QString student_file_name = s_d->xrw.data_dir+
            "/"+s_d->xrw.curr_year+"/"+
            filename.split("/").last().split(";").first()+
            "/list_of_students.xml";
//    qInfo("%s",student_file_name.toUtf8().constData());

    rec_students(ui->textEdit->toPlainText(), student_file_name);
    tmp_dir.removeRecursively();
}

void recognition_window::rec_students(const QString &data, const QString &s_f_name){
    QVector<QStringList> res;
    QVector<QStringList> b_c; //[0]-имя [1]-бюджет-контракт
    QStringList sl = data.split("\n");
    for(int i=0;i<sl.size();++i) sl[i].replace(QRegExp("[ ]{2,}")," ");
//    for(auto s:sl) qInfo("%s",s.toUtf8().constData());
    bool budget = false;
    bool contract = false;
    int num = 1;
    for(auto s:sl){
//        qInfo("s(before)=%s",s.toUtf8().constData());
        if(s==" ") continue;
        if(s=="  ") continue;
        if(s.contains(s_d->budget_contract[0])){
            contract = false;
            budget = true;
            num = 1;
            continue;
        }
        if(s.contains(s_d->budget_contract[1])){
            contract = true;
            budget = false;
            num = 1;
            continue;
        }
//        qInfo("бюджет - %s",budget?"true":"false");
//        qInfo("контракт - %s",contract?"true":"false");
        QStringList student;
        if(s.indexOf(QString("%1.").arg(num))==0){
//            qInfo("s = %s",s.toUtf8().constData());
            QString tmp_student = s.right(s.length()-3);
//            qInfo("tmp_student = %s",tmp_student.toUtf8().constData());
            while(tmp_student.indexOf(" ")==0) tmp_student.remove(0,1);
            student<<tmp_student;
            if(budget) student<<s_d->budget_contract[0]; //бюджет
            if(contract) student<<s_d->budget_contract[1]; //контракт
            res.append(QStringList()<<student[0]<<"");
            b_c.append(student);
            num++;
        }
    }
    s_d->xrw.save_students_to_file(&res,s_f_name);
    for(int i=0;i<res.size();++i){
        QString ob;
        for(auto bc:b_c)
            if(bc[0]==res[i][0])
                res.replace(i,QStringList()<<res[i][0]<<res[i][1]<<bc[1]);
    }
//    for(auto r:res)
//        qInfo("%s %s %s",r[0].toUtf8().constData(),r[1].toUtf8().constData(),r[2].toUtf8().constData());

    for(auto r:res){
        QString xml_file_name = s_f_name.left(s_f_name.size()-20)+r[1]+"_data.xml";
//        qInfo("%s",xml_file_name.toUtf8().constData());
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

        xmlWriter.writeTextElement("name_engl","");
        xmlWriter.writeTextElement("name_code","");
        xmlWriter.writeTextElement("data_born","");
        xmlWriter.writeTextElement("prikaz","");
        xmlWriter.writeTextElement("budget_contract",r[2]);

        xmlWriter.writeStartElement("perevod");
        xmlWriter.writeAttribute("kurs","");
        xmlWriter.writeAttribute("prikaz","");
        xmlWriter.writeAttribute("soderg","");
        xmlWriter.writeEndElement(); // Закрываем тег "perevod"

        xmlWriter.writeStartElement("prev_edu");
        xmlWriter.writeAttribute("period","");
        xmlWriter.writeAttribute("name_org","");
        xmlWriter.writeAttribute("level_edu","");
        xmlWriter.writeEndElement(); // Закрываем тег "prev_edu"

        xmlWriter.writeStartElement("result_control");
        xmlWriter.writeAttribute("disciplina","");
        xmlWriter.writeAttribute("ze","");
        xmlWriter.writeAttribute("kurs","");
        xmlWriter.writeAttribute("semestr","");
        xmlWriter.writeAttribute("data","");
        xmlWriter.writeAttribute("form_control","");
        xmlWriter.writeAttribute("ball","");
        xmlWriter.writeAttribute("ball_100","");
        xmlWriter.writeAttribute("ball_7","");
        xmlWriter.writeEndElement(); // Закрываем тег "result_control"

        xmlWriter.writeStartElement("kr_control");
        xmlWriter.writeAttribute("disciplina","");
        xmlWriter.writeAttribute("semestr","");
        xmlWriter.writeAttribute("tema","");
        xmlWriter.writeAttribute("ball","");
        xmlWriter.writeAttribute("ball_100","");
        xmlWriter.writeAttribute("ball_7","");
        xmlWriter.writeEndElement(); // Закрываем тег "kr_control"

        xmlWriter.writeStartElement("pract_control");
        xmlWriter.writeAttribute("vid","");
        xmlWriter.writeAttribute("basa","");
        xmlWriter.writeAttribute("work","");
        xmlWriter.writeAttribute("data","");
        xmlWriter.writeAttribute("ball","");
        xmlWriter.writeAttribute("ball_100","");
        xmlWriter.writeAttribute("ball_7","");
        xmlWriter.writeEndElement(); // Закрываем тег "pract_control"

        xmlWriter.writeStartElement("dp_control");
        xmlWriter.writeAttribute("tema","");
        xmlWriter.writeAttribute("ball","");
        xmlWriter.writeAttribute("ball_100","");
        xmlWriter.writeAttribute("ball_7","");
        xmlWriter.writeEndElement(); // Закрываем тег "dp_control"

        xmlWriter.writeStartElement("nir_control");
        xmlWriter.writeAttribute("num","");
        xmlWriter.writeAttribute("name","");
        xmlWriter.writeEndElement(); // Закрываем тег "nir_control"

        xmlWriter.writeStartElement("extra_edu_control");
        xmlWriter.writeAttribute("num","");
        xmlWriter.writeAttribute("name","");
        xmlWriter.writeEndElement(); // Закрываем тег "extra_edu_control"

        xmlWriter.writeStartElement("add_edu_control");
        xmlWriter.writeAttribute("num","");
        xmlWriter.writeAttribute("name","");
        xmlWriter.writeAttribute("value","");
        xmlWriter.writeEndElement(); // Закрываем тег "add_edu_control"

        xmlWriter.writeTextElement("add_data","");
        xmlWriter.writeTextElement("kontakts","");

        xmlWriter.writeEndElement(); // Закрываем тег "data_student"
        xmlWriter.writeEndDocument(); // Завершаем запись в документ
        file.close();   // Закрываем файл
    }
}

recognition_window::recognition_window(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::recognition_window), s_d(new startup_data(this)){
    ui->setupUi(this);    
}

recognition_window::~recognition_window()
{
    delete ui;
}
