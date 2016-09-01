#include "board.h"
#include "ui_board.h"

Board::Board(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Board)
{
    ui->setupUi(this);
    m_background.load(":/background/wood.png");
    m_bomb.load(":/icon/bomb.png");
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
    m_pieces.clear();
    m_dangers.clear();
    qDebug() << m_pieceColor << m_otherPieceColor;
    update();
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
        // qDebug() << x << y;
        int c = toIndex(x - (width()-m_length)/2);
        int r = toIndex(y - (height()-m_length)/2);
        if (c < 0 || c >= 15 || r < 0 || r >= 15)
            return;
        if (m_board[r][c] != PieceType::None)
            return;
        // qDebug() << r << c;
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
    m_danger = false;
    m_dangers.clear();
    m_state = Pend;
    emit inputFinished(piece);
    update();
}

// 检查是不是赢了
bool Board::checkState()
{
    // 把棋盘打印出来
    qDebug() << "checkState";
    for (int i = 0; i < 15; i++)
    {
        QString state;
        for (int j = 0; j < 15; j++)
            if (m_board[i][j] == None)
                state += "-";
            else if (m_board[i][j] == MyPiece)
                state += "1";
            else
                state += "0";
        qDebug() << state;
    }
    qDebug() << '\n';

    for (int i = 0; i < 15; i++)
    {
        // 横
        int j = 0;
        while (j < 15)
        {
            while (j < 15 && m_board[i][j] != PieceType::MyPiece)
                j++;
            if (j >= 15)
                break;
            int k = 0;
            while (j < 15 && m_board[i][j] == PieceType::MyPiece)
            {
                j++;
                k++;
            }
            if (k >= 5)
                return true;
        }
        // 竖
        j = 0;
        while (j < 15)
        {
            while (j < 15 && m_board[j][i] != PieceType::MyPiece)
                j++;
            if (j >= 15)
                break;
            int k = 0;
            while (j < 15 && m_board[j][i] == PieceType::MyPiece)
            {
                j++;
                k++;
            }
            if (k >= 5)
                return true;
        }
    }

    // 左斜
    for (int i = 0; i < 15; i++)
    {
        int j = 0;
        while (j + i < 15)
        {
            while (j + i < 15 && m_board[j][i+j] != PieceType::MyPiece)
                j++;
            if (j+i == 15) break;
            int k = 0;
            while (i+j < 15 && m_board[j][i+j] == PieceType::MyPiece)
            {
                k++;
                j++;
            }
            if (k >= 5) return true;
        }

        j = 0;
        while (j + i < 15)
        {
            while (j + i < 15 && m_board[i+j][j] != PieceType::MyPiece)
                j++;
            if (j+i == 15) break;
            int k = 0;
            while (i+j < 15 && m_board[i+j][j] == PieceType::MyPiece)
            {
                k++;
                j++;
            }
            if (k >= 5) return true;
        }
    }
    // 右斜
    for (int i = 0; i < 15; i++)
    {
        int j = 0;
        while (j <= i)
        {
            while (j <= i && m_board[i-j][j] != PieceType::MyPiece)
                j++;
            if (j > i) break;
            int k = 0;
            while (j <= i && m_board[i-j][j] == PieceType::MyPiece)
            {
                k++;
                j++;
            }
            if (k >= 5) return true;
        }


        j = 14;
        int m = i+j;
        while (j >= 0)
        {
            while (j >= 0 && m_board[m-j][j] != PieceType::MyPiece)
                j--;
            if (j < 0) break;
            int k = 0;
            while (j >= 0 && m_board[m-j][j] == PieceType::MyPiece)
            {
                k++;
                j--;
            }
            if (k >= 5) return true;
        }

    }

    return false;
}

int Board::toIndex(int pos)
{
    return std::floor(pos / (m_length/14.0) + 0.5);
}

// 绘制棋盘
void Board::paintEvent(QPaintEvent *event)
{
    int totWidth = width(), totHeight = height();
    int length = std::min(totWidth, totHeight) - m_margin;
    m_length = length;

    m_pieceSize = length / 14.0 * 0.45;
    m_pointSize = std::floor(length / 14.0 * 0.05 + 0.5);
    m_pointSize = m_pointSize == 0 ? 1 : m_pointSize;

    QPainter painter(this);
    // 画背景
    painter.setPen(Qt::NoPen);
    // painter.setBrush(QBrush(m_background));
    painter.drawPixmap((totWidth - length - m_margin)/2, (totHeight - length - m_margin)/2,
                       length + m_margin, length + m_margin,
                       m_background);

    // 画格子
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

        /*
        qDebug() << "pieceColor" << m_pieceColor << "Other piece Color" << m_otherPieceColor;
        qDebug() << "画棋子" << m_pieces.at(i).row() << m_pieces.at(i).column()
                 << m_pieces.at(i).type();
        qDebug() << "MyPiece" << MyPiece << "OtherPiece" << OtherPiece;
        */
        if (m_pieces.at(i).type() == MyPiece)
            painter.setBrush(m_pieceColor);
        else
            painter.setBrush(m_otherPieceColor);
        painter.setPen(Qt::black);
        painter.translate(length / 14.0 * m_pieces.at(i).column(),
                          length / 14.0 * m_pieces.at(i).row());
        painter.drawEllipse(QPoint(0, 0), m_pieceSize, m_pieceSize);
        painter.restore();
    }

    // 画危险位置
    if (m_danger)
    {
        for (int i = 0; i < m_dangers.size(); i++)
        {
            painter.save();
            painter.translate(length / 14.0 * m_dangers.at(i).x(),
                              length / 14.0 * m_dangers.at(i).y());
            // painter.drawEllipse(QPoint(0, 0), m_pieceSize, m_pieceSize);
            painter.drawPixmap(QPoint(-m_pieceSize, -m_pieceSize), m_bomb.scaled(m_pieceSize*2, m_pieceSize*2,
                                                           Qt::KeepAspectRatio));
            painter.restore();
        }
    }

    event->accept();
}

