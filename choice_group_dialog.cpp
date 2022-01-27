#include "choice_group_dialog.h"

Choice_Group_Dialog::Choice_Group_Dialog(
        QStringList groups,
        QWidget *parent):R_Dialog(parent){
    setWindowTitle("Выбор группы");
    resize(330, 87);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(160, 50, 161, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    comboBox = new QComboBox(this);
    comboBox->setGeometry(QRect(10, 10, 311, 22));
    for(auto gr:groups) comboBox->addItem(gr);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);

}
