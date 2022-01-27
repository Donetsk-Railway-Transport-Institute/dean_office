#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDialog>
#include <QMimeData>
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

#include <QApplication>
#include <QDesktopWidget>

#include "report_editor.h"

const QString rsrcPath = ":/images/win";

void report_editor::edit_connected(QTextEdit *te){
    connect(te->document(), &QTextDocument::modificationChanged,
            actionSave, &QAction::setEnabled);
    connect(te->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(te->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(te->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    setWindowModified(te->document()->isModified());
}

report_editor::report_editor(startup_data *s_data, const QString &r_dir, QWidget *parent)
    : QMainWindow(parent),hl_h(nullptr),hl_j(nullptr),s_d(s_data), report_dir(r_dir){
    setWindowTitle(QCoreApplication::applicationName());
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    resize((availableGeometry.width() *2) / 3, (availableGeometry.height() * 3) / 4);
    move((availableGeometry.width() - width()) / 2,(availableGeometry.height() - height()) / 2);

    pp = new print_preview(s_d,this);

    horizontalLayoutWidget = new QWidget(this);
    horizontalLayoutWidget->setGeometry(QRect(0,90,geometry().width(),geometry().height()-110));
    horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout = new QVBoxLayout();
    htmlEdit = new QTextEdit(horizontalLayoutWidget);
    htmlEdit->setObjectName("html");
    verticalLayout->addWidget(htmlEdit);
    jsEdit = new QTextEdit(horizontalLayoutWidget);
    jsEdit->setObjectName("js");
    verticalLayout->addWidget(jsEdit);
    horizontalLayout->addLayout(verticalLayout);
    textEdit = new QTextEdit(horizontalLayoutWidget);
    textEdit->setObjectName("text");
    horizontalLayout->addWidget(textEdit);

    connect(textEdit, &QTextEdit::currentCharFormatChanged,
            this, &report_editor::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &report_editor::cursorPositionChanged);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();

    QMenu *helpMenu = menuBar()->addMenu(tr("Помощь"));
    helpMenu->addAction(tr("О программе"), this, &report_editor::about);

    QFont textFont("Times New Roman");
    textFont.setStyleHint(QFont::Times);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());
    textSize("14");

    edit_connected(textEdit);
    edit_connected(htmlEdit);
    edit_connected(jsEdit);

    actionSave->setEnabled(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());
    actionCut->setEnabled(false);

    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &report_editor::clipboardDataChanged);

    textEdit->setFocus();
    setCurrentFileName(QString());
}

void report_editor::closeEvent(QCloseEvent *e)
{    
    if (maybeSave()) e->accept();
    else e->ignore();
}

void report_editor::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Файл"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
    QAction *a = menu->addAction(newIcon,  tr("&Новый"), this, &report_editor::fileNew);
    tb->addAction(a);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    a = menu->addAction(openIcon, tr("&Открыть..."), this, &report_editor::fileOpen);
    a->setShortcut(QKeySequence::Open);
    tb->addAction(a);

    menu->addSeparator();

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    actionSave = menu->addAction(saveIcon, tr("&Сохранить"), this, &report_editor::fileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    tb->addAction(actionSave);

    a = menu->addAction(tr("Сохранить &как..."), this, &report_editor::fileSaveAs);
    a->setPriority(QAction::LowPriority);
    menu->addSeparator();

#ifndef QT_NO_PRINTER
    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    a = menu->addAction(printIcon, tr("&Печать..."), this, &report_editor::filePrint);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Print);
    tb->addAction(a);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    menu->addAction(filePrintIcon, tr("Предварительный просмотр..."), this, &report_editor::filePrintPreview);

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Экспорт PDF..."), this, &report_editor::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    tb->addAction(a);

    menu->addSeparator();
#endif

    a = menu->addAction(tr("&Выход"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

void report_editor::setupEditActions()
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Редактирование"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Отменить"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Повторить"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("&Вырезать"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Копировать"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Вставить"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());

    const QIcon addIcon = QIcon::fromTheme("edit-add", QIcon(rsrcPath + "/icon-add.png"));
    actionAdd = menu->addAction(addIcon, tr("&Добавить"), this, &report_editor::textAdd);
    actionAdd->setPriority(QAction::LowPriority);
    actionAdd->setShortcut(QKeySequence::AddTab);
    tb->addAction(actionAdd);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionAdd->setEnabled(md->hasText());

    const QIcon runleftIcon = QIcon::fromTheme("runleft", QIcon(rsrcPath + "/runleft.png"));
    actionRunLeft = menu->addAction(runleftIcon, tr("&Обновить HTML"), this, &report_editor::run_left);
    actionRunLeft->setPriority(QAction::LowPriority);
    tb->addAction(actionRunLeft);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionRunLeft->setEnabled(md->hasText());

    const QIcon runrightIcon = QIcon::fromTheme("runright", QIcon(rsrcPath + "/runright.png"));
    actionRunRight = menu->addAction(runrightIcon, tr("&Обновить текст"), this, &report_editor::run_right);
    actionRunRight->setPriority(QAction::LowPriority);
    tb->addAction(actionRunRight);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionRunRight->setEnabled(md->hasText());
}

void report_editor::textAdd(){    
    QTextEdit *tmp_Edit = nullptr;
    QWidget *widgetName = qApp->focusWidget();
    if(widgetName->objectName()=="text") tmp_Edit = textEdit;
    else if(widgetName->objectName()=="html") tmp_Edit = htmlEdit;
    else return;

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Добавление вызова js скрипта");
    dlg->resize(400, 300);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(dlg);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(30, 250, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QTextEdit *dlg_textEdit = new QTextEdit(dlg);
    dlg_textEdit->setGeometry(QRect(10, 10, 381, 221));
    dlg_textEdit->setFocus();
    dlg_textEdit->setPlainText("<script></script>");

    QTextCursor tmpCursor = dlg_textEdit->textCursor();
    tmpCursor.setPosition(8);
    dlg_textEdit->setTextCursor(tmpCursor);

    QObject::connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));
    QMetaObject::connectSlotsByName(dlg);
    if(dlg->exec()){
        tmpCursor = tmp_Edit->textCursor();
        tmpCursor.setPosition(tmp_Edit->textCursor().position());
        tmp_Edit->setTextCursor(tmpCursor);
        if(tmp_Edit->objectName()=="html")
            tmp_Edit->insertPlainText(dlg_textEdit->toPlainText());
        else {
            QString rnd = xml_read_write::randx();
            tmp_Edit->insertPlainText(rnd);
            run_left();
            QString tmp = htmlEdit->document()->toPlainText();
            QRegularExpression re("<p style=(.*?)</p>");
            QRegularExpressionMatchIterator matchIterator = re.globalMatch(tmp);
            while (matchIterator.hasNext()){
                QRegularExpressionMatch match = matchIterator.next();
                int pos = match.capturedTexts()[0].indexOf(rnd);
//                qInfo("%d %s",pos,match.capturedTexts()[0].toUtf8().constData());
                if(pos>0)
                    tmp.replace(match.capturedTexts()[0],dlg_textEdit->toPlainText());
            }
            htmlEdit->setPlainText(tmp);
            run_right();
        }
        //qInfo("%s pos=%d",tmp_Edit->objectName().toUtf8().constData(),tmp_Edit->textCursor().position());
        //qInfo("%d %s",dlg_textEdit->textCursor().position(), dlg_textEdit->toPlainText().toUtf8().constData());
    }
}

void report_editor::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Форматирование"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Полужирный"), this, &report_editor::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Курсив"), this, &report_editor::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Подчеркивание"), this, &report_editor::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&По левому краю"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("&По центру"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&По правому краю"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&По ширине"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &report_editor::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Цвет..."), this, &report_editor::textColor);
    tb->addAction(actionTextColor);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &report_editor::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &report_editor::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &report_editor::textSize);
}

