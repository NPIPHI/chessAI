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

class board {
public:
    std::array<std::array<piece, 8>, 8> state;
    //vert, uv
    [[nodiscard]] std::vector<float> toBuffer() const;

    void setDefault();
    bool isValid(chessMove move, enum side side);
    board applyMove(chessMove move) const;
    [[nodiscard]] std::vector<chessMove> validMoves(side side) const;
    [[nodiscard]] piece atSquare(square sq) const;
    [[nodiscard]] float value(enum side side) const;
    std::string print() const;
};


#endif //CHESS_BOARD_H
