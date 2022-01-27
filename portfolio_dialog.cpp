#include <QStyleFactory>
#include <QtWidgets/QTextBrowser>
#include <QtGui/QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QProcess>
#include <QMessageBox>

#include "portfolio_dialog.h"
#include "prev_edu_dialog.h"
#include "edu_activity_dialog.h"
#include "other_activity_dialog.h"

void Portfolio_Dialog::on_pushButton_other_pressed(){
    other_activity_dialog *oad = new other_activity_dialog(this);
    if(oad->exec()){
        oad->save_table_to_file();
    }
}

void Portfolio_Dialog::on_pushButton_edu_pressed(){
    edu_activity_dialog *ead = new edu_activity_dialog(this);
    if(ead->exec()){
        ead->save_table_to_file();
    }
}

void Portfolio_Dialog::on_pushButton_prev_edu_pressed(){
    prev_edu_dialog *ped = new prev_edu_dialog(this);
    if(ped->exec()){
        ped->save_table_to_file();
    }
}

QString Portfolio_Dialog::format_text_to_string(const QString &str,
                                                        int width,
                                                        int hor_font_size){
    QStringList res;
    QStringList spl = str.split(" ");
    int curr = 0; res.append("");
    for(auto s:spl){
        if(res[curr].size()*hor_font_size+s.size()*hor_font_size<width){
            res[curr]+=s+" ";
        } else {
            res.append(s+" ");
            curr++;
        }
    }
    QString result;
    for(int i=0;i<res.size();++i) result+=res[i]+"\n";
    return result;
}

QPixmap* Portfolio_Dialog::paint_text_to_label_rect(QLabel *label){
    QRect rect(0,0,label->geometry().width(),label->geometry().height());
    QPixmap *pixmap = new QPixmap(rect.width(),rect.height());
    QPainter painter(pixmap);
    painter.fillRect(rect, palette().color(QPalette::Background));
    QString tmp_str = format_text_to_string(label->text(),label->geometry().width(),label->font().pointSize());
    painter.setFont(label->font());
    painter.drawText(rect, Qt::AlignCenter, tmp_str);
    return pixmap;
}

