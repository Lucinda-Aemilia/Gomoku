#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置系统默认字体
    QFont font;
    font.setFamily("Microsoft Yahei UI");
    font.setPointSize(9);
    font.setBold(false);
    a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
