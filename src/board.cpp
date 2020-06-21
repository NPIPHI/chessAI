//
// Created by 16182 on 6/20/2020.
//

#include <algorithm>
#include <iostream>
#include "board.h"

std::vector<float> board::toBuffer() const{
    std::vector<float> renderBuffer;
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            const piece & piece = state[y][x];
            if(piece.type != empty){
                auto [u1, v1, u2, v2] = piece.UV();
                auto [x1, y1, x2, y2] = std::tuple{float(x)/8, float(y)/8, float(x+1)/8, float(y+1)/8};
                std::vector<float> pieceBuffer = {
                        x1, y1,
                        u1, v1,
                        x2, y1,
                        u2, v1,
                        x1, y2,
                        u1, v2,

                        x2, y1,
                        u2, v1,
                        x2, y2,
                        u2, v2,
                        x1, y2,
                        u1, v2,
                };
                renderBuffer.insert(renderBuffer.end(), pieceBuffer.begin(), pieceBuffer.end());
            }
        }
    }
    return renderBuffer;
}

void board::setDefault() {
    state[0][0] = {white, rook};
    state[0][1] = {white, knight};
    state[0][2] = {white, bishop};
    state[0][3] = {white, queen};
    state[0][4] = {white, king};
    state[0][5] = {white, bishop};
    state[0][6] = {white, knight};
    state[0][7] = {white, rook};
    for(piece & p : state[1]){
        p = {white, pawn};
    }
    state[7][0] = {black, rook};
    state[7][1] = {black, knight};
    state[7][2] = {black, bishop};
    state[7][4] = {black, king};
    state[7][3] = {black, queen};
    state[7][5] = {black, bishop};
    state[7][6] = {black, knight};
    state[7][7] = {black, rook};
    for(piece & p : state[6]){
        p = {black, pawn};
    }
}

std::vector<chessMove> board::validMoves(side side) const {
    std::vector<chessMove> moves;
    for(char rank = 0; rank < 8; rank++){
        for(char file = 0; file < 8; file++){
            piece p = atSquare({rank, file});
            if(p.side == side){
                auto pieceMoves = p.moves(state, {rank, file});
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    return moves;
}

board board::applyMove(chessMove move) const {
    board ret;
    ret.state = state;
    piece & end = ret.state[move.end.rank][move.end.file];
    end = state[move.start.rank][move.start.file];
    ret.state[move.start.rank][move.start.file] = {};
    if(end.type == pawn){
        if(end.side == white){
            if(move.end.rank == 7){
                end.type = queen;
            }
        } else if(move.end.rank == 0){
            end.type = queen;
        }
    }
    return ret;
}

piece board::atSquare(square sq) const {
    if(sq.rank >= 0 && sq.rank < 8 && sq.file >= 0 && sq.file < 8){
        return state[sq.rank][sq.file];
    } else {
        return {oob, empty};
    }
}

float board::value(enum side side) const {
    float val = 0;
    for(char rank = 0; rank < 8; rank++){
        for(char file = 0; file < 8; file++){
            piece p = atSquare({rank, file});
            if(p.side == white) val += p.value({rank, file});
            if(p.side == black) val -= p.value({rank, file});
        }
    }
    if(side == black){
        val *= -1;
    }
    return val;
}

bool board::isValid(chessMove move, enum side side) {
    auto moves = validMoves(side);
    if(std::find(moves.begin(), moves.end(), move) != moves.end()){
        return true;
    } else {
        return false;
    }
}

std::string board::print() const {
    std::string ret;
    for(auto rank : state){
        for(auto piece : rank){
            ret += piece.ascii();
        }
        ret += '\n';
    }
    return ret;
}