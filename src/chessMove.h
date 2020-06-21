//
// Created by 16182 on 6/20/2020.
//

#ifndef CHESS_CHESSMOVE_H
#define CHESS_CHESSMOVE_H

struct square {
    char rank;
    char file;
    square(): rank(-1), file(-1){}
    square(char rank, char file): rank(rank), file(file){}
    square operator+(square s) const{
        return square(rank + s.rank, file + s.file);
    }
    bool operator==(square sq) const {
        return rank == sq.rank && file == sq.file;
    }
};

struct chessMove {
    square start;
    square end;
    bool operator==(chessMove m) const {
        return start == m.start && end == m.end;
    }
};


#endif //CHESS_CHESSMOVE_H
