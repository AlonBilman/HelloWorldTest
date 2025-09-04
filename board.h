#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <utility>
#include <cstdint>

static constexpr int BOARD_SIZE = 8;

enum class Color : uint8_t { WHITE, BLACK };

struct Move {
    int from_r = 0;
    int from_c = 0;
    int to_r = 0;
    int to_c = 0;
    char promotion = 0;
};

class Board {
public:
    Board();
    void reset();
    void print() const;
    bool onBoard(int row, int col) const;
    bool isWhitePiece(char piece) const;
    bool isBlackPiece(char piece) const;
    bool isEmpty(char piece) const;
    std::vector<Move> generateMoves(Color side) const;
    bool isInCheck(Color col) const;
    void makeMove(const Move &m);
    Color sideToMove{Color::WHITE};

private:
    using Row = std::array<char, BOARD_SIZE>;
    std::array<Row, BOARD_SIZE> squares{};
    bool findKing(Color col, int &kr, int &kc) const;
    bool isAttacked(int r, int c, Color byColor) const;
    void addPawnMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const;
    void addKnightMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const;
    void addKingMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const;
    void addSlidingMoves(int row, int col, bool pieceIsWhite, const std::vector<std::pair<int,int>> &dirs, std::vector<Move> &out) const;
    bool isOpponentPiece(char piece, bool pieceIsWhite) const;
};

#endif
