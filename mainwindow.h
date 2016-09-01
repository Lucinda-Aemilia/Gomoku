#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "board.h"
#include "connectdialog.h"
#include "createdialog.h"

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QPixmap>
#include <QMessageBox>
#include <QTimer>
#include <ctime>
#include <cstdlib>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Role { Server, Client, None };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void acceptConnection();
    void recvMessage();
    void addedPiece(const Board::Piece& piece);
    void waitForServerConnection();
    void clearAll();
    void win(QString info = "");
    void lose(QString info = "");

private slots:
    void on_createButton_clicked();

    void on_connectButton_clicked();

    void on_quitButton_clicked();

    void on_remindButton_clicked();

private:
    Ui::MainWindow *ui;
    Role m_role;
    QTcpServer  *m_listenSocket;
    QTcpSocket  *m_readWriteSocket;
    Board::State m_state;
    QHostAddress m_hostAddress;
    QPixmap m_blackPng;
    QPixmap m_whitePng;
};

#endif // MAINWINDOW_H
