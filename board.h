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

namespace Ui {
class Board;
}

class Board : public QWidget
{
    Q_OBJECT

public:
    enum PieceType { MyPiece, OtherPiece };
    explicit Board(QWidget *parent = 0);
    void paintEvent(QPaintEvent* event);
    ~Board();

protected:
    class Piece
    {
        int row;
        int column;
        PieceType type;
    };

private:
    Ui::Board *ui;
    QColor m_backgroundColor;
    int m_pointSize;
    int m_pieceSize;
    QVector<Piece> pieces;
};

#endif // BOARD_H