void student_data::load_data_from_xml(){
    QFile file(xml_file_name);
    if(!QFileInfo(xml_file_name).exists()){
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            qFatal("Ошибка создания файла %s",xml_file_name.toUtf8().constData());
        QTextStream out(&file);
        out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        out << "<!DOCTYPE data_student>\n";
        out << "<data_student>\n";
        out << "</data_student>\n";
        file.close();
        perevods.append(QStringList()<<""<<""<<"");
        prevs_edu.append(QStringList()<<""<<""<<"");
        result_control.append(QStringList()<<""<<""<<""<<""<<""<<""<<""<<""<<"");
        kr_control.append(QStringList()<<""<<""<<""<<""<<""<<"");
        pract_control.append(QStringList()<<""<<""<<""<<""<<""<<""<<"");
        dp_control.append(QStringList()<<""<<""<<""<<"");
        nir_control.append(QStringList()<<""<<"");
        extra_edu_control.append(QStringList()<<""<<"");
        add_edu_control.append(QStringList()<<""<<""<<"");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(nullptr,
                            "Чтение данных",
                            QString("Ошибка открытия файла %1").arg(xml_file_name),
                            QMessageBox::Ok);
        return;
    }

    auto xml = new QXmlStreamReader(&file);
    while (!xml->atEnd() && !xml->hasError()) {
        xml->readNext();
        QString name = xml->name().toString();
        if ((xml->isStartElement()) && (name == QString("data_student"))) {
            while (!(xml->isEndElement() && name == QString("data_student"))) {
                xml->readNext();
                name = xml->name().toString();
                if (xml->isStartElement()){
                    if(name == QString("name_engl")) name_engl = xml->readElementText();
                    if(name == QString("name_code")) name_code = xml->readElementText();
                    if(name == QString("data_born")) data_born = xml->readElementText();
                    if(name == QString("prikaz")) prikaz = xml->readElementText();
                    if(name == QString("budget_contract")) budget_contract = xml->readElementText();
                    if(name == QString("add_data")) add_data = xml->readElementText();
                    if(name == QString("kontakts")) kontakts = xml->readElementText();
                    if(name == QString("perevod"))
                        perevods.append(QStringList()
                                        <<xml->attributes().value("kurs").toString()
                                        <<xml->attributes().value("prikaz").toString()
                                        <<xml->attributes().value("soderg").toString());
                    if(name == QString("prev_edu"))
                        prevs_edu.append(QStringList()
                                        <<xml->attributes().value("period").toString()
                                        <<xml->attributes().value("name_org").toString()
                                        <<xml->attributes().value("level_edu").toString());
                    if(name == QString("result_control"))
                        result_control.append(QStringList()
                                        <<xml->attributes().value("disciplina").toString()
                                        <<xml->attributes().value("ze").toString()
                                        <<xml->attributes().value("kurs").toString()
                                        <<xml->attributes().value("semestr").toString()
                                        <<xml->attributes().value("data").toString()
                                        <<xml->attributes().value("form_control").toString()
                                        <<xml->attributes().value("ball").toString()
                                        <<xml->attributes().value("ball_100").toString()
                                        <<xml->attributes().value("ball_7").toString());
                    if(name == QString("kr_control"))
                        kr_control.append(QStringList()
                                        <<xml->attributes().value("disciplina").toString()
                                        <<xml->attributes().value("semestr").toString()
                                        <<xml->attributes().value("tema").toString()
                                        <<xml->attributes().value("ball").toString()
                                        <<xml->attributes().value("ball_100").toString()
                                        <<xml->attributes().value("ball_7").toString());
                    if(name == QString("pract_control"))
                        pract_control.append(QStringList()
                                        <<xml->attributes().value("vid").toString()
                                        <<xml->attributes().value("basa").toString()
                                        <<xml->attributes().value("work").toString()
                                        <<xml->attributes().value("data").toString()
                                        <<xml->attributes().value("ball").toString()
                                        <<xml->attributes().value("ball_100").toString()
                                        <<xml->attributes().value("ball_7").toString());
                    if(name == QString("dp_control"))
                        dp_control.append(QStringList()
                                        <<xml->attributes().value("tema").toString()
                                        <<xml->attributes().value("ball").toString()
                                        <<xml->attributes().value("ball_100").toString()
                                        <<xml->attributes().value("ball_7").toString());
                    if(name == QString("nir_control"))
                        nir_control.append(QStringList()<<xml->attributes().value("num").toString()
                                           <<xml->attributes().value("name").toString());
                    if(name == QString("extra_edu_control"))
                        extra_edu_control.append(QStringList()<<xml->attributes().value("num").toString()
                                                 <<xml->attributes().value("name").toString());
                    if(name == QString("add_edu_control"))
                        add_edu_control.append(QStringList()<<xml->attributes().value("num").toString()
                                               <<xml->attributes().value("name").toString()
                                               <<xml->attributes().value("value").toString());
                }
            }
        }
    }
    file.close();
}

