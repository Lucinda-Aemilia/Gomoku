#include "board.h"
#include "ui_board.h"

Board::Board(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Board)
{
    ui->setupUi(this);
    m_backgroundColor = QColor("tan");
    // memset(m_inputArea, 0, sizeof(m_inputArea));

}

Board::~Board()
{
    delete ui;
}

// 绘制棋盘
void Board::paintEvent(QPaintEvent *event)
{


    // 画格子
    int totWidth = width(), totHeight = height();
    int length = std::min(totWidth, totHeight) - 30;

    m_pieceSize = length / 14.0 * 0.45;
    m_pointSize = std::floor(length / 14.0 * 0.05 + 0.5);
    m_pointSize = m_pointSize == 0 ? 1 : m_pointSize;

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.translate((totWidth - length)/2, (totHeight - length)/2);

    for (int i = 0; i <= 14; i++)
    {
        painter.save();
        painter.translate(0, length / 14.0 * i);
        painter.drawLine(0, 0, length, 0);
        painter.restore();
    }
    for (int i = 0; i <= 14; i++)
    {
        painter.save();
        painter.translate(length / 14.0 * i, 0);
        painter.drawLine(0, 0, 0, length);
        painter.restore();
    }

    // 画点
    painter.setBrush(Qt::black);
    painter.save();
    painter.translate(length / 2, length / 2);
    painter.drawEllipse(QPoint(0, 0), m_pointSize, m_pointSize);
    painter.restore();
    for (int i = 3; i <= 11; i += 8)
        for (int j = 3; j <= 11; j += 8)
        {
            painter.save();
            painter.translate(length / 14.0 * i, length / 14.0 * j);
            painter.drawEllipse(QPoint(0, 0), m_pointSize, m_pointSize);
            painter.restore();
        }

    event->accept();
}
