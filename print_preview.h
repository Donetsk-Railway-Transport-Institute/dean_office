#ifndef PRINT_PREVIEW_H
#define PRINT_PREVIEW_H

#include <QFile>
#include <QTextEdit>
#include <QJSEngine>
#include <QBuffer>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#include "startup_data.h"
#include "portfolio_dialog.h"

class Js_Object : public QObject
{
    Q_OBJECT
public:
    explicit Js_Object(startup_data *s_data, QObject *parent = nullptr) :
        QObject(parent), s_d(s_data) {}

    //возвращает преобразованный в символы рисунок из ресурсов
    Q_INVOKABLE QString f_img(const QString &name) {
        QImage image(":/images/"+name);        
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer,"PNG");
        return QString::fromLatin1(arr.toBase64().data());
    }

    //возвращает текущий год
    Q_INVOKABLE QString curr_year(){ return s_d->xrw.curr_year; }

    //возвращает полное название деканата текущего пользователя
    Q_INVOKABLE QString curr_dean(){ return get_dean_full_name(s_d->xrw.curr_user[1]); }

    //возвращает полное название деканата по сокращенному
    Q_INVOKABLE QString get_dean_full_name(const QString &cut_name){
        return s_d->deans[s_d->deans_indexOf(cut_name)][1];
    }

    //возвращает количество студентов в группе
    Q_INVOKABLE int student_list_by_group_size(const QString &name_group){
        return s_d->students[name_group].size();
    }

    //возвращает номер курса по названию группы
    Q_INVOKABLE int get_number_kurs_by_group(const QString &name_group){
        return s_d->get_gruops_by_name(name_group)[1].toInt();
    }

    //возвращает список названий групп по номеру курса и форме обучения
    Q_INVOKABLE QStringList get_groups_by_kurs(const QString &str_n, const QString &forms){
        QStringList ret;
        for(auto gr:s_d->groups)
            if((gr[1]==str_n)&&(gr[3]==forms)) ret<<gr[0];
        return ret;
    }

    //возвращает полное название специальности по имени группы
    Q_INVOKABLE QString get_spec_by_group(const QString &name_group){
        return s_d->get_specialty_by_num(s_d->get_gruops_by_name(name_group)[4]);
    }

    //возвращает фио студента группы name_group с номером number
    Q_INVOKABLE QString get_student_list_by_group(const QString &name_group, int number){
        return s_d->students[name_group][number][0];
    }

    //возвращает форму обучения студента (бюджет/контракт)
    Q_INVOKABLE QString get_student_budget_contract(const QString &name){
        student_data sd(name,s_d);
        return sd.budget_contract;
    }

    //преобразует целочисленное значение num в строку
    Q_INVOKABLE QString to_str(int num){
        return QString("%1").arg(num);
    }

    //записывает строку msg в лог
    Q_INVOKABLE void msg_to_log(const QString &msg){
        qInfo("%s",msg.toUtf8().constData());
    }

    Q_INVOKABLE QString result() { return res; }
    Q_INVOKABLE void add(const QString &par) { res.append(par); }
    QString res;

private:
    startup_data *s_d;
};

class print_preview : public QObject
{
    Q_OBJECT

public:
    //*s_data - указатель на для доступа к данным и функциям из startup_data;
    //func_name - имя функции, которая должна быть выполнена в этом скрипте;
    explicit print_preview(startup_data *s_data,
                           QObject *parent = nullptr);
    Q_INVOKABLE QObject* create() { return new Js_Object(s_d); }

    //html_name - имя html файла, в котором выполняются функции
    //из скрипта (то же имя, но с расширением .js). Имя функции
    //прописано в теге script, например: <script src="pix_1()" />
    //Возвращает HTML_PLAIN_TEXTS, где HTML_PLAIN_TEXTS::html - данные из html файла
    //с замененными тегами на результат работы соответствующей функции,
    //прописанной в данном теге. Реализация функции считанна из js в HTML_PLAIN_TEXTS::js
    struct HTML_PLAIN_TEXTS{
        QString html;
        QString plain;
        QString js;
    };
    void prepare_data(HTML_PLAIN_TEXTS hpt);
    HTML_PLAIN_TEXTS get_html_with_js(const QString &html_name);

    QString replace_html_on_plain(const QString &plain);
    QString replace_plain_on_html(const QString &html);

    //Добавление данных в вектор changer. Передается
    //js функция func, она выполняется, формируется
    //соответствующий тег и результат функционирования.
    void add_data_to_changer(const QString &func);

    //предварительны просмотр и печать файла file_name
    void preview(const QString &file_name);

private slots:
    void sl_preview(QPrinter *printer);

private:
    QJSEngine myEngine;
    QTextEdit *textEdit;
    startup_data *s_d;
    struct CHANGER{
        QString reg; //результат поиска регулярным выражением
        QString func; //результат выполнения скрипта
    };
    QVector<CHANGER> changer;
    void test_changer_1(const QString &html,const QString &func);
};

#endif // PRINT_PREVIEW_H