void Board::calcDangerous()
{
    m_dangers.clear();
    m_danger = true;
    int oriThree = 0, oriHalfFour = 0;
    calcDangerous(&oriThree, &oriHalfFour);
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
            if (three >= 2 && three > oriThree)
            {
                // qDebug() << i << j << three << halfFour;
                m_dangers.append(QPoint(j, i));
            }
            else if (three > 0 && halfFour > 0 && (three > oriThree || halfFour > oriHalfFour))
            {
                m_dangers.append(QPoint(j, i));
            }
            m_board[i][j] = PieceType::None;
            m_pieces.removeLast();
        }
    update();
}

void Board::waitForInput()
{
    m_state = Run;
    update();
}

void Board::calcDangerous(int* a, int* b)
{
    // qDebug() << "calcDangerous()";
    int three = 0, halfFour = 0;
    for (int i = 0; i < 15; i++)
    {
        // 横
        int j = 0;
        while (j < 15)
        {
            int whole = 0;
            while (j < 15 && m_board[i][j] != PieceType::OtherPiece)
                j++;
            if (j >= 15) break;
            if (j == 0) whole = 1;
            else if (m_board[i][j-1] == PieceType::MyPiece) whole = 1;

            int k = 0;
            while (j < 15 && m_board[i][j] == PieceType::OtherPiece)
            {
                j++;
                k++;
            }
            if (j == 15 || m_board[i][j] == PieceType::MyPiece)
                whole++;


            if (k == 4 && whole == 1)
                halfFour++;
            else if (k == 3 && whole == 0)
                three++;

            /*
            if (k == 4) halfFour++;
            if (k == 3) three++;
            */
        }
        // 竖
        j = 0;
        while (j < 15)
        {
            int whole = 0;
            while (j < 15 && m_board[j][i] != PieceType::OtherPiece) j++;
            if (j >= 15) break;
            if (j == 0) whole = 1;
            else if (m_board[j-1][i] == PieceType::MyPiece) whole = 1;

            int k = 0;
            while (j < 15 && m_board[j][i] == PieceType::OtherPiece)
            {
                j++;
                k++;
            }
            if (j == 15 || m_board[j][i] == PieceType::MyPiece)
                whole++;

            if (k == 4 && whole == 1)
                halfFour++;
            else if (k == 3 && whole == 0)
                three++;

            /*
            if (k == 4) halfFour++;
            if (k == 3) three++;
            */
        }
    }


    // 左斜 '\'
    for (int i = 0; i < 15; i++)
    {
        int j = 0;
        while (j + i < 15)
        {
            int whole = 0;
            while (j + i < 15 && m_board[j][i+j] != PieceType::OtherPiece)
                j++;
            if (j+i == 15) break;
            if (j == 0 || m_board[j-1][i+j-1] == PieceType::MyPiece)
                whole = 1;

            int k = 0;
            while (i+j < 15 && m_board[j][i+j] == PieceType::OtherPiece)
            {
                k++;
                j++;
            }
            if (i+j == 15 || m_board[j][i+j] == PieceType::MyPiece)
                whole++;

            if (whole == 1 && k == 4)
                halfFour++;
            if (whole == 0 && k == 3)
                three++;
        }

        j = 0;
        while (j + i < 15)
        {
            int whole = 0;
            while (j + i < 15 && m_board[i+j][j] != PieceType::OtherPiece)
                j++;
            if (j+i == 15) break;
            if (j == 0 || m_board[i+j-1][j-1] == PieceType::MyPiece)
                whole = 1;

            int k = 0;
            while (i+j < 15 && m_board[i+j][j] == PieceType::OtherPiece)
            {
                k++;
                j++;
            }
            if (i+j == 15 || m_board[i+j][j] == PieceType::MyPiece)
                whole++;

            if (whole == 1 && k == 4)
                halfFour++;
            if (whole == 0 && k == 3)
                three++;
        }
    }

    // 右斜 '/'
    for (int i = 0; i < 15; i++)
    {
        int j = 0;
        while (j <= i)
        {
            int whole = 0;
            while (j <= i && m_board[i-j][j] != PieceType::OtherPiece)
                j++;
            if (j > i) break;
            if (j == 0 || m_board[i-j+1][j-1] == PieceType::MyPiece)
                whole = 1;

            int k = 0;
            while (j <= i && m_board[i-j][j] == PieceType::OtherPiece)
            {
                k++;
                j++;
            }
            if (j > i || m_board[i-j][j] == PieceType::MyPiece)
                whole++;
            if (whole == 1 && k == 4)
                halfFour++;
            if (whole == 0 && k == 3)
                three++;
        }


        j = 14;
        int m = i+j;
        while (j >= 0)
        {
            int whole = 0;
            while (j >= 0 && m_board[m-j][j] != PieceType::OtherPiece)
                j--;
            if (j < 0) break;
            if (j == 14 || m_board[m-j-1][j+1] == PieceType::MyPiece)
                whole = 1;

            int k = 0;
            while (j >= 0 && m_board[m-j][j] == PieceType::OtherPiece)
            {
                k++;
                j--;
            }
            if (j < 0 || m_board[m-j][j] == PieceType::MyPiece)
                whole++;
            if (whole == 1 && k == 4)
                halfFour++;
            if (whole == 0 && k == 3)
                three++;
        }

    }

    if (three != 0 && halfFour != 0)
        qDebug() << "three" << three << "halffour" << halfFour;
    (*a) = three;
    (*b) = halfFour;
}
