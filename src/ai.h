//
// Created by 16182 on 6/20/2020.
//

#ifndef CHESS_AI_H
#define CHESS_AI_H


#include "board.h"

class ai {
public:
    static chessMove bestMove(const board &board, int depth, side side);
    static std::vector<chessMove> minimaxHeadTrace(const board &startBoard, int depth, side maxPlayerSide);
private:
    static std::tuple<chessMove, int> minimaxSlow(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide);
    static chessMove minimaxHead(const board &startBoard, int depth, side maxPlayerSide);
    static int minimax(const board &startBoard, int depth, int a, int b, bool maxPlayer, side maxPlayerSide);
    static int minimaxBase(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide);

    static std::vector<chessMove> minimaxTrace(const board &startBoard, int depth, int a, int b, bool maxPlayer, side maxPlayerSide);
    static std::vector<chessMove> minimaxBaseTrace(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide);
};


#endif //CHESS_AI_H
