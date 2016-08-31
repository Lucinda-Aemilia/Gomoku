#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include <QHostAddress>


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

private:
    Ui::CreateDialog *ui;
};

#endif // CREATEDIALOG_H
