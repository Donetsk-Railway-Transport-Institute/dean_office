#include <QtWidgets>
#include <QMessageBox>
#include "tt_widget.h"

void tt_widget::connected_shortcut(){
    //Ctrl + C (копировать в буффер обмена)
    connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, this, &tt_widget::copy_to_clipboard);
    //Ctrl + V (вставить из буффера обмена)
    connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated, this, &tt_widget::paste_from_clipboard);
    //Insert (вставить строку после текущей)
    connect(new QShortcut(Qt::Key_Insert, this), &QShortcut::activated, this, &tt_widget::insert_after_row);
    //SHIFT+Insert (вставить строку перед текущей)
    connect(new QShortcut(Qt::SHIFT+Qt::Key_Insert, this), &QShortcut::activated, this, &tt_widget::insert_before_row);
    //Delete (удалить текущую строку)
    connect(new QShortcut(Qt::Key_Delete, this), &QShortcut::activated, this, &tt_widget::remove_row);
}

tt_widget::tt_widget(QWidget *parent) : QTableWidget(parent){ connected_shortcut(); }

tt_widget::tt_widget(const QStringList &title,
                     const QVector<QStringList> *data,
                     QWidget *parent) :
    QTableWidget(parent), p_data(data){
//    qInfo("tt_widget::tt_widget title.size()=%d data.size()=%d",title.size(),data->size());
    setColumnCount(title.size());
    setHorizontalHeaderLabels(title);
    for(int r = 0;r<p_data->size();++r) insert_row(r,p_data->at(r));
    verticalHeader()->hide();
    setCurrentItem(item(0, 0));
    setFocus();
    connected_shortcut();
}

int tt_widget::check_empty_item(){
    for(int r=0;r<rowCount();++r)
        for(int c=0;c<columnCount();++c)
            if(item(r, c)->text().isEmpty()) return r;
    return -1;
}

void tt_widget::contextMenuEvent(QContextMenuEvent *event) {
    const QTableWidgetItem *item = itemAt(event->pos());
    if (!item) return;
    QMenu contextMenu;
    QAction *insert_row_after_Action = contextMenu.addAction(tr("Добавить строку после (Ins)"));
    QAction *insert_row_before_Action = contextMenu.addAction(tr("Добавить строку перед (Shift+Ins)"));
    QAction *deleteAction = contextMenu.addAction(tr("Удалить строку (Del)"));
    QAction *copyAction = contextMenu.addAction(tr("Копировать (Ctr+C)"));
    QAction *insertAction = contextMenu.addAction(tr("Вставить (Ctr+V)"));
    QAction *saveAction = contextMenu.addAction(tr("Сохранить файл"));
    QAction *action = contextMenu.exec(event->globalPos());
    if (action == insert_row_after_Action) insert_after_row();
    if (action == insert_row_before_Action) insert_before_row();
    if (action == deleteAction) remove_row();
    if (action == copyAction) copy_to_clipboard();
    if (action == insertAction) paste_from_clipboard();
    if (action == saveAction) emit save_table_to_file();
}

void tt_widget::remove_row(){
    removeRow(currentItem()->row());
}

void tt_widget::insert_row(int iserted_row, const QStringList &str_row){
//    qInfo("tt_widget::insert_row() iserted_row=%d str_row.size()=%d",iserted_row,str_row.size());
    insertRow(iserted_row);
    for(int c=0;c<columnCount();++c){
        setItem(iserted_row,c, new QTableWidgetItem(str_row[c]));
    }
}

QStringList tt_widget::get_empty_row(){
    QStringList sl;
    for(int c=0;c<columnCount();++c) sl<<"";
    return sl;
}

void tt_widget::insert_after_row(){
//    qInfo("tt_widget::insert_after_row() currentItem()->row()=%d get_empty_row().size()=%d",currentItem()->row(),get_empty_row().size());
    insert_row(currentItem()->row()+1,get_empty_row());
    if(rowCount()==1) setCurrentItem(item(1, 0));
    else setCurrentItem(item(currentItem()->row()+1, 0));
}

void tt_widget::insert_before_row(){
//    qInfo("tt_widget::insert_before_row()");
    insert_row(currentItem()->row(),get_empty_row());
    setCurrentItem(item(currentItem()->row(), 0));
}

void tt_widget::copy_to_clipboard(){
    if(focusWidget()!=this) return;
    QString clipboardString;
    int curr_row = selectedItems()[0]->row();
    for(auto item:selectedItems()){
        if(item->row()!=curr_row){
            clipboardString.replace(clipboardString.size()-1,1,QString("\n"));
            curr_row = item->row();
        }
        clipboardString+=QString("%1\t").arg(item->text());
    }
    clipboardString.remove(clipboardString.size()-1,1);
    QApplication::clipboard()->setText(clipboardString);
}

void tt_widget::paste_from_clipboard(){
    if(focusWidget()!=this) return;
    QTableWidgetItem *first_item = currentItem();
    QStringList str_rows = QApplication::clipboard()->text().split("\n");
    const int RES_COUNT_ROWS = first_item->row() + str_rows.size();
    if(RES_COUNT_ROWS > rowCount()){
        if(QMessageBox::question(this,
                                 QString("Вставка"),
                                 "Недостаточное количество строк для вставки. Добавить?",
                                 QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::No) return;
        const int FIRST_ADD_ROW = rowCount();
        for(int r=FIRST_ADD_ROW;r<RES_COUNT_ROWS;++r) insert_row(r,get_empty_row());
    }
    for(int row = first_item->row();row<RES_COUNT_ROWS;++row){
        QStringList str_columns = str_rows[row-first_item->row()].split("\t");
        int curr_index = 0;
        for(int column = first_item->column();column<columnCount();++column){
            if(curr_index<str_columns.size()){
                QTableWidgetItem *curr_item = item(row,column);
                curr_item->setText(str_columns[curr_index]);
                curr_index++;
            }
        }
    }
}
