//
// Created by 16182 on 6/20/2020.
//

#include "ai.h"
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <limits>

long long movesSearched;
struct {
    chessMove moveBuffer[128];
    float valueBuffer[128];
} buffers;

constexpr float inf = std::numeric_limits<float>::infinity();

chessMove ai::bestMove(const board &board, int depth, side side) {
    if(depth > 7) {
        std::cout << "depth too deep" << std::endl;
        return {};
    }
    movesSearched = 0;
    auto move = ai::minimaxHead(board, depth, side);
//    auto [move, value] = ai::minimaxSlow(board, depth, true, side);
    std::cout << "Moves searched: " << movesSearched << std::endl;
    return move;
}

std::tuple<chessMove, float> ai::minimaxSlow(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide) {
    if(depth == 0){
        return {{}, startBoard.value(maxPlayerSide)};
    }
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    auto moves = startBoard.validMovesFast(evalSide);
    auto children = std::vector<std::tuple<chessMove, float>>(moves.size());
    std::transform(moves.begin(), moves.end(), children.begin(), [startBoard, depth, maxPlayer, maxPlayerSide](chessMove move){
        board b = startBoard.applyMove(move);
        return std::tuple{move, std::get<float>(minimaxSlow(b, depth - 1, !maxPlayer, maxPlayerSide))};
    });
    std::tuple<chessMove, float> move;
    if(maxPlayer){
        move = std::tuple{chessMove{}, -inf};
        for(auto v : children){
            if(std::get<float>(v) > std::get<float>(move)){
                move = v;
            }
        }
    }
    if(!maxPlayer){
        move = std::tuple{chessMove{}, inf};
        for(auto v : children){
            if(std::get<float>(v) < std::get<float>(move)){
                move = v;
            }
        }
    }
    return move;
}

chessMove ai::minimaxHead(const board &startBoard, int depth, side maxPlayerSide) {
    if(depth == 2){
        return std::get<chessMove>(minimaxSlow(startBoard, 2, true, maxPlayerSide));
    }
    if(depth <= 1){
        return std::get<chessMove>(minimaxSlow(startBoard, 1, true, maxPlayerSide));
    }
    auto moves = startBoard.validMovesFast(maxPlayerSide);
    auto values = std::vector<float>(moves.size());
    float bestValue = -inf;
    std::transform(moves.begin(), moves.end(), values.begin(), [&](chessMove move){
        float moveValue =  minimax(startBoard.applyMove(move), depth - 1, bestValue, inf, false, maxPlayerSide);
        bestValue = std::max(bestValue, moveValue);
        return moveValue;
    });
    int index = std::max_element(values.begin(), values.end()) - values.begin();
    return moves[index];
}

float ai::minimax(const board &startBoard, int depth, float a, float b, bool maxPlayer, side maxPlayerSide) {
    if(depth == 1){
        return minimaxBase(startBoard, depth, maxPlayer, maxPlayerSide);
    }
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    auto moves = startBoard.validMovesFast(evalSide);
    if(maxPlayer){
        float value = -inf;
        for(auto move : moves){
            value = std::max(value, minimax(startBoard.applyMove(move), depth - 1, a, b, false, maxPlayerSide));
            a = std::max(value, a);
            if (a >= b){
                break;
            }
        }
        return value;
    } else {
        float value = inf;
        for(auto move : moves){
            value = std::min(value, minimax(startBoard.applyMove(move), depth - 1, a, b, true, maxPlayerSide));
            b = std::min(value, b);
            if (a >= b){
                break;
            }
        }
        return value;
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
    if(vEnd == vBegin){
        if(maxPlayer){
            return -inf;
        } else {
            return inf;
        }
    }
    if(maxPlayer){
        return *std::max_element(vBegin, vEnd);
    } else {
        return *std::min_element(vBegin, vEnd);
    }
}

std::vector<chessMove> ai::minimaxHeadTrace(const board &startBoard, int depth, side maxPlayerSide) {
    if(depth <= 1){
        auto[move, value] = (minimaxSlow(startBoard, 1, true, maxPlayerSide));
        return {move};
    }
    auto moves = startBoard.validMovesFast(maxPlayerSide);
    auto values = std::vector<float>(moves.size());
    std::transform(moves.begin(), moves.end(), values.begin(), [&](chessMove move){
        return minimax(startBoard.applyMove(move), depth, -inf, inf, false, maxPlayerSide);
    });
    int index = std::max_element(values.begin(), values.end()) - values.begin();
    chessMove move = moves[index];
    std::vector<chessMove> ret = {move};
    auto append = minimaxTrace(startBoard.applyMove(move), depth, -inf, inf,false, maxPlayerSide);
    ret.insert(ret.end(), append.begin(), append.end());
    return ret;
}

std::vector<chessMove>
ai::minimaxTrace(const board &startBoard, int depth, float a, float b, bool maxPlayer, side maxPlayerSide) {
    if(depth == 1){
        return minimaxBaseTrace(startBoard, depth, maxPlayer, maxPlayerSide);
    }
    side evalSide = maxPlayer ? maxPlayerSide : ((maxPlayerSide == white) ? black : white);
    auto moves = startBoard.validMovesFast(evalSide);

    if(maxPlayer){
        auto values = std::vector<float>(moves.size());
        std::transform(moves.begin(), moves.end(), values.begin(), [&](chessMove move){
           return minimax(startBoard.applyMove(move), depth - 1, a, b, false, maxPlayerSide);
        });
        int index = std::max_element(values.begin(), values.end()) - values.begin();
        chessMove bestMove = moves[index];
        std::vector<chessMove> ret = {bestMove};
        auto append = minimaxTrace(startBoard.applyMove(bestMove), depth - 1, a, b, false, maxPlayerSide);
        ret.insert(ret.end(), append.begin(), append.end());
        return ret;

    } else {
        auto values = std::vector<float>(moves.size());
        std::transform(moves.begin(), moves.end(), values.begin(), [&](chessMove move){
            return minimax(startBoard.applyMove(move), depth - 1, a, b, true, maxPlayerSide);
        });
        int index = std::min_element(values.begin(), values.end()) - values.begin();
        chessMove bestMove = moves[index];
        std::vector<chessMove> ret = {bestMove};
        auto append = minimaxTrace(startBoard.applyMove(bestMove), depth - 1, a, b, true, maxPlayerSide);
        ret.insert(ret.end(), append.begin(), append.end());
        return ret;
    }
}

std::vector<chessMove>
ai::minimaxBaseTrace(const board &startBoard, int depth, bool maxPlayer, side maxPlayerSide) {
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
    int index;
    if(maxPlayer){
        index = std::max_element(vBegin, vEnd) - vBegin;
    } else {
        index = std::min_element(vBegin, vEnd) - vBegin;
    }
    return {mBegin[index]};
}