void report_editor::run_left(){
    QString tmp = pp->replace_plain_on_html(textEdit->document()->toHtml());
    htmlEdit->setPlainText(tmp);
    textEdit->document()->setModified(true);
    if(hl_h) delete hl_h;
    hl_h = new HtmlHighLighter(htmlEdit->document());
}

void report_editor::run_right(){
    pp->prepare_data( {textEdit->document()->toPlainText(),
                       htmlEdit->document()->toPlainText(),
                       jsEdit->document()->toPlainText()} );
    textEdit->setHtml(pp->replace_html_on_plain(htmlEdit->document()->toPlainText()));
    textEdit->document()->setModified(true);
}

bool report_editor::load(const QString &f) {
    print_preview::HTML_PLAIN_TEXTS t = pp->get_html_with_js(f);
    if(t.html.isEmpty()) return false;
    if(t.plain.isEmpty()) return false;
    htmlEdit->setPlainText(t.plain);
    textEdit->setHtml(t.html);
    jsEdit->setPlainText(t.js);

    if(hl_h) delete hl_h;
    hl_h = new HtmlHighLighter(htmlEdit->document());
    if(hl_j) delete hl_j;
    hl_j = new JSHighLighter(jsEdit->document());

//    qInfo("%s",textEdit->document()->toHtml().toUtf8().constData());
    setCurrentFileName(f);
    return true;
}

