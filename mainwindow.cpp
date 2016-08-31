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
    ui->board->init(Board::Undefined);
    connect(ui->board, SIGNAL(inputFinished(Board::Piece)),
            this, SLOT(recvOtherPiece(Board::Piece)));
    m_blackPng.load(":/icon/black.png");
    m_whitePng.load(":/icon/white.png");
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
        ui->roleTextEdit->setPlainText("Role: Server");
        ui->statusTextEdit->setPlainText("Status: Waiting for connection");

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
        // qDebug() << "...";
        connect(this->m_listenSocket, SIGNAL(newConnection()),
                messageBox, SLOT(close()));
        // qDebug() << "...";
        ret = messageBox->exec();
        if (ret == QMessageBox::Cancel && m_state == Board::WaitForConnect)
        {
            qDebug() << "new connection aborted";
            m_listenSocket->close();
            m_state = Board::Undefined;
            m_role = None;
            ui->roleTextEdit->setPlainText("Role: Undefined");
            ui->statusTextEdit->setPlainText("Status: Undefined");
        }

        // 连接完后，开始决定谁是先手，谁是后手，开始下棋
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
        ui->statusTextEdit->setPlainText("Status: Received connection from client");
        this->m_readWriteSocket = this->m_listenSocket->nextPendingConnection();
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead())
                         , this, SLOT(recvMessage()));

        // random黑色或白色
        std::srand(std::time(0));
        int x = std::rand() % 2;
        if (x == 0) // server 执黑棋
        {
            ui->board->init(Board::Run, Qt::black);
            ui->meColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size()));
            ui->otherColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size()));
            ui->board->waitForInput();
        }
        else // server 执白棋
        {
            ui->board->init(Board::Pend, Qt::white);
            ui->meColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size()));
            ui->otherColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size()));
            QString text("COLOR(BLACK)");
            m_readWriteSocket->write(text.toLocal8Bit());
        }
    }
    else if (m_role == Client)
    {
        // m_state = Board::Pend;
        qDebug() << "accept connection from server";
        ui->statusTextEdit->setPlainText("Status: Received connection from server");
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead())
                         , this, SLOT(recvMessage()));
    }

}

void MainWindow::recvOtherPiece(const Board::Piece &piece)
{
    ui->board->addOtherPiece(piece.row(), piece.column());
}

void MainWindow::recvMessage()
{
    qDebug() << "received message";
    QString info(this->m_readWriteSocket->readAll());
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
        ui->roleTextEdit->setPlainText("Role: Client");
        ui->statusTextEdit->setPlainText("Status: Waiting for connection");

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
            ui->roleTextEdit->setPlainText("Role: Undefined");
            ui->statusTextEdit->setPlainText("Status: Undefined");
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
