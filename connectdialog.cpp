#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    QSignalMapper* map = new QSignalMapper(this);
    connect(ui->button0, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button0, "0");
    connect(ui->button1, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button1, "1");
    connect(ui->button2, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button2, "2");
    connect(ui->button3, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button3, "3");
    connect(ui->button4, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button4, "4");
    connect(ui->button5, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button5, "5");
    connect(ui->button6, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button6, "6");
    connect(ui->button7, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button7, "7");
    connect(ui->button8, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button8, "8");
    connect(ui->button9, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->button9, "9");
    connect(ui->buttonDot, SIGNAL(clicked(bool)), map, SLOT(map()));
    map->setMapping(ui->buttonDot, ".");
    connect(map, SIGNAL(mapped(QString)), this, SLOT(appendToAddress(QString)));
}

void ConnectDialog::appendToAddress(const QString &c)
{
    ui->lineEdit->setText(ui->lineEdit->text().append(c));
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

QHostAddress ConnectDialog::getHostAddress() const
{
    return QHostAddress(ui->lineEdit->text());
}

void ConnectDialog::closeEvent(QCloseEvent *event)
{
    /*
    // QHostAddress address(ui->lineEdit->text());
    QRegExp rx2("^([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])/.\
([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])$");
    qDebug() << "closeEvent" << ui->lineEdit->text();
    if (!rx2.exactMatch(ui->lineEdit->text()))
    {
        event->ignore();
        QMessageBox::warning(this, tr("Error"), tr("Wrong IP address."));
        return;
    }
    else
    */
    QDialog::closeEvent(event);
}

void ConnectDialog::accept()
{
    QHostAddress address(ui->lineEdit->text());
    /*
    QRegExp rx2("^([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])/.\
([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])$");
*/
    qDebug() << "accept" << ui->lineEdit->text();
    qDebug() << address.toString();
    if (address.toString() != ui->lineEdit->text())
    {
        QMessageBox::warning(this, tr("Error"), tr("Wrong IP address."));
        return;
    }
    else
    QDialog::accept();
}

void ConnectDialog::on_buttonDelete_clicked()
{
    if (!ui->lineEdit->text().isEmpty())
    {
        QString text(ui->lineEdit->text());
        text.remove(text.length() - 1, 1);
        ui->lineEdit->setText(text);
    }
}
