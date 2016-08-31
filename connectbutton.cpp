#include "connectbutton.h"
#include "ui_connectbutton.h"

ConnectButton::ConnectButton(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectButton)
{
    ui->setupUi(this);
}

ConnectButton::~ConnectButton()
{
    delete ui;
}
