#ifndef report_editor_H
#define report_editor_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include "HTMLHighLighter.h"
#include "JSHighLighter.h"
#include "startup_data.h"
#include "print_preview.h"

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QHBoxLayout;
class QVBoxLayout;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;
QT_END_NAMESPACE

class report_editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit report_editor(startup_data *s_data, const QString &r_dir, QWidget *parent = nullptr);

    bool load(const QString &f);

public slots:
    void fileNew();

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void textAdd(); //добавление данных из основной программы
    void run_left();
    void run_right();

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter *);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void edit_connected(QTextEdit *te);

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;
    QAction *actionUndo;
    QAction *actionRedo;
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif
    QAction *actionAdd;
    QAction *actionRunLeft;
    QAction *actionRunRight;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
    QTextEdit *htmlEdit;
    QTextEdit *jsEdit;
    QWidget *centralWidget;
    QWidget *horizontalLayoutWidget;
    QWidget *verticalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    HtmlHighLighter *hl_h;
    JSHighLighter *hl_j;
    startup_data *s_d;
    print_preview *pp;
    const QString report_dir;
};

#endif // report_editor_H
