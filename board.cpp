#include "board.h"

#include <iostream>
#include <cctype>
#include <vector>

static constexpr std::pair<int,int> kKnightDirs[] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
static constexpr std::pair<int,int> kBishopDirs[] = {{-1,-1},{-1,1},{1,-1},{1,1}};
static constexpr std::pair<int,int> kRookDirs[] = {{-1,0},{1,0},{0,-1},{0,1}};
static constexpr std::pair<int,int> kKingDirs[] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

Board::Board() { reset(); }

void Board::reset() {
	const std::array<std::string, BOARD_SIZE> rows = {
		"rnbqkbnr",
		"pppppppp",
		"........",
		"........",
		"........",
		"........",
		"PPPPPPPP",
		"RNBQKBNR"
	};
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			squares[row][col] = rows[row][col];
		}
	}
	sideToMove = Color::WHITE;
}

void Board::print() const {
	std::cout << "  a b c d e f g h\n";
	for (int row = 0; row < BOARD_SIZE; ++row) {
		std::cout << (BOARD_SIZE - row) << ' ';
		for (int col = 0; col < BOARD_SIZE; ++col) {
			std::cout << squares[row][col] << ' ';
		}
		std::cout << (BOARD_SIZE - row) << '\n';
	}
	std::cout << "  a b c d e f g h\n";
	std::cout << (sideToMove == Color::WHITE ? "White to move" : "Black to move") << '\n';
}

bool Board::onBoard(int row, int col) const { 
	return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE; 
}
bool Board::isWhitePiece(char piece) const { 
	return piece >= 'A' && piece <= 'Z'; 
}
bool Board::isBlackPiece(char piece) const { 
	return piece >= 'a' && piece <= 'z'; 
}
bool Board::isEmpty(char piece) const { 
	return piece == '.'; 
}

bool Board::isOpponentPiece(char piece, bool pieceIsWhite) const {
	if (isEmpty(piece)) 
		return false;
	return pieceIsWhite ? isBlackPiece(piece) : isWhitePiece(piece);
}

void Board::addSlidingMoves(int row, int col, bool pieceIsWhite, const std::vector<std::pair<int,int>> &dirs, std::vector<Move> &out) const {
	for (const auto &d : dirs) {
		int dirRow = d.first;
		int dirCol = d.second;
		int nextRow = row + dirRow;
		int nextCol = col + dirCol;
		while (onBoard(nextRow, nextCol)) {
			//this walk along dir until hit piece or edge
			char targetPiece = squares[nextRow][nextCol];
			if (isEmpty(targetPiece)) {
				out.push_back(Move{row, col, nextRow, nextCol, 0});
			} else {
				if (isOpponentPiece(targetPiece, pieceIsWhite)) {
					out.push_back(Move{row, col, nextRow, nextCol, 0});
				}
				break;
			}
			nextRow += dirRow;
			nextCol += dirCol;
		}
	}
}

void Board::addKnightMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const {
	for (auto d : kKnightDirs) {
		int nextRow = row + d.first;
		int nextCol = col + d.second;
		if (!onBoard(nextRow, nextCol)) {
			continue;
		}
		char targetPiece = squares[nextRow][nextCol];
		//this knight jumps over pieces
		if (isEmpty(targetPiece) || isOpponentPiece(targetPiece, pieceIsWhite)) {
			out.push_back(Move{row, col, nextRow, nextCol, 0});
		}
	}
}

void Board::addKingMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const {
	for (auto d : kKingDirs) {
		int nextRow = row + d.first;
		int nextCol = col + d.second;
		if (!onBoard(nextRow, nextCol)) {
			continue;
		}
		char targetPiece = squares[nextRow][nextCol];
		//this king one-step moves
		if (isEmpty(targetPiece) || isOpponentPiece(targetPiece, pieceIsWhite)) {
			out.push_back(Move{row, col, nextRow, nextCol, 0});
		}
	}
}

void Board::addPawnMoves(int row, int col, bool pieceIsWhite, std::vector<Move> &out) const {
	int forward = pieceIsWhite ? -1 : 1;
	int startRow = pieceIsWhite ? 6 : 1;
	int oneStepRow = row + forward;
	int curCol = col;
	if (onBoard(oneStepRow, curCol) && isEmpty(squares[oneStepRow][curCol])) {
			//this pawns move forward (dir depends on color)
			Move m{row, col, oneStepRow, curCol, 0};
			//this auto promote to queen for simplicity
			if ((pieceIsWhite && oneStepRow == 0) || (!pieceIsWhite && oneStepRow == BOARD_SIZE - 1)) {
				m.promotion = 'q';
			}
			out.push_back(m);

			//this initial two-square move
			if (row == startRow) {
				int twoStepRow = row + 2 * forward;
				if (onBoard(twoStepRow, curCol) && isEmpty(squares[twoStepRow][curCol])) {
					out.push_back(Move{row, col, twoStepRow, curCol, 0});
				}
			}
		}
	for (int dcol : {-1, 1}) {
		int capRow = row + forward;
		int capCol = col + dcol;
		if (!onBoard(capRow, capCol)) {
			continue;
		}
		//this captures diagonally
		if (isOpponentPiece(squares[capRow][capCol], pieceIsWhite)) {
			Move m{row, col, capRow, capCol, 0};
			//this auto promote on capture to last rank
			if ((pieceIsWhite && capRow == 0) || (!pieceIsWhite && capRow == BOARD_SIZE - 1)) {
				m.promotion = 'q';
			}
			out.push_back(m);
		}
	}
}

