#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <createdialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_role = None;
    m_state = Board::Undefined;
    m_listenSocket = NULL;
    m_readWriteSocket = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    qDebug() << "create button" << m_state << Board::Undefined;
    if (m_state == Board::Undefined)
    {
        // 显示主机ip
        CreateDialog dialog(this);
        int ret = dialog.exec();
        if (ret == QDialog::Rejected)
            return;
        m_state = Board::WaitForConnect;
        m_role = Server;
        ui->roleLabel->setText("Role: Server");
        ui->statusLabel->setText("Status: Waiting for connection");

        if (!m_listenSocket)
            this->m_listenSocket = new QTcpServer;

        this->m_listenSocket->listen(QHostAddress::Any, 8888);
        // this->listenSocket->listen(QHostAddress("192.168.2.106"), 8888);
        qDebug() << m_listenSocket->isListening();
        QObject::connect(this->m_listenSocket, SIGNAL(newConnection()),
                         this, SLOT(acceptConnection()));

        // 显示正在连接，可取消
        QMessageBox* messageBox = new QMessageBox(this);
        messageBox->addButton(QMessageBox::Cancel);
        messageBox->setWindowTitle("Waiting...");
        messageBox->setText("Waiting for new connection...");
        qDebug() << "...";
        connect(this->m_listenSocket, SIGNAL(newConnection()),
                messageBox, SLOT(close()));
        qDebug() << "...";
        ret = messageBox->exec();
        if (ret == QMessageBox::Cancel && m_state == Board::WaitForConnect)
        {
            qDebug() << "new connection aborted";
            m_listenSocket->close();
            m_state = Board::Undefined;
            m_role = None;
            ui->roleLabel->setText("Role: Undefined");
            ui->statusLabel->setText("Status: Undefined");
        }
    }
}

void MainWindow::acceptConnection()
{
    qDebug() << "MainWindow::acceptConnection()";
    m_state = Board::Pend;
    // m_role = Server;
    // ui->roleLabel->setText("Role: Server");
    // ui->statusLabel->setText("Status: Waiting for connection");
    if (m_role == Server)
    {
        qDebug() << "accept connection from client";
        ui->statusLabel->setText("Status: Received connection from client");
        this->m_readWriteSocket = this->m_listenSocket->nextPendingConnection();
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead())
                         , this, SLOT(recvMessage()));
    }
    else if (m_role == Client)
    {
        // m_state = Board::Pend;
        qDebug() << "accept connection from server";
        ui->statusLabel->setText("Status: Received connection from server");
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead())
                         , this, SLOT(recvMessage()));
    }

}
void MainWindow::recvMessage()
{
    qDebug() << "received message";
    QString info;
    info += this->m_readWriteSocket->readAll();
    qDebug() << info;
}

void MainWindow::on_connectButton_clicked()
{
    qDebug() << "connect button" << m_state << Board::Undefined;
    if (m_state == Board::Undefined)
    {
        ConnectDialog dialog(this);
        int ret = dialog.exec();
        if (ret == QDialog::Rejected)
            return;
        m_hostAddress = dialog.getHostAddress();
        m_state = Board::WaitForConnect;
        m_role = Client;
        ui->roleLabel->setText("Role: Client");
        ui->statusLabel->setText("Status: Waiting for connection");

        if (!m_readWriteSocket)
            this->m_readWriteSocket = new QTcpSocket;

        // this->m_readWriteSocket->connectToHost(hostAddress, 8888);
        qDebug() << "waiting for host connection";
        /*
        qDebug() << this->m_readWriteSocket->waitForConnected(-1);
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead()),
                         this, SLOT(recvMessage()));
        */
        // 显示正在连接，可取消
        QMessageBox* messageBox = new QMessageBox(this);
        messageBox->addButton(QMessageBox::Cancel);
        messageBox->setWindowTitle("Waiting...");
        messageBox->setText("Waiting for the server...");
        // qDebug() << "...";
        connect(this->m_readWriteSocket, SIGNAL(connected()),
                messageBox, SLOT(close()));
        connect(this->m_readWriteSocket, SIGNAL(connected()),
                this, SLOT(acceptConnection()));
        // qDebug() << "...";
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(waitForServerConnection()));
        timer->start(1000);
        this->m_readWriteSocket->connectToHost(m_hostAddress, 8888);
        waitForServerConnection();

        ret = messageBox->exec();
        if (ret == QMessageBox::Cancel && m_state == Board::WaitForConnect)
        {
            qDebug() << "new connection aborted";
            m_readWriteSocket->close();
            m_state = Board::Undefined;
            m_role = None;
            // timer->stop();
            ui->roleLabel->setText("Role: Undefined");
            ui->statusLabel->setText("Status: Undefined");
        }
        timer->stop();
    }
}

void MainWindow::waitForServerConnection()
{
    qDebug() << "trying to connect to server...";
    this->m_readWriteSocket->connectToHost(m_hostAddress, 8888);;
}

/*
void clientWindow::connectHost()
{
    this->readWriteSocket = new QTcpSocket;
    // this->readWriteSocket->connectToHost(QHostAddress("192.168.2.106"),8888);
    this->readWriteSocket->connectToHost(QHostAddress::LocalHost, 8888);
    qDebug() << this->readWriteSocket->state();
    QObject::connect(this->readWriteSocket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
}
*/
