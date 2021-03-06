#include "createdialog.h"
#include "ui_createdialog.h"

CreateDialog::CreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDialog)
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i).protocol() == QAbstractSocket::IPv4Protocol)
            qDebug() << ipAddressesList.at(i).toString();
    }

    QString localHostName = QHostInfo::localHostName();

    qDebug() << "localHostName:" << localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address.toString() != "192.168.79.1" &&
                address.toString() != "192.168.213.1")
        {
            qDebug() <<"IPV4 Address: "<< address.toString();
            ipAddress = address.toString();
        }
    }

    /*
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        // qDebug() << ipAddressesList.at(i).toString();
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    */

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    // =====
    // ipAddress = QHostAddress::LocalHost;

    ui->setupUi(this);
    // ui->lineEdit->setText(QHostAddress(QHostAddress::LocalHost).toString());
    ui->lineEdit->setText(ipAddress);
}

CreateDialog::~CreateDialog()
{
    delete ui;
}

QHostAddress CreateDialog::getHostAddress() const
{
    return QHostAddress(ui->lineEdit->text());
}

void CreateDialog::on_localHostButton_clicked()
{
    ui->lineEdit->setText(QHostAddress(QHostAddress::LocalHost).toString());
}
