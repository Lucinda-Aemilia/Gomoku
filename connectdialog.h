#ifndef CONNECTBUTTON_H
#define CONNECTBUTTON_H

#include <QDialog>
#include <QHostAddress>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSignalMapper>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    QHostAddress getHostAddress() const;
    ~ConnectDialog();

public slots:
    void appendToAddress(const QString& c);
    void accept();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void on_buttonDelete_clicked();

    void on_localHostButton_clicked();

private:
    Ui::ConnectDialog *ui;
};

#endif // CONNECTBUTTON_H
