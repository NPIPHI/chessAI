//
// Created by 16182 on 6/20/2020.
//

#include "ai.h"
#include <algorithm>
#include <iostream>
#include <assert.h>

int movesSearched;
struct {
    chessMove moveBuffer[128];
    float valueBuffer[128];
} buffers;

chessMove ai::bestMove(const board &board, side side, int depth) {
    if(depth > 7){
        std::cout << "depth too deep" << std::endl;
    }
    movesSearched = 0;
//    auto [move, value] = ai::minMax(board, side, true, depth);
    auto move = ai::minimaxHead(board, depth, side);
    std::cout << "Moves searched: " << movesSearched << std::endl;
    return move;
}

std::tuple<chessMove, float> ai::minimaxSlow(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide) {
    if(depth == 0){
        return {{}, startBoard.value(maxPlayerSide)};
    }
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    auto moves = startBoard.validMoves(evalSide);
    auto children = std::vector<std::tuple<chessMove, float>>(moves.size());
    std::transform(moves.begin(), moves.end(), children.begin(), [startBoard, depth, maxPlayer, maxPlayerSide](chessMove move){
        board b = startBoard.applyMove(move);
        return std::tuple{move, std::get<float>(minimaxSlow(b, depth - 1, !maxPlayer, maxPlayerSide))};
    });
    std::tuple<chessMove, float> move;
    if(maxPlayer){
        move = std::tuple{chessMove{}, -1000000000};
        for(auto v : children){
            if(std::get<float>(v) > std::get<float>(move)){
                move = v;
            }
        }
    }
    if(!maxPlayer){
        move = std::tuple{chessMove{}, 1000000000};
        for(auto v : children){
            if(std::get<float>(v) < std::get<float>(move)){
                move = v;
            }
        }
    }
    return move;
}

chessMove ai::minimaxHead(const board &startBoard, int depth, side maxPlayerSide) {
    if(depth <= 1){
        return std::get<chessMove>(minimaxSlow(startBoard, 1, true, maxPlayerSide));
    }
    auto moves = startBoard.validMoves(maxPlayerSide);
    auto values = std::vector<float>(moves.size());
    std::transform(moves.begin(), moves.end(), values.begin(), [&](chessMove move){
        return minimax(startBoard.applyMove(move), depth, false, maxPlayerSide);
    });
    int index = std::max_element(values.begin(), values.end()) - values.begin();
    return moves[index];
}

float ai::minimax(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide) {
    if(depth == 1){
        return minimaxBase(startBoard, depth, !maxPlayer, maxPlayerSide);
    }
    if(depth == 0){
        return startBoard.value(maxPlayerSide);
    }
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    auto moves = startBoard.validMoves(evalSide);
    auto children = std::vector<float>(moves.size());
    std::transform(moves.begin(), moves.end(), children.begin(), [startBoard, depth, maxPlayer, maxPlayerSide](chessMove move){
        board b = startBoard.applyMove(move);
        return minimax(b, depth - 1, !maxPlayer, maxPlayerSide);
    });
    if(maxPlayer){
        return *std::max_element(children.begin(), children.end());
    } else {
        return *std::min_element(children.begin(), children.end());
    }
}

float ai::minimaxBase(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide) {
    assert(depth == 1);
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    chessMove * mBegin = buffers.moveBuffer;
    chessMove * mEnd = startBoard.inPlaceValidMoves(mBegin, evalSide);
    float * vBegin = buffers.valueBuffer;
    float * vEnd = vBegin + (mEnd - mBegin);
    startBoard.cacheValue();
    std::transform(mBegin, mEnd, vBegin, [&](chessMove move){
       return startBoard.valueAfter(move, maxPlayerSide);
    });
    movesSearched += vEnd - vBegin;
    if(maxPlayer){
        return *std::max_element(vBegin, vEnd);
    } else {
        return *std::min_element(vBegin, vEnd);
    }
}
