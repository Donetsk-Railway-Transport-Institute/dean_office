#ifndef DEANOFFICEWINDOW_H
#define DEANOFFICEWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTabWidget>
#include <QMessageBox>
#include <QCryptographicHash>

#include "startup_data.h"
#include "students_table_widget.h"

namespace Ui {
class DeanOfficeWindow;
}

class DeanOfficeWindow : public QMainWindow
{
    Q_OBJECT

public:
    static int const EXIT_CODE_REBOOT = -4444;
    QTabWidget *tabWidget;

    explicit DeanOfficeWindow(QWidget *parent = nullptr);
    ~DeanOfficeWindow();

public slots:
    //Слоты меню (начало):
    void on_change_password_triggered();    //изменение пароля
    void on_find_triggered();               //поиск студента
    void on_quit_triggered(){ close(); }    //выход
    void on_arx_to_ftp_triggered();         //сохранение архивной копии БД на FTP сервере
    void on_edit_reports_triggered();       //редактор отчетов
    void on_transfer_to_next_year_triggered();   //перевод на следующий учебный год
    void on_change_curr_year_triggered();   //изменение учебного года
    void on_edit_specialty_triggered();
    void on_edit_specialization_triggered();
    void on_edit_groups_triggered();
    void on_edit_students_triggered();
    void on_edit_transfer_table_triggered();
    void on_deleted_students_triggered();   //удаленные студенты
    //Окончание слотов меню

    //по одиночному клику выводится в статусбар дерево
    //выбранного элемента (элементы отделяются через пробелом)
    void itemClicked(QTreeWidgetItem *item, int column);
    //по двойному щелчку на группе выводится список студентов в виде таблицы
    void itemDoubleClicked(QTreeWidgetItem *item, int column);

    //выполнение действий при закрытии tab
    void tabCloseRequested(int index);
    //выполнение действий при смене tab
    void currentChanged(int index);
    //предварительный просмотр печати
    void run_report(const QString &file_name);

    void about_box(){
        auto msgBox = new QMessageBox(this);
        msgBox->setFixedSize(320,240);
        msgBox->setWindowTitle("О программе");
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->setInformativeText(startup_data::copyright_string());
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    }

private:
    QStringList *g_log;
    startup_data *s_d;    
    const QString dir_reports;

    Ui::DeanOfficeWindow *ui;
    //текущая таблица списка студентов (изменяется в currentChanged)
    students_table_widget *curr_tab;

    //Отображение дерева специальностей, специализаций, групп (стац и заочн)
    void view_tree_widget();

    //Отображение студентов группы в таблице.
    //Если таблица существует, то выделяется студент
    //группы как результат работы слота on_find_triggered(),
    //если таблица группы не существует, то создается
    void view_students_group(const QString &group_name, const QString &selected = "");
};

#endif // DEANOFFICEWINDOW_H
