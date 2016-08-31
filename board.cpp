#include "board.h"
#include "ui_board.h"

Board::Board(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Board)
{
    ui->setupUi(this);
    m_backgroundColor = QColor("tan");
    m_margin = 60;
    // memset(m_inputArea, 0, sizeof(m_inputArea));

    // 测试部分
    init(Run, Qt::white);
}

void Board::init(const State &state, const QColor &pieceColor)
{
    m_state = state;
    m_pieceColor = pieceColor;
    m_otherPieceColor = pieceColor == Qt::white ? Qt::black : Qt::white;
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            m_board[i][j] = None;
}

Board::~Board()
{
    delete ui;
}

void Board::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_state == Run)
    {
        int x = event->pos().x(), y = event->pos().y();
        qDebug() << x << y;
        int r = toIndex(x - (width()-m_length)/2);
        int c = toIndex(y - (height()-m_length)/2);
        if (m_board[r][c] != PieceType::None)
            return;
        qDebug() << r << c;
        Piece piece(r, c, MyPiece);
        addPiece(piece);
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void Board::addPiece(const Piece &piece)
{
    if (piece.row() < 0 || piece.column() < 0)
        return;
    for (int i = 0; i < m_pieces.size(); i++)
    {
        if (piece.row() == m_pieces.at(i).row() && piece.column() == m_pieces.at(i).column())
            return;
    }
    m_board[piece.row()][piece.column()] = PieceType::MyPiece;
    m_pieces.append(piece);
    m_state = Pend;
    emit inputFinished(piece);
    update();
}

int Board::toIndex(int pos)
{
    return std::floor(pos / (m_length/14.0) + 0.5);
}

// 绘制棋盘
void Board::paintEvent(QPaintEvent *event)
{

    // 画格子
    int totWidth = width(), totHeight = height();
    int length = std::min(totWidth, totHeight) - m_margin;
    m_length = length;

    m_pieceSize = length / 14.0 * 0.45;
    m_pointSize = std::floor(length / 14.0 * 0.05 + 0.5);
    m_pointSize = m_pointSize == 0 ? 1 : m_pointSize;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
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

    // 画棋子
    for (int i = 0; i < m_pieces.size(); i++)
    {
        painter.save();
        if (m_pieces.at(i).type() == MyPiece)
            painter.setBrush(m_pieceColor);
        else
            painter.setBrush(m_otherPieceColor);
        painter.setPen(Qt::black);
        painter.translate(length / 14.0 * m_pieces.at(i).row(),
                          length / 14.0 * m_pieces.at(i).column());
        painter.drawEllipse(QPoint(0, 0), m_pieceSize, m_pieceSize);
        painter.restore();
    }

    // 画危险位置


    event->accept();
}

void Board::calcDangerous()
{
    m_dangers.clear();

    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
        {
            if (m_board[i][j] != PieceType::None)
                continue;
            m_pieces.append(Piece(i, j, PieceType::OtherPiece));
            m_board[i][j] = PieceType::OtherPiece;

            int three = 0;
            int halfFour = 0;
            calcDangerous(&three, &halfFour);

            m_board[i][j] = PieceType::None;
            m_pieces.removeLast();
        }

}

void Board::waitForInput()
{
    m_state = Run;
    update();
}

void Board::calcDangerous(int* three, int* halfFour)
{

}
