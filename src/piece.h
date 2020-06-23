//
// Created by 16182 on 6/20/2020.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <tuple>
#include "chessMove.h"
#include "array"

enum pieceType : char {
    empty = 0,
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6
};

enum side : char {
    neither,
    oob,
    white,
    black,
};

enum moveType : char {
    invalid = 0,
    capture = 1,
    noCapture = 2,
};

class piece {
public:
    [[nodiscard]] std::vector<chessMove> moves(const std::array<std::array<piece, 8>, 8> &board, square pos) const;
    chessMove * inPlaceMoves(chessMove *out, const std::array<std::array<piece, 8>, 8> &board, square pos) const;
    side side = neither;
    pieceType type = empty;
    char homeRank() const;
    [[nodiscard]] std::tuple<float, float, float, float> UV() const;
    [[nodiscard]] moveType isMovable(piece target) const;
    [[nodiscard]] int value(square sq) const;

    char ascii() const;

    char oppositeRow() const;
};


#endif //CHESS_PIECE_H
