#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QPaintEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QRect>
#include <QVector>
#include <QDebug>

namespace Ui {
class Board;
}

class Board : public QWidget
{
    Q_OBJECT

public:
    class Piece;
    enum PieceType { MyPiece, OtherPiece, None };
    enum State {Undefined, WaitForConnect, Pend, Run, Win, Lost };
    explicit Board(QWidget *parent = 0);
    void init(const State& state, const QColor& pieceColor = Qt::white);
    void setState(State state) { m_state = state; }
    // 画棋盘和棋子
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    // 检查是否下完后赢了
    void checkState();
    // 画出危险位置
    void drawDangerous(bool danger)
    {
        m_danger = danger;
        calcDangerous();
        update();
    }
    Piece getLastInput() const
    {
        return m_pieces.last();
    }
    ~Board();

    class Piece
    {
    public:
        Piece(int r = 0, int c = 0, PieceType t = MyPiece) :
            _row(r), _column(c), _type(t) {}
        int row() const { return _row; }
        int column() const { return _column; }
        PieceType type() const { return _type; }

    private:
        int _row;
        int _column;
        PieceType _type;
    };

signals:
    void inputFinished(const Board::Piece& piece);

protected:
    void calcDangerous();
    void calcDangerous(int* three, int* halfFour);

public slots:
    void addPiece(const Piece& piece);
    int toIndex(int pos);
    // 等待输入
    void waitForInput();
    void addOtherPiece(int r, int c)
    {
        Piece piece(r, c, OtherPiece);
        if (m_board[r][c] != None)
            return;
        m_board[r][c] = OtherPiece;
        m_pieces.append(piece);

        qDebug() << "\n*******addOtherPiece*******";
        for (int i = 0; i < m_pieces.size(); i++)
        {
            qDebug() << "pieceColor" << m_pieceColor << "Other piece Color" << m_otherPieceColor;
            qDebug() << "画棋子" << m_pieces.at(i).row() << m_pieces.at(i).column()
                     << m_pieces.at(i).type();
            qDebug() << "MyPiece" << MyPiece << "OtherPiece" << OtherPiece;
        }

        update();
    }

private:
    Ui::Board *ui;
    QColor m_backgroundColor;
    QColor m_pieceColor;
    QColor m_otherPieceColor;
    State m_state;
    int m_pointSize;
    int m_pieceSize;
    bool m_danger;

    int m_margin;
    int m_length;
    QVector<Piece> m_pieces;
    QVector<QPoint> m_dangers;
    PieceType m_board[15][15];
};

#endif // BOARD_H