void Portfolio_Dialog::save_data_to_xml(){
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
    xmlWriter.writeCharacters(ui->name_engl->text());
    xmlWriter.writeEndElement(); // Закрываем тег "name_engl"

    xmlWriter.writeStartElement("name_code");
    xmlWriter.writeCharacters(ui->name_code->text());
    xmlWriter.writeEndElement(); // Закрываем тег "name_code"

    xmlWriter.writeStartElement("data_born");
    xmlWriter.writeCharacters(ui->name_born->text());
    xmlWriter.writeEndElement(); // Закрываем тег "data_born"

    xmlWriter.writeStartElement("prikaz");
    xmlWriter.writeCharacters(ui->name_prikaz->text());
    xmlWriter.writeEndElement(); // Закрываем тег "prikaz"

    xmlWriter.writeStartElement("budget_contract");
    xmlWriter.writeCharacters(ui->comboBox->currentText());
    xmlWriter.writeEndElement(); // Закрываем тег "prikaz"

    for(auto p:perevods){
        xmlWriter.writeStartElement("perevod");
        xmlWriter.writeAttribute("kurs",p[0]);
        xmlWriter.writeAttribute("prikaz",p[1]);
        xmlWriter.writeAttribute("soderg",p[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "perevod"
    }
    for(auto p:prevs_edu){
        xmlWriter.writeStartElement("prev_edu");
        xmlWriter.writeAttribute("period",p[0]);
        xmlWriter.writeAttribute("name_org",p[1]);
        xmlWriter.writeAttribute("level_edu",p[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "prev_edu"
    }
    for(auto r:result_control){
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
    for(auto r:kr_control){
        xmlWriter.writeStartElement("kr_control");
        xmlWriter.writeAttribute("disciplina",r[0]);
        xmlWriter.writeAttribute("semestr",r[1]);
        xmlWriter.writeAttribute("tema",r[2]);
        xmlWriter.writeAttribute("ball",r[3]);
        xmlWriter.writeAttribute("ball_100",r[4]);
        xmlWriter.writeAttribute("ball_7",r[5]);
        xmlWriter.writeEndElement(); // Закрываем тег "kr_control"
    }
    for(auto r:pract_control){
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
    for(auto r:dp_control){
        xmlWriter.writeStartElement("dp_control");
        xmlWriter.writeAttribute("tema",r[0]);
        xmlWriter.writeAttribute("ball",r[1]);
        xmlWriter.writeAttribute("ball_100",r[2]);
        xmlWriter.writeAttribute("ball_7",r[3]);
        xmlWriter.writeEndElement(); // Закрываем тег "dp_control"
    }
    for(auto r:nir_control){
        xmlWriter.writeStartElement("nir_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeEndElement(); // Закрываем тег "nir_control"
    }
    for(auto r:extra_edu_control){
        xmlWriter.writeStartElement("extra_edu_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeEndElement(); // Закрываем тег "extra_edu_control"
    }
    for(auto r:add_edu_control){
        xmlWriter.writeStartElement("add_edu_control");
        xmlWriter.writeAttribute("num",r[0]);
        xmlWriter.writeAttribute("name",r[1]);
        xmlWriter.writeAttribute("value",r[2]);
        xmlWriter.writeEndElement(); // Закрываем тег "add_edu_control"
    }

    xmlWriter.writeStartElement("add_data");
    xmlWriter.writeCharacters(ui->textEdit_add_data->toPlainText());
    xmlWriter.writeEndElement(); // Закрываем тег "add_data"

    xmlWriter.writeStartElement("kontakts");
    xmlWriter.writeCharacters(ui->textEdit_kontakts->toPlainText());
    xmlWriter.writeEndElement(); // Закрываем тег "kontakts"

    xmlWriter.writeEndElement(); // Закрываем тег "data_student"

    xmlWriter.writeEndDocument(); // Завершаем запись в документ
    file.close();   // Закрываем файл
}

void Portfolio_Dialog::save_table_to_file(){
    if(check_change_data(ptw,&perevods))
        if(yes_msg_save("со сведениями о переводах с курса на курс"))
            copy_table_to_data(ptw,&perevods);
    save_data_to_xml();
}

student_data::student_data(const QString &student_name,
                           startup_data *s_data){
    QMapIterator<QString,QVector<QStringList>> i(s_data->students);
    while (i.hasNext()) {
        i.next();
        for(auto st:i.value())
            if(st[0]==student_name)
                student = QStringList()<<st[0]<<st[1]<<i.key();
    }
    xml_file_name = QString("dean_data/%1/%2/%3_data.xml")
            .arg(s_data->xrw.curr_year)
            .arg(student[2])
            .arg(student[1]);
    png_name = QString("dean_data/%1/%2/%3_pix.png")
            .arg(s_data->xrw.curr_year)
            .arg(student[2])
            .arg(student[1]);
    load_data_from_xml();
}

Portfolio_Dialog::Portfolio_Dialog(const QString &student_name,
                                    startup_data *s_data,
                                    QWidget *parent):
    R_Dialog(parent),student_data(student_name,s_data),
    cam(nullptr),ui(new Ui::Potfolio_Dialog),s_d(s_data),ptw(nullptr){

    ui->setupUi(this);

    ui->name_engl->setText(name_engl);
    ui->name_code->setText(name_code);
    ui->name_born->setText(data_born);
    ui->name_prikaz->setText(prikaz);
    for(auto f:s_d->budget_contract) ui->comboBox->addItem(f);
    int index = ui->comboBox->findText(budget_contract);
    ui->comboBox->setCurrentIndex(index);
    ui->textEdit_add_data->setText(add_data);
    ui->textEdit_kontakts->setText(kontakts);

    QString tmp_str = format_text_to_string(s_d->i_names[0],
                                            ui->label_institut->geometry().width(),
                                            ui->label_institut->font().pointSize());
    ui->label_institut->setText(tmp_str);
    ui->label_institut->setAlignment(Qt::AlignCenter);

    QString dean_name;
    for(auto d:s_d->deans)
        if(d[0]==s_d->get_gruops_by_name(student[2])[5])
            dean_name = QString("Факультет: %1").arg(d[1]);
    ui->label_dean->setText(dean_name);

    QString prog_name = QString("Образовательная программа ВПО: %1")
            .arg(s_d->prog[s_d->get_gruops_by_name(student[2])[4].split(".")[1]]);
    ui->label_prog->setText(prog_name);

    QString specialty_name;
    for(auto sp:s_d->specialty)
        if(sp[0]==s_d->get_gruops_by_name(student[2])[4])
            specialty_name = QString("Специальность: %1 %2").arg(sp[0]).arg(sp[1]);
    ui->label_specialty->setText(specialty_name);

    QString spec;
    QString specialization_name;
    for(auto gr:s_d->groups)
        if(gr[0] == student[2])
            spec = gr[2];
    for(auto s:s_d->specialization)
        if(s[1] == spec)
            specialization_name = QString("Специализация: %1").arg(s[0]);
    ui->label_specialization->setText(specialization_name);

    QString group_name = QString("Группа: %1").arg(student[2]);
    ui->label_group->setText(group_name);

    QString form_name = QString("(форма обучения: %1)").arg(s_d->get_gruops_by_name(student[2])[3]);
    ui->label_form->setText(form_name);

    QString name = QString("Фамилия, имя, отчество: %1").arg(student[0]);
    ui->label_name->setText(name);
    ui->label_name_engl->setText(QString("Фамилия, имя (англ):"));
    ui->label_code->setText(QString("Шифр:"));
    ui->label_name_born->setText(QString("Дата рождения:"));
    ui->label_name_prikaz->setText(QString("Зачислен (ный/на) приказом от:"));

    load_foto();
    ui->label_foto->setPixmap(pix_foto);

    QStringList sl;
    for(int c=0;c<ui->table_perewod->horizontalHeader()->count();++c)
        sl<<ui->table_perewod->horizontalHeaderItem(c)->text();
    ptw = new tt_widget(sl, &perevods, this);
    ptw->setGeometry(ui->table_perewod->geometry());
    ptw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ptw,&tt_widget::save_table_to_file,this,&Portfolio_Dialog::save_table_to_file);    
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(comboBox_activated(int)));
}

void Portfolio_Dialog::comboBox_activated(int index){
    Q_UNUSED(index);
    save_data_to_xml();
}

void Portfolio_Dialog::save_foto(){
    QFile file(png_name);
    file.open(QIODevice::WriteOnly);
    pix_foto.save(&file, "PNG");
    file.close();
}

void Portfolio_Dialog::load_foto(){
    if(!pix_foto.load(png_name, "PNG"))
        qWarning("Ошибка чтения файла %s",png_name.toUtf8().constData());
}


void Portfolio_Dialog::captured(QImage im){
    QImage im_tmp = im.scaled(ui->label_foto->size(),
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);
    if(pix_foto.convertFromImage(im_tmp)){
        ui->label_foto->setPixmap(pix_foto);
        save_foto();
    } else qWarning("Error converting foto from QImage");
    cam->close();
    delete cam;
}

void Portfolio_Dialog::on_pushButton_cam_pressed(){
    cam = new Camera(this);
    connect(cam, &Camera::captured, this, &Portfolio_Dialog::captured);
    cam->show();
}

void Portfolio_Dialog::draw_label_text_to_pdf(QPainter *p,
                                              qreal koef,
                                              QLabel *label,
                                              QLineEdit *le){
    QRectF res = QRectF(label->geometry().x()*koef,
                        label->geometry().y()*koef,
                        label->geometry().width()*koef,
                        label->geometry().height()*koef);
    p->setFont(label->font());
    if(le!=nullptr){
        res.setWidth(res.width()+le->geometry().width()*koef);
        p->drawText(res,label->text()+le->text(),label->alignment());
    } else p->drawText(res,label->text(),label->alignment());
}

void Portfolio_Dialog::on_pushButton_pdf_pressed(){
    save_table_to_file();
    QString file_name = QString("dean_data/%1/%2/%3_portfolio.pdf")
            .arg(s_d->xrw.curr_year)
            .arg(student[2])
            .arg(student[1]);
    if(!QFileInfo::exists(file_name)) create_pdf_file(file_name);
    else {
        int res = QMessageBox::warning(this,
                    tr("Формирование портфолио"),
                    tr("Портфолио уже сформировано.\n"
                    "Сформировать заново?"),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(res == QMessageBox::Yes) create_pdf_file(file_name);
    }
    QProcess::startDetached(s_d->xrw.get_adobe_path(), QStringList() << "/A" << "page=1" << file_name);
}

void Portfolio_Dialog::create_pdf_file(const QString &file_name){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
//    printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
    printer.setFullPage(false);

    printer.setOutputFileName(file_name);
    printer.setOutputFormat(QPrinter::PdfFormat);

    QTextDocument document;
    QString html_str = QString("<p style=\"text-align: center;\">"
                               "<strong><font size=\"+2\">%1</font>"
                               "</strong></p>").arg(s_d->i_names[0]);
    html_str += "<p>&nbsp;</p>";

    html_str += QString("<p><img src=\"%1\" alt=\"Фотография\" "
                        "align=\"right\" vspace=\"5\" "
                        "hspace=\"5\">").arg(png_name);

    html_str += QString("<p>%1</p>").arg(ui->label_dean->text());
    html_str += QString("<p>%1</p>").arg(ui->label_prog->text());
    html_str += QString("<p>%1</p>").arg(ui->label_specialty->text());
    html_str += QString("<p>%1</p>").arg(ui->label_specialization->text());
    html_str += QString("<p>%1</p>").arg(ui->label_group->text());

    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+2\">%1</font>"
                       "</strong></p>").arg(ui->label_portfolio_name->text());
    html_str += QString("<p style=\"text-align: center;\">%1</p>").arg(ui->label_form->text());
    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p>%1</p>").arg(ui->label_name->text());
    html_str += QString("<p>%1</p>").arg(ui->label_name_engl->text()+" "+ui->name_engl->text());
    html_str += QString("<p>%1</p>").arg(ui->label_name_born->text()+" "+ui->name_born->text());
    html_str += QString("<p>%1</p>").arg(ui->label_name_prikaz->text()+" "+ui->name_prikaz->text());
    html_str += QString("<p>%1</p>").arg(ui->label_code->text()+" "+ui->name_code->text());
    html_str += "</pre></font></strong></p><p>&nbsp;</p>";
    html_str += QString("<p><strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(ui->label_table_head->text());
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(int c=0;c<ui->table_perewod->columnCount();++c)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(ui->table_perewod->horizontalHeaderItem(c)->text());
    html_str += QString("</tr>");
    for(auto p:perevods)
        html_str += QString("<tr><td>%1</td>\n<td>%2</td>\n<td>%3</td></tr>")
                .arg(p[0]).arg(p[1]).arg(p[2]);
    html_str += QString("</table>");

    html_str += QString("<p><strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(ui->label_add_data->text());
    html_str += QString("<p>%1</p>").arg(ui->textEdit_add_data->toPlainText());

    html_str += QString("<p><strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(ui->label_kontakts->text());
    html_str += QString("<p>%1</p>").arg(ui->textEdit_kontakts->toPlainText());

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_prev_edu_table);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:prev_edu_table)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p:prevs_edu)
        html_str += QString("<tr><td>%1</td>\n<td>%2</td>\n<td>%3</td></tr>")
                .arg(p[0]).arg(p[1]).arg(p[2]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_result_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
//    for(auto s:head_result_control)
//        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
//    html_str += QString("</tr>");
    html_str += QString("<tr><td style=\"width: 30.0%; text-align: center;\"><strong>%1</strong></td>"
                        "<td style=\"width: 10.0%; text-align: center;\"><strong>%2</strong></td>"
                        "<td style=\"width: 5.0%; text-align: center;\"><strong>%3</strong></td>"
                        "<td style=\"width: 5.0%; text-align: center;\"><strong>%4</strong></td>"
                        "<td style=\"width: 10.0%; text-align: center;\"><strong>%5</strong></td>"
                        "<td style=\"width: 10.0%; text-align: center;\"><strong>%6</strong></td>"
                        "<td style=\"width: 15.0%; text-align: center;\"><strong>%7</strong></td>"
                        "<td style=\"width: 10.0%; text-align: center;\"><strong>%8</strong></td>"
                        "<td style=\"width: 5.0%; text-align: center;\"><strong>%9</strong></td></tr>")
            .arg(head_result_control[0]).arg(head_result_control[1]).arg(head_result_control[2])
            .arg(head_result_control[3]).arg(head_result_control[4]).arg(head_result_control[5])
            .arg(head_result_control[6]).arg(head_result_control[7]).arg(head_result_control[8]);

    for(auto p_two:result_control)
//        html_str += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td><td>%8</td><td>%9</td></tr>")
        html_str += QString("<tr><td style=\"width: 30.0%;\">%1</td>"
                        "<td style=\"width: 10.0%;\">%2</td>"
                        "<td style=\"width: 5.0%;\">%3</td>"
                        "<td style=\"width: 5.0%;\">%4</td>"
                        "<td style=\"width: 10.0%;\">%5</td>"
                        "<td style=\"width: 10.0%;\">%6</td>"
                        "<td style=\"width: 15.0%;\">%7</td>"
                        "<td style=\"width: 10.0%;\">%8</td>"
                        "<td style=\"width: 5.0%;\">%9</td></tr>")
            .arg(p_two[0]).arg(p_two[1]).arg(p_two[2])
            .arg(p_two[3]).arg(p_two[4]).arg(p_two[5])
            .arg(p_two[6]).arg(p_two[7]).arg(p_two[8]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_kr_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_kr_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:kr_control)
        html_str += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(p_two[0]).arg(p_two[1]).arg(p_two[2])
                .arg(p_two[3]).arg(p_two[4]).arg(p_two[5]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_pract_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_pract_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:pract_control)
        html_str += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td></tr>")
                .arg(p_two[0]).arg(p_two[1]).arg(p_two[2])
                .arg(p_two[3]).arg(p_two[4]).arg(p_two[5]).arg(p_two[6]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_dp_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_dp_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:dp_control)
        html_str += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                .arg(p_two[0]).arg(p_two[1]).arg(p_two[2]).arg(p_two[3]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_nir_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_nir_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:nir_control)
        html_str += QString("<tr><td>%1</td><td>%2</td></tr>")
                .arg(p_two[0]).arg(p_two[1]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_extra_edu_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_extra_edu_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:extra_edu_control)
        html_str += QString("<tr><td>%1</td><td>%2</td></tr>")
                .arg(p_two[0]).arg(p_two[1]);
    html_str += QString("</table>");

    html_str += "<p>&nbsp;</p>";
    html_str += QString("<p style=\"text-align: center;\">"
                       "<strong><font size=\"+1\">%1</font>"
                       "</strong></p>").arg(name_add_edu_control);
    html_str += QString("<table border=\"1\" width=\"100%\" bgcolor=\"#FFFFFF\" "
                        "bordercolor=\"#000000\" cellspacing=\"0\" cellpadding=\"2\">");
    html_str += QString("<tr>");
    for(auto s:head_add_edu_control)
        html_str += QString("<td style=\"text-align: center;\"><strong>%1</strong></td>").arg(s);
    html_str += QString("</tr>");
    for(auto p_two:add_edu_control)
        html_str += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(p_two[0]).arg(p_two[1]).arg(p_two[2]);
    html_str += QString("</table>");
/*
    QString html_file_name = file_name.split(".")[0]+".html";
    QFile html_file(html_file_name);
    if (html_file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&html_file);
        stream << html_str << endl;
        html_file.close();
    } else qWarning("Ошибка записи файла %s",html_file_name.toUtf8().constData());
*/
    document.setHtml(html_str);
    document.print(&printer);
    QApplication::restoreOverrideCursor();
}
