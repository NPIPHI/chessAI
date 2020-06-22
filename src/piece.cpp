//
// Created by 16182 on 6/20/2020.
//

#include <vector>
#include "piece.h"

float pieceUV[] = {
        0,
        0.75,
        0.5,
        0.375,
        0.625,
        0.25,
        0.125
};


float pieceValue[] = {
        0,
        1,
        3,
        3,
        5,
        9,
        100
};

std::tuple<float, float, float, float> piece::UV() const{
    float y = (side == white) ? 0.5 : 0;
//    return {0, 0, 1, 1};
    return {pieceUV[type], y, pieceUV[type] + 0.125, y + 0.5};
}

char piece::homeRank() const {
    if(side == white){
        if(type == pawn) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if(type == pawn) {
            return 6;
        } else {
            return 7;
        }
    }
}

std::vector<chessMove> piece::moves(const std::array<std::array<piece, 8>, 8> &board, square pos) const {
    std::vector<chessMove> validMoves;
    auto atSquare = [&board](square sq){
        if(sq.rank >= 0 && sq.rank < 8 && sq.file >= 0 && sq.file < 8){
            return board[sq.rank][sq.file];
        } else {
            return piece{oob, empty};
        }};
    if(type == pawn){
        if(side == white){
            if(pos.rank == homeRank()){
                if(atSquare(pos + square(1, 0)).side == neither) {
                    if (atSquare(pos + square(2, 0)).side == neither) {
                        validMoves.push_back({pos, pos + square(2, 0)});
                    }
                }
            }
            if(atSquare(pos + square(1, 0)).side == neither){
                validMoves.push_back({pos, pos + square(1, 0)});
            }
            if(atSquare(pos + square(1, 1)).side == black){
                validMoves.push_back({pos, pos + square(1, 1)});
            }
            if(atSquare(pos + square(1, -1)).side == black){
                validMoves.push_back({pos, pos + square(1, -1)});
            }
        }
        if(side == black){
            if(pos.rank == homeRank()){
                if(atSquare(pos + square(-1, 0)).side == neither) {
                    if (atSquare(pos + square(-2, 0)).side == neither) {
                        validMoves.push_back({pos, pos + square(-2, 0)});
                    }
                }
            }
            if(atSquare(pos + square(-1, 0)).side == neither){
                validMoves.push_back({pos, pos + square(-1, 0)});
            }
            if(atSquare(pos + square(-1, 1)).side == white){
                validMoves.push_back({pos, pos + square(-1, 1)});
            }
            if(atSquare(pos + square(-1, -1)).side == white){
                validMoves.push_back({pos, pos + square(-1, -1)});
            }
        }
    }
    if(type == king){
        square dirs[] = {{1, 0}, {1, 1}, {1, -1}, {0, 1},
                         {0, -1}, {-1, 0}, {-1, 1}, {-1, -1}};
        for(square dir : dirs){
            if(isMovable(atSquare(pos + dir))){
                validMoves.push_back({pos, pos + dir});
            }
        }
    }
    if(type == rook){
        square dirs[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                validMoves.push_back({pos, pos+offset});
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == bishop){
        square dirs[] = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                validMoves.push_back({pos, pos+offset});
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == queen){
        square dirs[] = {{1, 0}, {1, 1}, {1, -1}, {0, 1},
                         {0, -1}, {-1, 0}, {-1, 1}, {-1, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                validMoves.push_back({pos, pos+offset});
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == knight){
        square dirs[] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                         {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
        for(square dir : dirs){
            if(isMovable(atSquare(pos + dir))){
                validMoves.push_back({pos, pos + dir});
            }
        }
    }

    return validMoves;
}

moveType piece::isMovable(piece target) const {
    if(side == white){
        if(target.side == neither){
            return noCapture;
        }
        if(target.side == black){
            return capture;
        }
    } else {
        if(target.side == neither){
            return noCapture;
        }
        if(target.side == white){
            return capture;
        }
    }
    return invalid;
}

float piece::value(square sq, enum side side) const {
    if(type == pawn){
        if(side == black){
            return pieceValue[type] + float(7 - sq.rank)/100;
        } else {
            return pieceValue[type] + float(sq.rank)/100;
        }
    }
    if(type == king){
        if(this->side != side){
            //prevents ai from trading kings
            return pieceValue[type] * 2;
        }
    }
    return pieceValue[type];
}

char piece::ascii() const {
    int isBlack = (side == black) ? 32 : 0;
    switch (type) {
        case empty:
            return '_';
        case pawn:
            return 'P' + isBlack;
        case rook:
            return 'R' + isBlack;
        case bishop:
            return 'B' + isBlack;
        case knight:
            return 'K' + isBlack;
        case queen:
            return 'Q' + isBlack;
        case king:
            return 'M' + isBlack;
    }
    return 'V';
}

chessMove * piece::inPlaceMoves(chessMove * out, const std::array<std::array<piece, 8>, 8> &board, square pos) const {
    auto atSquare = [&board](square sq){
        if(sq.rank >= 0 && sq.rank < 8 && sq.file >= 0 && sq.file < 8){
            return board[sq.rank][sq.file];
        } else {
            return piece{oob, empty};
        }};
    if(type == pawn){
        if(side == white){
            if(pos.rank == homeRank()){
                if(atSquare(pos + square(1, 0)).side == neither) {
                    if (atSquare(pos + square(2, 0)).side == neither) {
                        *out = {pos, pos + square(2, 0)};
                        out++;
                    }
                }
            }
            if(atSquare(pos + square(1, 0)).side == neither){
                *out = {pos, pos + square(1, 0)};
                out++;
            }
            if(atSquare(pos + square(1, 1)).side == black){
                *out = {pos, pos + square(1, 1)};
                out++;
            }
            if(atSquare(pos + square(1, -1)).side == black){
                *out = {pos, pos + square(1, -1)};
                out++;
            }
        }
        if(side == black){
            if(pos.rank == homeRank()){
                if(atSquare(pos + square(-1, 0)).side == neither) {
                    if (atSquare(pos + square(-2, 0)).side == neither) {
                        *out = {pos, pos + square(-2, 0)};
                        out++;
                    }
                }
            }
            if(atSquare(pos + square(-1, 0)).side == neither){
                *out = {pos, pos + square(-1, 0)};
                out++;
            }
            if(atSquare(pos + square(-1, 1)).side == white){
                *out = {pos, pos + square(-1, 1)};
                out++;
            }
            if(atSquare(pos + square(-1, -1)).side == white){
                *out = {pos, pos + square(-1, -1)};
                out++;
            }
        }
    }
    if(type == king){
        square dirs[] = {{1, 0}, {1, 1}, {1, -1}, {0, 1},
                         {0, -1}, {-1, 0}, {-1, 1}, {-1, -1}};
        for(square dir : dirs){
            if(isMovable(atSquare(pos + dir))){
                *out = {pos, pos + dir};
                out++;
            }
        }
    }
    if(type == rook){
        square dirs[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                *out = {pos, pos+offset};
                out++;
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == bishop){
        square dirs[] = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                *out = {pos, pos+offset};
                out++;
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == queen){
        square dirs[] = {{1, 0}, {1, 1}, {1, -1}, {0, 1},
                         {0, -1}, {-1, 0}, {-1, 1}, {-1, -1}};
        for(square dir : dirs){
            square offset = dir;
            while(isMovable(atSquare(pos+offset))){
                *out = {pos, pos+offset};
                out++;
                if(isMovable(atSquare(pos+offset)) == capture) break;
                offset = offset + dir;
            }
        }
    }
    if(type == knight){
        square dirs[] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                         {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};
        for(square dir : dirs){
            if(isMovable(atSquare(pos + dir))){
                *out = {pos, pos + dir};
                out++;
            }
        }
    }
    return out;
}