bool report_editor::maybeSave()
{
//    qInfo("textEdit->document()->isModified() %s",textEdit->document()->isModified()?"true":"false");
//    qInfo("htmlEdit->document()->isModified() %s",htmlEdit->document()->isModified()?"true":"false");
//    qInfo("jsEdit->document()->isModified() %s",jsEdit->document()->isModified()?"true":"false");
    if ((!textEdit->document()->isModified()) &&
       (!htmlEdit->document()->isModified()) &&
       (!jsEdit->document()->isModified())) return true;

    const QMessageBox::StandardButton ret =
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             tr("Документ был изменен.\n"
                                "Вы хотите сохранить изменения?"),
                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save) return fileSave();
    else if (ret == QMessageBox::Cancel) return false;
    return true;
}

void report_editor::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void report_editor::fileNew()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void report_editor::fileOpen()
{
    QFileDialog fileDialog(this, tr("Открыть файл..."), report_dir);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList() << "text/html" << "text/plain");
    if (fileDialog.exec() != QDialog::Accepted) return;
    const QString fn = fileDialog.selectedFiles().first();
    if (load(fn))
        statusBar()->showMessage(tr("Открытие \"%1\"").arg(QDir::toNativeSeparators(fn)));
    else
        statusBar()->showMessage(tr("Невозможно открыть \"%1\"").arg(QDir::toNativeSeparators(fn)));
}

bool report_editor::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",fileName.toUtf8().constData());
    QByteArray data(htmlEdit->document()->toPlainText().toUtf8());
    file.write(data);
    file.close();
    htmlEdit->document()->setModified(false);

    QString js_name = fileName.split(".html")[0]+".js";
    QFile js_file(js_name);
    if (!js_file.open(QFile::WriteOnly))
        qFatal("Ошибка открытия файла %s для записи",js_name.toUtf8().constData());
    QByteArray js_data(jsEdit->document()->toPlainText().toUtf8());
    js_file.write(js_data);
    js_file.close();
    textEdit->document()->setModified(false);
    jsEdit->document()->setModified(false);
    return true;
}

bool report_editor::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Сохранить как..."), report_dir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
//    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    mimeTypes << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
//    fileDialog.setDefaultSuffix("odt");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}

void report_editor::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Печать документа"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
}

void report_editor::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &report_editor::printPreview);
    preview.exec();
}

void report_editor::printPreview(QPrinter *printer)
{
    textEdit->print(printer);
}


void report_editor::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"), report_dir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif
}

void report_editor::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void report_editor::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void report_editor::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void report_editor::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void report_editor::textSize(const QString &p)
{
    qreal pointSize = static_cast<qreal>(p.toFloat());
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void report_editor::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

void report_editor::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void report_editor::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void report_editor::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void report_editor::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
        case QTextListFormat::ListDisc:
            comboStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            comboStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            comboStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            comboStyle->setCurrentIndex(4);
            break;
        case QTextListFormat::ListLowerAlpha:
            comboStyle->setCurrentIndex(5);
            break;
        case QTextListFormat::ListUpperAlpha:
            comboStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerRoman:
            comboStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperRoman:
            comboStyle->setCurrentIndex(8);
            break;
        default:
            comboStyle->setCurrentIndex(-1);
            break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);
    }
}

void report_editor::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void report_editor::about()
{
    QMessageBox::about(this, tr("О программе"), tr("Программа "
        "функционирует в составе программного комплекса DEAN_OFFICE "
        "и предназначена для формирования шаблонов для печати отчетов."));
}

void report_editor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void report_editor::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void report_editor::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void report_editor::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

