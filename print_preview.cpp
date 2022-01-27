#include <QFile>
#include <QTextEdit>
#include <QRegularExpression>
#include <QMessageBox>
#include "print_preview.h"

print_preview::print_preview(startup_data *s_data,
                             QObject *parent) :
    QObject(parent),textEdit(nullptr), s_d(s_data){
    QJSValue jsFactory = myEngine.newQObject(this);
    myEngine.globalObject().setProperty("f", jsFactory);
}

void print_preview::prepare_data(print_preview::HTML_PLAIN_TEXTS hpt){
    changer.clear();
    QString first_tag = "<script>";
    QString end_tag = "</script>";
    QRegularExpression re(first_tag+"(.*?)"+end_tag);
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(hpt.plain);
    while (matchIterator.hasNext()){
        QRegularExpressionMatch match = matchIterator.next();
        QString tmp_str = match.capturedTexts()[0];
        QString fn_str = tmp_str.mid(first_tag.size(),tmp_str.size()-(first_tag.size()+end_tag.size()));
//        qInfo("%s",fn_str.toUtf8().constData());
        QString eval_str = hpt.js +"\nvar t = f.create();\n"+fn_str+";";
//        qInfo("%s",eval_str.toUtf8().constData());
        QJSValue v = myEngine.evaluate(eval_str);
        QString result = v.toString();
//        qInfo("result = %s",result.toUtf8().constData());
        changer.append({tmp_str, result});
//        str.replace(iconsRegExp,result);
    }
}

print_preview::HTML_PLAIN_TEXTS print_preview::get_html_with_js(const QString &html_name){
    HTML_PLAIN_TEXTS r;
    if (!QFile::exists(html_name)){
        QMessageBox::warning(nullptr, "Редактор отчетов",
                             QString("Файл %1 не найден").arg(html_name),
                             QMessageBox::Ok);
        return r;
    }
    QFile file(html_name);
    if (!file.open(QFile::ReadOnly)){
        QMessageBox::warning(nullptr, "Редактор отчетов",
                             QString("Ошибка открытия файла %1").arg(html_name),
                             QMessageBox::Ok);
        return r;
    }

    r.plain = QString::fromUtf8(file.readAll()); //html файл

    QString js_name = html_name.split(".html")[0]+".js";
    if (!QFile::exists(js_name)){
        QMessageBox::warning(nullptr, "Редактор отчетов",
                             QString("Файл %1 не найден").arg(js_name),
                             QMessageBox::Ok);
        return HTML_PLAIN_TEXTS();
    }
    QFile js_file(js_name);
    if (!js_file.open(QFile::ReadOnly)){
        QMessageBox::warning(nullptr, "Редактор отчетов",
                             QString("Ошибка открытия файла %1").arg(js_name),
                             QMessageBox::Ok);
        return HTML_PLAIN_TEXTS();
    }
    r.js = QString::fromUtf8(js_file.readAll()); //js файл
    prepare_data(r);
    r.html = replace_html_on_plain(r.plain);
    return r;
}

QString print_preview::replace_html_on_plain(const QString &plain){
    QString tmp = plain;
    for(auto c:changer){
        tmp.replace(c.reg,c.func);
//        qInfo("%s - %s",c.func.toUtf8().constData(),c.reg.toUtf8().constData());
    }
    return tmp;
}

void print_preview::test_changer_1(const QString &html,const QString &func){
    QString tmp = html;
    if(!tmp.contains(func)){
        int res = 0;
        int count_ch = 1;
        QString cut;
        while((res>=0)&&(count_ch<func.size())){
            cut = func.mid(0,count_ch);
            res = tmp.indexOf(cut);
            ++count_ch;
        }
        qInfo("res = %d count_ch=%d %s", res, count_ch, cut.toUtf8().constData());
        qInfo("%s",tmp.mid(res).toUtf8().constData());
        qInfo("%s",func.toUtf8().constData());
    }
}

QString print_preview::replace_plain_on_html(const QString &html){
    QString tmp = html;
    for(auto c:changer){
//        qInfo("%s",tmp.contains(c.func)?"yes":"no");
        test_changer_1(tmp,c.func);
        tmp.replace(c.func,c.reg);
//        qInfo("%s - %s",c.func.toUtf8().constData(),c.reg.toUtf8().constData());
    }
//    qInfo("%s",tmp.toUtf8().constData());
    return tmp;
}

void add_data_to_changer(const QString &func){
    QString func_name = func.split("{")[0];
    func_name = func_name.split("function")[1];
    func_name.remove(' ');
    func_name.remove('\n');
    QString scr = "<script src=\""+func_name+"\"/>";
    qInfo("scr = %s",scr.toUtf8().constData());
}

void print_preview::preview(const QString &file_name){
    QString str = get_html_with_js(file_name).html;
    if(str.isEmpty()) return;
    textEdit = new QTextEdit();
    textEdit->setHtml(str);

    QPrinter printer(QPrinter::HighResolution);
    QString pdf_name = file_name;
    pdf_name.replace("html","pdf");
    printer.setOutputFileName(pdf_name);
    printer.setOutputFormat(QPrinter::PdfFormat);

    QPrintPreviewDialog p_view(&printer);
    connect(&p_view, &QPrintPreviewDialog::paintRequested, this, &print_preview::sl_preview);
    p_view.exec();
}

void print_preview::sl_preview(QPrinter *printer){
    textEdit->print(printer);
}