std::vector<Move> Board::generateMoves(Color side) const {
	std::vector<Move> moves;
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			char piece = squares[row][col];
			if (isEmpty(piece)) {
				continue;
			}
			const bool pieceIsWhite = isWhitePiece(piece);
			if ((side == Color::WHITE && !pieceIsWhite) || (side == Color::BLACK && !isBlackPiece(piece))) {
				continue;
			}
			const char pieceLower = static_cast<char>(std::tolower(static_cast<unsigned char>(piece)));

			if (pieceLower == 'p') {
				addPawnMoves(row, col, pieceIsWhite, moves);
			} else if (pieceLower == 'n') {
				addKnightMoves(row, col, pieceIsWhite, moves);
			} else if (pieceLower == 'b') {
				addSlidingMoves(row, col, pieceIsWhite, std::vector<std::pair<int,int>>(kBishopDirs, kBishopDirs + 4), moves);
			} else if (pieceLower == 'r') {
				addSlidingMoves(row, col, pieceIsWhite, std::vector<std::pair<int,int>>(kRookDirs, kRookDirs + 4), moves);
			} else if (pieceLower == 'q') {
				std::vector<std::pair<int,int>> dirs;
				dirs.insert(dirs.end(), kBishopDirs, kBishopDirs + 4);
				dirs.insert(dirs.end(), kRookDirs, kRookDirs + 4);
				addSlidingMoves(row, col, pieceIsWhite, dirs, moves);
			} else if (pieceLower == 'k') {
				addKingMoves(row, col, pieceIsWhite, moves);
			}
		}
	}

    
	std::vector<Move> legal;
	for (auto mv : moves) {
		Board copy = *this;
		copy.makeMove(mv);
		//this filter out moves that leave own king in check
		if (!copy.isInCheck(side)) {
			legal.push_back(mv);
		}
	}
	return legal;
}

bool Board::findKing(Color col, int &kr, int &kc) const {
	const char want = (col == Color::WHITE ? 'K' : 'k');
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			if (squares[row][col] == want) {
				kr = row;
				kc = col;
				return true;
			}
		}
	}
	return false;
}

bool Board::isAttacked(int r, int c, Color byColor) const {
	if (byColor == Color::WHITE) {
		//this pawn attacks are from above for white
		int pawnDir = -1;
		for (int dcol : {-1, 1}) {
			int checkRow = r + pawnDir;
			int checkCol = c + dcol;
			if (onBoard(checkRow, checkCol) && squares[checkRow][checkCol] == 'P') {
				return true;
			}
		}
	} else {
		//this pawn attacks are from below for black
		int pawnDir = 1;
		for (int dcol : {-1, 1}) {
			int checkRow = r + pawnDir;
			int checkCol = c + dcol;
			if (onBoard(checkRow, checkCol) && squares[checkRow][checkCol] == 'p') {
				return true;
			}
		}
	}

	//this check knight attacks
	for (auto d : kKnightDirs) {
		int checkRow = r + d.first;
		int checkCol = c + d.second;
		if (!onBoard(checkRow, checkCol)) {
			continue;
		}
		char p = squares[checkRow][checkCol];
		if ((byColor == Color::WHITE && p == 'N') || (byColor == Color::BLACK && p == 'n')) {
			return true;
		}
	}

	//this sliding bishop/queen attackers
	for (auto d : kBishopDirs) {
		int checkRow = r + d.first;
		int checkCol = c + d.second;
		while (onBoard(checkRow, checkCol)) {
			char p = squares[checkRow][checkCol];
			if (!isEmpty(p)) {
				if ((byColor == Color::WHITE && (p=='B' || p=='Q')) || (byColor == Color::BLACK && (p=='b' || p=='q'))) {
					return true;
				} else {
					break;
				}
			}
			checkRow += d.first;
			checkCol += d.second;
		}
	}

	//this sliding rook/queen attackers
	for (auto d : kRookDirs) {
		int checkRow = r + d.first;
		int checkCol = c + d.second;
		while (onBoard(checkRow, checkCol)) {
			char p = squares[checkRow][checkCol];
			if (!isEmpty(p)) {
				if ((byColor == Color::WHITE && (p=='R' || p=='Q')) || (byColor == Color::BLACK && (p=='r' || p=='q'))) {
					return true;
				} else {
					break;
				}
			}
			checkRow += d.first;
			checkCol += d.second;
		}
	}

	//this adjacent king
	for (auto d : kKingDirs) {
		int checkRow = r + d.first;
		int checkCol = c + d.second;
		if (onBoard(checkRow, checkCol)) {
			char p = squares[checkRow][checkCol];
			if ((byColor == Color::WHITE && p == 'K') || (byColor == Color::BLACK && p == 'k')) {
				return true;
			}
		}
	}

	return false;
}

bool Board::isInCheck(Color col) const {
	int kingRow = -1, kingCol = -1; 
	if (!findKing(col, kingRow, kingCol)) return false;
	return isAttacked(kingRow, kingCol, (col == Color::WHITE ? Color::BLACK : Color::WHITE));
}

void Board::makeMove(const Move &m) {
	char piece = squares[m.from_r][m.from_c];
	squares[m.to_r][m.to_c] = piece;
	squares[m.from_r][m.from_c] = '.';
	if (m.promotion) {
		if (isWhitePiece(piece)) {
			squares[m.to_r][m.to_c] = 'Q';
		} else {
			squares[m.to_r][m.to_c] = 'q';
		}
	}
	sideToMove = (sideToMove == Color::WHITE ? Color::BLACK : Color::WHITE);
}

 
