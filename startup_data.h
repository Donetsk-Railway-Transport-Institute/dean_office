#ifndef STARTUP_DATA_H
#define STARTUP_DATA_H

#include <QtWidgets/QApplication>
#include <QVector>
#include <QMap>
#include <QtWidgets/QMessageBox>
#include <QtCore/QVariant>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableWidget>
#include <QStyleFactory>

#include "xml_read_write.h"

//рендеринг экрана
class R_Dialog : public QDialog {
    Q_OBJECT
public:
    explicit R_Dialog(QWidget *parent = nullptr) : QDialog(parent){
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setStyle(QStyleFactory::create("Fusion")); // these lines before the next
        connect(this, SIGNAL(finished(int)), this, SLOT(before_exit(int)));
    }

public slots:
    //вызываетя по сигналу finished(int result)
    //для выполнения завершающих действий
    void before_exit(int res){
        if(res == 1) { //нажата ОК
            save_table_to_file();
        }
    }

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file()=0;
    //Проверка наличия изменений в данных
    bool check_change_data(QTableWidget *table, QVector<QStringList> *data){
        if(table->rowCount()!=data->size()) return true;
        for(int r=0;r<table->rowCount();++r)
            for(int c=0;c<table->columnCount();c++)
                if(table->item(r,c)->text()!=data->at(r)[c])
                    return true;
        return false;
    }
    //Копирование данных из table в data
    void copy_table_to_data(QTableWidget *table, QVector<QStringList> *data){
        data->clear();
        for(int r=0;r<table->rowCount();++r){
            QStringList sl;
            for(int c=0;c<table->columnCount();c++) sl<<table->item(r,c)->text();
            data->append(sl);
        }
    }

    bool yes_msg_save(const QString &file_name = ""){
        return QMessageBox::question( this,QString("Сохранение файла"),
                                  QString("Сохранить изменения %1?").arg(file_name),
                                  QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes;
    }
};

class Password_Dialog : public R_Dialog {
Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_name;
    QLabel *label_password;
    QLineEdit *lineEdit_password;
    QComboBox *comboBox_name;

    explicit Password_Dialog(QVector<QStringList> names, QWidget *parent = nullptr);

protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file(){}
};

class Change_Password_Dialog : public R_Dialog {
Q_OBJECT
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_name;
    QLabel *label_user_name;
    QLabel *label_old_password;
    QLineEdit *old_password;
    QLineEdit *new_password_first;
    QLabel *label_new_password_first;
    QLineEdit *new_password_second;
    QLabel *label_new_password_second;

    explicit Change_Password_Dialog(QStringList user_name, QWidget *parent = nullptr);
protected:
    //Сохранение данных в файл.
    virtual void save_table_to_file(){}

};

class startup_data : public QObject
{
    Q_OBJECT
public:
    //Функции записи-чтения xml файлов
    xml_read_write xrw;

    explicit startup_data(QObject *parent = nullptr);

    //сохраняет в xml измененный пароль текущего пользователя
    void change_password_at_xml(const QString &new_pass){
        xrw.change_password_at_xml(new_pass,xrw.curr_user,&users);
    }

    //названия групп по курсу, деканату, спеиализации и форме обучения
    QStringList get_gruops_by_form(int kurs,
                                   const QString &dean,
                                   const QString &spec,
                                   const QString &parent,
                                   const QString &form);

    //возвращает группу по ее названию
    QStringList get_gruops_by_name(const QString &name);

    //возвращает группы деканата
    QStringList get_gruops_by_dean(const QString &dean);

    //возвращает группы текущего деканата
    QStringList get_gruops_by_dean(){ return get_gruops_by_dean(xrw.curr_user[1]); }

    //Герб ДНР
    QPixmap pix_DPR;

    //название института (бывшая струкура INSTITUT_NAMES)
    //[0] - полное название (name);
    //[1] - сокращенное название (cut_name);
    //[2] - аббривеатура (litle_name).
    QStringList i_names;

    //пользователи (бывшая струкура DEAN_USER)
    //[0] - имя(name);
    //[1] - деканат(dean)(сокращенное название);
    //[2] - пароль (pass)
    QVector<QStringList> users; //Список пользователей

    //Специальности ([0] - номер; [1] - название)
    QVector<QStringList> specialty;
    //возвращает специальность по ее коду
    QString get_specialty_by_num(const QString &num){
        for(auto sp:specialty)
            if(sp[0]==num) return sp[1];
        return "";
    }

    //Например: name="Локомотивы" cut_name="ПСЖД" parent="23.05.03" dean="УЖТ"
    //[0] name;   //Название специализации
    //[1] cut_name;   //Сокращенное название специализации (название группы)
    //[2] parent; //Код специальности
    //[3] dean;   //Принадлежность к деканату
    QVector<QStringList> specialization; //Специализация (сокращенное название:полное)
    QString get_code_by_cut_name(const QString &cut_name){
        for(auto sp:specialization)
            if(sp[1]==cut_name) return sp[2];
        return "";
    }
    QString get_name_by_cut_name(const QString &cut_name){
        for(auto sp:specialization)
            if(sp[1]==cut_name) return sp[0];
        return "";
    }

    //     <group name="1-УАз" kurs="1" spec="УА" form="заочн" parent="38.03.01" dean="ИЭЖТ"/>
    //[0] name;   //Название групы
    //[1] kurs;   //Номер курса
    //[2] spec;   //Специализация
    //[3] form;   //Форма обучения (заочн/стац)
    //[4] parent; //Код специальности
    //[5] dean;   //Принадлежность к деканату
    QVector<QStringList> groups;

    //QMap по названиям групп. QVector<QStringList> -
    //[0] name;       //ФИО студента
    //[1] id;         //уникальный id студента
    //[2] complete;   //комплектность портфолио студента
    QMap<QString,QVector<QStringList>> students; //список студенты по названию группы
    QMap<QString,QVector<QStringList>> deleted_students; //Удаленные после редактирования студенты
    QVector<QStringList> deleted_students_from_dir; //Удаленные студенты из каталога $deleted_students_???

    QVector<QString> forms; //Список форм обучения
    QVector<QString> budget_contract; //бюджет или контракт
    QMap<QString,QString> prog; //Программа обучения (уровень:название, например "04:специалитет")

    QVector<QStringList> deans; //Деканаты ([0] сокращенное название, [1] полное)
    //возвращает индекс деканата в массиве deans,
    //если существует сокращенное или полное название
    //в противном случае -1
    int deans_indexOf(const QString &dean);

    static QString copyright_string(){
        return QString("<p style='text-align: center;'><strong>Деканат офис - программа "
                       "учета групп, студентов, учебной, научной и воспитательной работы на факультете.</strong></p>"
                       "<p style='text-align: center;'>dean_office v1.2.0</p>"
                       "<p style='text-align: center;'>Copyright &copy; Cheptsov, 2019&ndash;2020. All rights reserved.</p>"
                       "<p style='text-align: center;'>&nbsp;</p>");
    }

    //запись текущего состояния данных в startup.xml
    void xml_curr_data_save();
};

#endif // STARTUP_DATA_H
