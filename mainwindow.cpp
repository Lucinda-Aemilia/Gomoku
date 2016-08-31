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
    m_listenSocket = new QTcpServer;
    m_readWriteSocket = new QTcpSocket;
    ui->board->init(Board::Undefined);
    connect(ui->board, SIGNAL(inputFinished(Board::Piece)),
            this, SLOT(addedPiece(Board::Piece)));
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
        if (!m_readWriteSocket)
            this->m_readWriteSocket = new QTcpSocket;

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
    ui->board->init(Board::Pend);
    // m_role = Server;
    // ui->roleLabel->setText("Role: Server");
    // ui->statusLabel->setText("Status: Waiting for connection");
    if (m_role == Server)
    {
        qDebug() << "accept connection from client";
        ui->statusTextEdit->setPlainText("Status: Received connection from client");
        this->m_readWriteSocket = this->m_listenSocket->nextPendingConnection();
        qDebug() << m_readWriteSocket->isOpen();
        QObject::connect(this->m_readWriteSocket, SIGNAL(readyRead())
                         , this, SLOT(recvMessage()));


        // random黑色或白色
        std::srand(std::time(0));
        int x = std::rand() % 2;
        if (x == 0) // server 执黑棋
        {
            ui->board->init(Board::Run, Qt::black);
            ui->meColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size(),
                                                          Qt::KeepAspectRatio));
            ui->otherColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size(),
                                                             Qt::KeepAspectRatio));
            QString text("COLOR WHITE");
            m_readWriteSocket->write(text.toLocal8Bit());
            ui->board->waitForInput();
        }
        else // server 执白棋
        {
            ui->board->init(Board::Pend, Qt::white);
            ui->meColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size(),
                                                          Qt::KeepAspectRatio));
            ui->otherColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size(),
                                                             Qt::KeepAspectRatio));
            QString text("COLOR BLACK");
            m_readWriteSocket->write(text.toLocal8Bit());
        }
        /*
        ui->board->init(Board::Run, Qt::black);
        ui->meColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size()));
        ui->otherColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size()));
        QString text("COLOR(WHITE)");
        m_readWriteSocket->write(text.toLocal8Bit());
        qDebug() << "write to client" << text;
        ui->board->waitForInput();
        */
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

void MainWindow::addedPiece(const Board::Piece &piece)
{
    ui->board->addOtherPiece(piece.row(), piece.column());
    QString text(QString("ADD %1 %2").arg(piece.row()).arg(piece.column()));
    bool win = ui->board->checkState();

    if (win)
    {
        text += QString(";") +QString("WIN");
        qDebug() << "addpiece" << text;
        m_readWriteSocket->write(text.toLocal8Bit());
        this->win();
    }
    else
        m_readWriteSocket->write(text.toLocal8Bit());
}

void MainWindow::recvMessage()
{

    QString info(this->m_readWriteSocket->readAll());
    qDebug() << "received message" << info;
    QStringList list(info.split(';'));
    for (int i = 0; i < list.size(); i++)
    {
        QStringList command(list.at(i).split(" "));
        if (list.at(i).contains("COLOR"))
        {

            if (command.at(1) == "BLACK")
            {
                ui->meColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size(),
                                                              Qt::KeepAspectRatio));
                ui->otherColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size(),
                                                                 Qt::KeepAspectRatio));
                ui->board->init(Board::Run, Qt::black);
                ui->board->waitForInput();
                bool win = ui->board->checkState();
                if (win)
                {
                    m_readWriteSocket->write(QString("WIN").toLocal8Bit());
                    this->win();
                }

            }
            else
            {
                ui->meColorLabel->setPixmap(m_whitePng.scaled(ui->meColorLabel->size(),
                                                              Qt::KeepAspectRatio));
                ui->otherColorLabel->setPixmap(m_blackPng.scaled(ui->meColorLabel->size(),
                                                                 Qt::KeepAspectRatio));
                ui->board->init(Board::Pend, Qt::white);
            }
        }
        else if (list.at(i).contains("ADD"))
        {
            int r = command.at(1).toInt(), c = command.at(2).toInt();
            ui->board->addOtherPiece(r, c);
            ui->board->waitForInput();
            qDebug() << ui->board->getColor() << "After waiting for input";
            bool win = ui->board->checkState();
            qDebug() << win;
            if (win)
            {
                m_readWriteSocket->write(QString("WIN").toLocal8Bit());
                this->win();
            }
        }
        else if (list.at(i).contains("QUIT"))
        {
            win("Your opponent quit.");
        }
        else if (list.at(i).contains("WIN"))
        {
            qDebug() << "The opponent wins";
            lose();
        }
    }
    qDebug() << info;
}

void MainWindow::win(QString info)
{
    // clearAll();
    ui->board->setState(Board::Win);
    m_state = Board::Undefined;
    if (m_listenSocket)
    {
        m_listenSocket->close();
        m_listenSocket->deleteLater();
        m_listenSocket = NULL;
    }
    if (m_readWriteSocket)
    {
        m_readWriteSocket->close();
        m_readWriteSocket->deleteLater();
        m_readWriteSocket = NULL;
    }
    if (info != "")
        info.append(QString("\n") + QString("You win."));
    else
        info = "You win.";
    QMessageBox::information(this, "Contratulations!", info);
}

void MainWindow::lose(QString info)
{
    // clearAll();
    m_readWriteSocket->write(QString("QUIT").toLocal8Bit());
    ui->board->setState(Board::Lost);
    m_state = Board::Undefined;
    if (m_listenSocket)
    {
        m_listenSocket->close();
        m_listenSocket->deleteLater();
        m_listenSocket = NULL;
    }
    if (m_readWriteSocket)
    {
        m_readWriteSocket->close();
        m_readWriteSocket->deleteLater();
        m_readWriteSocket = NULL;
    }
    if (info != "")
        info.append(QString("\n") + QString("You lose."));
    else
        info = "You lose.";
    QMessageBox::information(this, "Sorry...", info);
}

void MainWindow::clearAll()
{

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
        if (!m_listenSocket)
            this->m_listenSocket = new QTcpServer;

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

void MainWindow::on_quitButton_clicked()
{
    if (m_state == Board::Undefined)
        return;
    int ret = QMessageBox::question(this, "Quit",
                          "Do you really want to quit?\nYou will lose.",
                                    QMessageBox::Yes, QMessageBox::No);
    if (ret == QMessageBox::No)
        return;

    lose("You quit.");
}
