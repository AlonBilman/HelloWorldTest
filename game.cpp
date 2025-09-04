#include "game.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

Game::Game() { board.reset(); }

static constexpr int kMinMoveChars = 4;
static constexpr int kPromoIndex = 4;
static constexpr char kFileA = 'a';
static constexpr char kRank1 = '1';

static inline int fileToCol(char f) { return f - kFileA; }
static inline int rankToRow(char r) { return 7 - (r - kRank1); }
static inline bool validFile(char f) { return f >= 'a' && f <= 'h'; }
static inline bool validRank(char r) { return r >= '1' && r <= '8'; }
static inline std::string coordToStr(int r, int c) {
    return std::string{char(kFileA + c), char(kRank1 + (7 - r))};
}
static inline std::string moveToStr(const Move &m) {
    std::string s = coordToStr(m.from_r, m.from_c) + coordToStr(m.to_r, m.to_c);
    if (m.promotion) s.push_back(char(std::tolower(static_cast<unsigned char>(m.promotion))));
    return s;
}
static inline std::string toLowerCopy(const std::string &s) {
    std::string out; out.reserve(s.size());
    for (unsigned char c : s) out.push_back(char(std::tolower(c)));
    return out;
}
static inline std::string trimCopy(const std::string &s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}
static inline const char *colorName(Color c) { return (c==Color::WHITE) ? "White" : "Black"; }

bool Game::parseMoveStr(const std::string &s, Move &m) const {
    std::string t; t.reserve(s.size());
    for (unsigned char ch : s) {
        if (std::isalpha(ch) || std::isdigit(ch) || ch=='-' || ch==' ') t.push_back(char(std::tolower(ch)));
    }
    t.erase(std::remove(t.begin(), t.end(), ' '), t.end());
    t.erase(std::remove(t.begin(), t.end(), '-'), t.end());
    if ((int)t.size() < kMinMoveChars) return false;
    const char f1 = t[0], r1 = t[1], f2 = t[2], r2 = t[3];
    if (!(validFile(f1) && validFile(f2) && validRank(r1) && validRank(r2))) return false;
    m.from_c = fileToCol(f1); m.from_r = rankToRow(r1);
    m.to_c   = fileToCol(f2); m.to_r   = rankToRow(r2);
    m.promotion = 0;
    if ((int)t.size() > kPromoIndex) {
        const char p = t[kPromoIndex];
        if (p=='q'||p=='r'||p=='b'||p=='n') m.promotion = p;
    }
    return true;
}

static void printLegalMoves(const std::vector<Move> &legal) {
    std::cout << "Legal moves (" << legal.size() << "): ";
    bool first = true;
    for (const auto &mv : legal) {
        if (!first) std::cout << ", ";
        first = false;
        std::cout << moveToStr(mv);
    }
    std::cout << '\n';
}

void Game::run() {
    std::cout << "CLI Chess — simple, local 1v1.\n";
    std::cout << "Rules: no castling, no en-passant. Moves like e2e4 or e7-e8q for promotion.\n";
    std::cout << "Commands: 'moves', 'board', 'help', 'quit'\n\n";
    std::cout << "Press Enter to start...\n"; std::string dummy; std::getline(std::cin, dummy);

    board.print();

    while (true) {
        Color tm = board.sideToMove;
        const auto legal = board.generateMoves(tm);
        if (legal.empty()) {
            if (board.isInCheck(tm)) std::cout << colorName(tm) << " is checkmated.\n";
            else std::cout << "Stalemate.\n";
            break;
        }

        std::cout << colorName(tm) << " to move. (type a move, 'moves', 'board', or 'help')\n> ";
    std::string line; if (!std::getline(std::cin, line)) break;
        const std::string cmd = toLowerCopy(trimCopy(line));
        if (cmd.empty()) continue;
        if (cmd == "quit" || cmd == "exit") break;
        if (cmd == "help") {
            std::cout << "Help: Enter moves like e2e4 or e7-e8q.\nType 'moves' to list legal moves, 'board' to redraw the board, 'quit' to exit.\n";
            continue;
        }
        if (cmd == "board") { board.print(); continue; }
        if (cmd == "moves") { printLegalMoves(legal); continue; }

        Move m;
        if (!parseMoveStr(cmd, m)) { std::cout << "Invalid format. Use e2e4 or e7-e8q, or type 'help'.\n"; continue; }

        bool applied = false;
        for (const auto &mv : legal) {
            if (mv.from_r==m.from_r && mv.from_c==m.from_c && mv.to_r==m.to_r && mv.to_c==m.to_c) {
                Move toPlay = mv;
                if (toPlay.promotion && m.promotion) toPlay.promotion = m.promotion;
                board.makeMove(toPlay);
                applied = true;
                break;
            }
        }
        if (!applied) { std::cout << "Illegal move. Type 'moves' to see legal moves.\n"; continue; }
        board.print();
    }
    std::cout << "Game over.\n";
}
