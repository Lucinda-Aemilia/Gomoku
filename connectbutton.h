#ifndef CONNECTBUTTON_H
#define CONNECTBUTTON_H

#include <QDialog>

namespace Ui {
class ConnectButton;
}

class ConnectButton : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectButton(QWidget *parent = 0);
    ~ConnectButton();

private:
    Ui::ConnectButton *ui;
};

#endif // CONNECTBUTTON_H
