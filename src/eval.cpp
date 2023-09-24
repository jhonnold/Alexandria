#include "eval.h"
#include "board.h"
#include <algorithm>

const int piece_phase[] = { 0,3,3,5,10,0 };

int GetPhase(const S_Board* pos) {
    int knights = CountBits(GetPieceBB(pos, KNIGHT));
    int bishops = CountBits(GetPieceBB(pos, BISHOP));
    int rooks = CountBits(GetPieceBB(pos, ROOK));
    int queens = CountBits(GetPieceBB(pos, QUEEN));

    return knights * piece_phase[KNIGHT] + bishops * piece_phase[BISHOP] + rooks * piece_phase[ROOK] + queens * piece_phase[QUEEN];
}

// if we don't have enough material to mate consider the position a draw
bool MaterialDraw(const S_Board* pos) {
    // If we only have kings on the board then it's a draw
    if (pos->PieceCount() == 2)
        return true;
    // KN v K, KB v K
    else if (pos->PieceCount() == 3 && ((CountBits(GetPieceBB(pos, KNIGHT)) == 1) || (CountBits(GetPieceBB(pos, BISHOP)) == 1)))
        return true;
    // If we have 4 pieces on the board
    else if (pos->PieceCount() == 4) {
        // KNN v K, KN v KN
        if ((CountBits(GetPieceBB(pos, KNIGHT)) == 2))
            return true;
        // KB v KB
        else if (((CountBits(GetPieceBB(pos, BISHOP)) == 2)) && CountBits(pos->GetPieceColorBB( BISHOP, WHITE)) == 1)
            return true;
    }

    return false;
}

static inline float MaterialScale(const S_Board* pos) {
    return 700 + GetMaterialValue(pos) / 32;
}

// position evaluation
int EvalPosition(const S_Board* pos) {
    bool stm = (pos->side == WHITE);
    int eval = nnue.output(pos->accumulator, stm);
    eval = eval * (128 + GetPhase(pos)) / 128;
    eval = eval * (200 - pos->Get50mrCounter()) / 200;
    // Clamp eval to avoid it somehow being a mate score
    eval = std::clamp(eval, -mate_score + 1, mate_score - 1);
    return eval;
}
