#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostInfo>


namespace Ui {
class CreateDialog;
}

class CreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDialog(QWidget *parent = 0);
    ~CreateDialog();
    QHostAddress getHostAddress() const;

private slots:
    void on_localHostButton_clicked();

private:
    Ui::CreateDialog *ui;
};

#endif // CREATEDIALOG_H
