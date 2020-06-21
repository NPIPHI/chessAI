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
    static std::tuple<chessMove, float> minimaxSlow(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide);
    static chessMove minimaxHead(const board &startBoard, int depth, side maxPlayerSide);
    static float minimax(const board &startBoard, int depth, float a, float b, bool maxPlayer, side maxPlayerSide);
    static float minimaxBase(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide);
};


#endif //CHESS_AI_H
