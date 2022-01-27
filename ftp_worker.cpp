#include "ftp_worker.h"
#include "ui_ftp_worker.h"
#include "ftp_archiving_dlg.h"
#include <QFileDialog>
#include <QTextCodec>
#include <QTextDecoder>
#include <QClipboard>
#include <QRegExp>
#include <QXmlStreamWriter>

void ftp_worker::on_put_data_triggered(){
    ftp_archiving_dlg *fad = new ftp_archiving_dlg(s_d,this);
    fad->exec();
}

void ftp_worker::on_download_triggered(){
}

ftp_worker::ftp_worker(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::ftp_worker), s_d(new startup_data(this)){
    ui->setupUi(this);    
}

ftp_worker::~ftp_worker()
{
    delete ui;
}
