#ifndef PORTFOLIO_DIALOG_H
#define PORTFOLIO_DIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

#include "ui_Portfolio_Dialog.h"
#include "startup_data.h"
#include "tt_widget.h"
#include "camera.h"

class student_data{
public:
    explicit student_data(const QString &student_name,
                          startup_data *s_data);

    //Чтение данных студента из xml
    void load_data_from_xml();

    QStringList student;

    QString name_engl;
    QString name_code;
    QString data_born;
    QString prikaz;
    QString budget_contract;
    QString add_data;
    QString kontakts;

    QStringList head_perevods = QStringList()<<""<<""<<"";
    //  <perevod kurs="1" prikaz="31.08.2020 года № 1449/р" soderg="Переведена на 2-й курс"/>
    //[0] kurs;
    //[1] prikaz;
    //[2] soderg;
    QVector<QStringList> perevods;

    QString name_prev_edu_table = "Сведения о предыдущем образовании (в хронологическом порядке)";
    QStringList prev_edu_table = QStringList()<<"Период \n(гг.-гг.)"<<
                                  "Наименование образовательной \nорганизации (учереждения)"<<
                                  "Уровень образования, наименование \nспециальности, направления "
                                  "\nподготовки (профиля),наименование \nприсвоенной квалификации";
    //  <pred_edu period="2015-2017" name_org="Дебальцевский колледж транспортной инфраструктуры" level_edu="специалист среднего звена"/>
    //[0] period;
    //[1] name_org;
    //[2] level_edu;
    QVector<QStringList> prevs_edu;

    QString name_result_control = "Результаты текущего и промежуточного контроля";
    QStringList head_result_control = QStringList()<<"Наименование дисциплины"<<
                                      "Объем"<<"Курс"<<"Семестр"<<"Дата"<<
                                      "Форма\nконтр."<<"Оценка"<<"ESTC(100)"<<"ESTC(7)";
    //<result_control disciplina="Макроэкономика (продвинутый уровень)"
    //ze="108" kurs="1" semestr="1" data="23.01.2019"
    //form_control="экзамен" ball="4" ball_100="80" ball_7="В"/>
    //[0] - disciplina
    //[1] - ze
    //[2] - kurs
    //[3] - semestr
    //[4] - data
    //[5] - form_control
    //[6] - ball
    //[7] - ball_100
    //[8] - ball_7
    QVector<QStringList> result_control;

    QString name_kr_control = "Результаты защиты курсовых работ (проектов)";
    QStringList head_kr_control = QStringList()<<"Наименование дисциплины"<<
                                      "Семестр"<<"Тема курсовой работы (проекта)"<<
                                      "Оценка"<<"ESTC(100)"<<"ESTC(7)";
    //<kr_control disciplina="Экологический маркетинг" semestr="1" tema="" ball="4" ball_100="75" ball_7="C"/>
    //[0] - disciplina
    //[1] - semestr
    //[2] - tema
    //[3] - ball
    //[4] - ball_100
    //[5] - ball_7
    QVector<QStringList> kr_control;

    QString name_pract_control = "Сведения о прохождении практики";
    QStringList head_pract_control = QStringList()<<"Вид практики"<<
                                      "База прохождения\nпрактики\n(предприятие)"<<
                                      "В качестве кого\nработал (должность)"<<
                                      "Сроки\nпрохождения\nпрактики"<<
                                      "Оценка"<<"ESTC(100)"<<"ESTC(7)";
    //<pract_control vid="Производственная" basa="ГП Донецкая ж.д. ст.Донецк"
    //work="Оператор при дежурном по станции" data="01.06.2019-01.07.2019"
    //ball="4" ball_100="80" ball_7="В"/>
    //[0] - vid
    //[1] - basa
    //[2] - work
    //[3] - data
    //[4] - ball
    //[5] - ball_100
    //[6] - ball_7
    QVector<QStringList> pract_control;

