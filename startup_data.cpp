#include <QGuiApplication>
#include <QScreen>
#include <QCryptographicHash>

#include "startup_data.h"

int startup_data::deans_indexOf(const QString &dean){
    for(int i=0;i<deans.size();++i)
        if((deans[i][0]==dean)||(deans[i][1]==dean))
            return i;
    return -1;
}

QStringList startup_data::get_gruops_by_form(int kurs,
                                                const QString &dean,
                                                const QString &spec,
                                                const QString &parent,
                                                const QString &form){
    QStringList sl;
    for(auto gr:groups){
        if((kurs==gr[1].toInt())&&(dean==gr[5])&&
            (spec==gr[2])&&(parent==gr[4])&&(form==gr[3]))
            sl.append(gr[0]);
    }
    return sl;
}

QStringList startup_data::get_gruops_by_name(const QString &name){
    QStringList sl = QStringList();
    for(auto gr:groups)
        if(gr[0]==name) sl = gr;
    return sl;
}

QStringList startup_data::get_gruops_by_dean(const QString &dean){
    QStringList sl;
    for(auto gr:groups)
        if(dean==gr[5])
            sl.append(gr[0]);
    return sl;
}

void startup_data::xml_curr_data_save(){
    QMap<QString,QString> par;
    par.insert("institut_name",i_names[0]);
    par.insert("institut_cut_name",i_names[1]);
    par.insert("institut_litle_name",i_names[2]);
    par.insert("curr_year",xrw.curr_year);
    for(int i=0;i<forms.size();++i)
        par.insert(QString("form_%1").arg(i),forms[i]);
    for(int i=0;i<budget_contract.size();++i)
        par.insert(QString("budget_contract_%1").arg(i),budget_contract[i]);
    xrw.xml_data_save(par,&prog,&deans);
}


startup_data::startup_data(QObject *parent):
    QObject(parent), i_names(QStringList()<<""<<""<<""){
    QMap<QString,QString> m_d = xrw.xml_data_read(&forms,&budget_contract,&prog,&deans);
    pix_DPR = QPixmap(":/images/coat_of_arms_DPR.png");
    i_names[0] = m_d["institut_name"];
    i_names[1] = m_d["institut_cut_name"];
    i_names[2] = m_d["institut_litle_name"];
    xrw.curr_year = m_d["curr_year"];
    users = xrw.xml_users_read();    
    Password_Dialog *p_d = new Password_Dialog(users);
    if(p_d->exec()){
        for(auto d_u:users)
            if(d_u[0].contains(p_d->comboBox_name->currentText()))
                xrw.curr_user = d_u;
        QString blah = QString(QCryptographicHash::hash(
                                   p_d->lineEdit_password->text().toUtf8(),
                                   QCryptographicHash::Md5).toHex());
        //пароль не совпадает
        if(blah!=xrw.curr_user[2]) qFatal("Неверный пароль!");
    } else qFatal("Нажата кнопка отмены при вводе пароля!");

    xrw.groups_load_from_xml(xrw.curr_user,&groups);
    xrw.xml_specialties_read(&specialty,&specialization);
    students = xrw.load_all_students_from_dirs(QString("dean_data/%1/").arg(xrw.curr_year),&groups);
    deleted_students_from_dir = xrw.load_students_group(QString("dean_data/%1/$deleted_students_%2/list_of_students.xml").arg(xrw.curr_year).arg(xrw.curr_user[1]));

    xrw.students_with_key.clear();
    QMapIterator<QString,QVector<QStringList>> i(students);
    while (i.hasNext()) {
        i.next();
        for(auto st:i.value())
            if(!xrw.students_with_key.contains(st[1]))
                xrw.students_with_key.insert(st[1],st[0]);
            else
                qFatal("Duplicated key=%s for name %s (group - %s)",
                       st[1].toUtf8().constData(),
                       st[0].toUtf8().constData(),
                       i.key().toUtf8().constData());
    }
}

Password_Dialog::Password_Dialog(QVector<QStringList> names,
                                 QWidget *parent) : R_Dialog(parent){
    setObjectName(QString::fromUtf8("Password_Dialog"));
    resize(199, 181);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(20, 140, 161, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    label_name = new QLabel(this);
    label_name->setObjectName(QString::fromUtf8("label_name"));
    label_name->setGeometry(QRect(10, 10, 111, 16));
    label_name->setText(QString("Имя пользователя"));
    label_password = new QLabel(this);
    label_password->setObjectName(QString::fromUtf8("label_password"));
    label_password->setGeometry(QRect(10, 70, 47, 13));
    label_password->setText(QString("Пароль"));
    lineEdit_password = new QLineEdit(this);
    lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
    lineEdit_password->setGeometry(QRect(10, 90, 181, 20));
    lineEdit_password->setText(QString(""));
    lineEdit_password->setEchoMode(QLineEdit::Password);
    lineEdit_password->setFocus();
    comboBox_name = new QComboBox(this);
    comboBox_name->setObjectName(QString::fromUtf8("comboBox_name"));
    comboBox_name->setGeometry(QRect(10, 30, 181, 22));
    for(auto n:names) comboBox_name->addItem(n[0]);
    setWindowTitle(QCoreApplication::translate("Edit_New_Value_Dialog", "Вход в программу", nullptr));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QMetaObject::connectSlotsByName(this);
};

Change_Password_Dialog::Change_Password_Dialog(QStringList user_name,
                                               QWidget *parent) : R_Dialog(parent){
    setObjectName(QString::fromUtf8("Change_Password_Dialog"));
    resize(199, 266);
    setWindowTitle(QCoreApplication::translate("UserPasswordDialog", "Смена пароля", nullptr));

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(20, 220, 161, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    label_name = new QLabel(this);
    label_name->setText("Имя пользователя:");
    label_name->setGeometry(QRect(10, 10, 101, 16));
    label_user_name = new QLabel(this);
    label_user_name->setText(user_name[0]);
    label_user_name->setGeometry(QRect(10, 30, 181, 16));
    label_old_password = new QLabel(this);
    label_old_password->setText("Старый пароль");
    label_old_password->setGeometry(QRect(10, 60, 91, 16));
    old_password = new QLineEdit(this);
    old_password->setGeometry(QRect(10, 80, 181, 20));
    old_password->setEchoMode(QLineEdit::Password);
    old_password->setFocus();
    new_password_first = new QLineEdit(this);
    new_password_first->setGeometry(QRect(10, 140, 181, 20));
    new_password_first->setEchoMode(QLineEdit::Password);
    label_new_password_first = new QLabel(this);
    label_new_password_first->setText("Новый пароль");
    label_new_password_first->setGeometry(QRect(10, 120, 91, 16));
    new_password_second = new QLineEdit(this);
    new_password_second->setGeometry(QRect(10, 190, 181, 20));
    new_password_second->setEchoMode(QLineEdit::Password);
    label_new_password_second = new QLabel(this);
    label_new_password_second->setText("Подтвердите пароль");
    label_new_password_second->setGeometry(QRect(10, 170, 121, 16));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
};

