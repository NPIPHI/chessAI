//
// Created by 16182 on 6/20/2020.
//

#ifndef CHESS_AI_H
#define CHESS_AI_H


#include "board.h"

class ai {
public:
    static chessMove bestMove(const board &board, side side, int depth);

private:
    static std::tuple<chessMove, float> minMax(const board &startBoard, side side, int depth, bool max);
};


#endif //CHESS_AI_H
