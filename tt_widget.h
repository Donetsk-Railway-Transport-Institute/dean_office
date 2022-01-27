#ifndef CRNEURALNET_TT_WIDGET_H
#define CRNEURALNET_TT_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

//Базовый класс для всех таблиц, в котором
//реализованы общие методы редактирования
class tt_widget : public QTableWidget {
Q_OBJECT
public:
    //В параметре title.size()==data[0].size()==data[1].size()== и т.д.
    //tile - титул таблицы; data[0], data[1] и т.д. -
    //первая, вторая и т.д. строки таблицы
    explicit tt_widget(const QStringList &title, const QVector<QStringList> *data, QWidget *parent = nullptr);
    tt_widget(QWidget *parent = nullptr);

    //если были изменения и таблица должна быть сохранена
    //то выдается сообщение о необходимости сохранения.
    //По выбору пользователя таблица сохраняется
    //Возвращает true, если было сохранение
    bool message_about_save(const QString &f_name){
        return QMessageBox::question(this,tr("Сохранение файла"),
                                          QString("Сохранить изменения в файле %1?").arg(f_name),
                                          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    }

    //Проверяет наличие пустых колонок в каждой строке
    //если обнаружена, то возвращает номер строки,
    //или возвращает -1, если не обнаружено
    int check_empty_item();

    const QVector<QStringList> *get_p_data(){return p_data;}

signals:
    void save_table_to_file();

public slots:
    //вызываются из выпадающего меню или по горячей клавише        
    virtual void remove_row();
    virtual void insert_row(int iserted_row, const QStringList &str_row);
    virtual void insert_after_row();
    virtual void insert_before_row();

    virtual void copy_to_clipboard();
    virtual void paste_from_clipboard();

protected:    
    //инициализация выпадающего меню и вызов соответствующей выбору пользователя функции
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
    void connected_shortcut();

private:
    const QVector<QStringList> *p_data;
    QStringList get_empty_row();
};

#endif //CRNEURALNET_TT_WIDGET_H
