//
// Created by 16182 on 6/20/2020.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <array>
#include <vector>
#include "piece.h"
#include "chessMove.h"
#include "string"
#include <numeric>

class board {
public:
    std::array<std::array<piece, 8>, 8> state;
    //vert, uv
    [[nodiscard]] std::vector<float> toBuffer() const;

    void setDefault();
    bool isValid(chessMove move, enum side side) const;
    void cacheValue() const;
    [[nodiscard]] board applyMove(chessMove move) const;
    //no check for check
    [[nodiscard]] std::vector<chessMove> validMovesFast(side side) const;
    //disallows check
    [[nodiscard]] std::vector<chessMove> validMoves(side side) const;
    chessMove * inPlaceValidMoves(chessMove * out, side side) const;
    [[nodiscard]] piece atSquare(square sq) const;
    [[nodiscard]] int value(enum side side) const;
    [[nodiscard]] int valueAfter(chessMove move, enum side side) const;
    [[nodiscard]] std::string print() const;
    [[nodiscard]] bool inCheck(side side) const;
    [[nodiscard]] bool checkmate(side side) const;
private:
    //side being threatened
    [[nodiscard]] std::vector<square> threatedSquares(side side) const;
    mutable int valueCache;
};


#endif //CHESS_BOARD_H
