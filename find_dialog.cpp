#include "find_dialog.h"

void Find_Dialog::gr_to_label(const QString &st){
    QString gr_name = "";
    for(auto s:f_students)
        if(s[0]==st) gr_name = s[1];
    label_gr->setText(l_gr+gr_name);
}

void Find_Dialog::textChanged(const QString &text){
    comboBox->clear();
    for(auto s:f_students)
        if(s[0].indexOf(text,0,Qt::CaseInsensitive)==0)
            comboBox->addItem(s[0]);
    gr_to_label(text);
}

void Find_Dialog::activated(int index){
    lineEdit->setText(comboBox->itemText(index));
}

Find_Dialog::Find_Dialog(startup_data *s_data, QWidget *parent)
    :R_Dialog(parent),s_d(s_data){
    setWindowTitle("Поиск студента");
    setObjectName(QString::fromUtf8("Find_Dialog"));
    resize(475, 96);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(120, 50, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    label = new QLabel(this);
    label->setGeometry(QRect(10, 10, 81, 16));
    label->setText(QString("ФИО студента"));
    comboBox = new QComboBox(this);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));
    comboBox->setGeometry(QRect(280, 10, 181, 22));
    QMapIterator<QString,QVector<QStringList>> i(s_d->students);
    while (i.hasNext()) {
        i.next();
        for(auto st:i.value())
            f_students.append(QStringList() << st[0]<< i.key());
    }

    for(auto s:f_students) comboBox->addItem(s[0]);

    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setGeometry(QRect(90, 10, 181, 20));
    lineEdit->setFocus();

    label_gr = new QLabel(this);
    label_gr->setGeometry(QRect(10, 50, 181, 16));

    connect(lineEdit,&QLineEdit::textChanged,this,&Find_Dialog::textChanged);
    connect(comboBox,SIGNAL(activated(int)),this,SLOT(activated(int)));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}