    QString name_dp_control = "Выполнение выпускной квалификационной работы";
    QStringList head_dp_control = QStringList()<<"Тема выпускной квалификационной работы"<<
                                      "Оценка"<<"ESTC(100)"<<"ESTC(7)";
    //<dp_control tema="Управление поездной и маневровой работой на участковой станции" ball="4" ball_100="80" ball_7="В"/>
    //[0] - tema
    //[1] - ball
    //[2] - ball_100
    //[3] - ball_7
    QVector<QStringList> dp_control;

    QString name_nir_control = "Научно-исследовательская деятельность (статьи, тезисы, дипломы, грамоты, сертификаты)";
    QStringList head_nir_control = QStringList()<<"№ п.п."<<"Наименование вида деятельности";
    //<nir_control num="1" name="Совершенствование системы управления поездной и грузовой работы станции.\nТезисы XXI
    //студенческой конференции\nДонецкого института железнодорожного транспорта";
    //[0] - num
    //[1] - name
    QVector<QStringList> nir_control;

    QString name_extra_edu_control = "Внеучебная деятельность (староста, профорг, участие в культмассовом секторе и т.д.)";
    QStringList head_extra_edu_control = QStringList()<<"№ п.п."<<"Наименование вида деятельности";
    //<extra_edu_control num="1" name="Староста группы 1-ЭЖД";
    //[0] - num
    //[1] - name
    QVector<QStringList> extra_edu_control;

    QString name_add_edu_control = "Дополнительное образование";
    QStringList head_add_edu_control = QStringList()<<"№ п.п."<<"Название специальности"<<"Разряд";
    //<add_edu_control num="1" name="Приемосдатчик груза и багажа" value="3"
    //[0] - num
    //[1] - name
    //[2] - value
    QVector<QStringList> add_edu_control;

    QString xml_file_name;
    QString png_name;
};

class Portfolio_Dialog: public R_Dialog, public student_data {
    Q_OBJECT

public:
    Camera *cam;

    explicit Portfolio_Dialog(const QString &student_name,
                              startup_data *s_data,
                              QWidget *parent = nullptr);
    ~Portfolio_Dialog(){ cam->close(); delete cam; delete ui; }
    Ui::Potfolio_Dialog *ui;

    //Сохранение данных студента в xml
    void save_data_to_xml();

    QString get_xml_file_name(){ return xml_file_name; }


public slots:
    //Сохранение данных в файл.
    virtual void save_table_to_file();
    void on_pushButton_cam_pressed();
    void on_pushButton_save_pressed(){
        save_table_to_file();
        done(QDialog::Accepted);
    }
    void on_pushButton_cancel_pressed(){
        done(QDialog::Rejected);
    }
    //формирование или печать портфолио (pdf и html файлы)
    void on_pushButton_pdf_pressed();

    //Предыдущее образование
    void on_pushButton_prev_edu_pressed();
    //Учебная деятельность
    void on_pushButton_edu_pressed();
    //Другие виды деятельности
    void on_pushButton_other_pressed();
    //Вызывается после сделанной фотографии
    void captured(QImage im);
    void comboBox_activated(int index);

private:
    startup_data *s_d;

    tt_widget *ptw; //переводы с курса на курс

    //Фотография студента и функции записи и чтения ее с файла
    QPixmap pix_foto;
    void save_foto();
    void load_foto();

    //Форматирует техт по длинне окна (width) c горизонтальным
    //размером фонта (hor_font_size) и возвращает строку не превышающие размер окна
    QString format_text_to_string(const QString &str, int width, int hor_font_size);

    //Возвращает QPixmap с размерами и содержимым label
    QPixmap*  paint_text_to_label_rect(QLabel *label);

    //Выводит текст из QLabel в pdf используя коэффициент koef для расчета координат
    void draw_label_text_to_pdf(QPainter *p, qreal koef, QLabel *label, QLineEdit *le = nullptr);

    //формирование портфолио (pdf и html файлы)
    void create_pdf_file(const QString &file_name);
};

#endif // PORTFOLIO_DIALOG_H
