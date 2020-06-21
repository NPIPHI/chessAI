//
// Created by 16182 on 6/20/2020.
//

#include "ai.h"
#include <algorithm>
#include <iostream>

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
    auto [move, value] = ai::minMax(board, side, depth, true);
    std::cout << "Moves searched: " << movesSearched << std::endl;
    return move;
}

std::tuple<chessMove, float> ai::minMax(const board &startBoard, side side, int depth, bool max) {
    if(depth<=0) {
        chessMove * movesBegin = buffers.moveBuffer;
        chessMove * movesEnd = startBoard.inPlaceValidMoves(movesBegin, side);

        float * valuesBegin = buffers.valueBuffer;
        float * valuesEnd = valuesBegin + (movesEnd - movesBegin);

        startBoard.cacheValue();

        std::transform(movesBegin, movesEnd, valuesBegin, [&startBoard, side](chessMove move) {
            return startBoard.valueAfter(move, side);
        });

        movesSearched+= movesEnd - movesBegin;

        int bestIndex;
        if(max) {
            bestIndex = std::max_element(valuesBegin, valuesEnd) - valuesBegin;
        } else {
            bestIndex = std::min_element(valuesBegin, valuesEnd) - valuesBegin;
        }
        return {movesBegin[bestIndex], valuesBegin[bestIndex]};

    } else {
        auto moves = startBoard.validMoves(side);
        if(moves.empty()){
            return {{{0, 0}, {0,0}}, -100000};
        }
        auto boards = std::vector<class board>(moves.size());
        std::transform(moves.begin(), moves.end(), boards.begin(), [&startBoard](chessMove move) {
            return startBoard.applyMove(move);
        });

        enum side other = side == white ? black : white;
        auto sndLevelValues = std::vector<float>(boards.size());
        std::transform(boards.begin(), boards.end(), sndLevelValues.begin(), [max, other, depth](const board & subBoard){
            auto [move, value] = (minMax(subBoard, other, depth - 1, !max));
            return value;
        });
        int bestIndex;

        if(max){
            bestIndex = std::min_element(sndLevelValues.begin(), sndLevelValues.end()) - sndLevelValues.begin();
        } else {
            bestIndex = std::max_element(sndLevelValues.begin(), sndLevelValues.end()) - sndLevelValues.begin();
        }

        return {moves[bestIndex], sndLevelValues[bestIndex]};
    }
}

//std::tuple<chessMove, float> ai::minMax(const board &startBoard, side side, int depth, bool max) {
//    if(depth<=0) {
//        auto moves = startBoard.validMoves(side);
//        auto values = std::vector<float>(moves.size());
//        std::transform(moves.begin(), moves.end(), values.begin(), [&startBoard, side](chessMove move) {
//            return startBoard.applyMove(move).value(side);
//        });
//        movesSearched+=moves.size();
//
//        int bestIndex;
//        if(max) {
//            bestIndex = std::max_element(values.begin(), values.end()) - values.begin();
//        } else {
//            bestIndex = std::min_element(values.begin(), values.end()) - values.begin();
//        }
//        return {moves[bestIndex], values[bestIndex]};
//
//    } else {
//        auto moves = startBoard.validMoves(side);
//        if(moves.empty()){
//            return {{{0, 0}, {0,0}}, -100000};
//        }
//        auto boards = std::vector<class board>(moves.size());
//        std::transform(moves.begin(), moves.end(), boards.begin(), [&startBoard](chessMove move) {
//            return startBoard.applyMove(move);
//        });
//
//        enum side other = side == white ? black : white;
//        auto sndLevelValues = std::vector<float>(boards.size());
////         for(const auto & b: boards){
////             auto [move, value] = (minMax(b, other, depth - 1, !max));
////             sndLevelValues.emplace_back(value);
////         }
//       std::transform(boards.begin(), boards.end(), sndLevelValues.begin(), [max, other, depth](const board & subBoard){
//           auto [move, value] = (minMax(subBoard, other, depth - 1, !max));
//           return value;
//       });
//        int bestIndex;
//
//        if(max){
//            bestIndex = std::min_element(sndLevelValues.begin(), sndLevelValues.end()) - sndLevelValues.begin();
//        } else {
//            bestIndex = std::max_element(sndLevelValues.begin(), sndLevelValues.end()) - sndLevelValues.begin();
//        }
//
//        return {moves[bestIndex], sndLevelValues[bestIndex]};
//    }
//}