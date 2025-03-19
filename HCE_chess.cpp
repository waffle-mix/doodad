#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#define PAWN_VALUE = 1
#define BISHOP_VALUE = 3
#define KNIGHT_VALUE = 3
#define ROOK_VALUE = 5
#define QUEEN_VALUE = 9
#define CHECK_BIAS = 12



/* float evaluate(std::vector<std::vector<int>> board) {
    float white_score = count_ones(board[0]);
    return 0.1;
} */

std::vector<std::vector<int>> make_move(std::vector<std::vector<int>> board, std::vector<int> move, bool c_move = true) {
    int piece = -1;
    for (int piece_type = 0; piece_type < board.size(); ++piece_type) {
        if ((board[piece_type][move[0]]) == 1) { // this condition shouldn't ever be True more than once
            if (piece != 1) {
                // std::cout << "make_move: multiple piece types/colors occupy the same square" << std::endl;
                return board;
            }
            piece = piece_type;
        }
    }
    if (piece == -1) {
        // std::cout << "make_move: start square is empty" << std::endl;
        return board;
    }
    if (c_move) {
        if (!check_move(board, piece % 6, move)) { // check_move doesn't recognize pieces numbers > 5
            // std::cout << "make_move: check_move returned False" << std::endl;
            return board;
        }
    }
    board[piece][move[0]] = 0;
    for (int piece_type = 0; piece_type < board.size(); ++piece_type) {
        board[]
    }
}

int count_ones(std::vector<int> bitboard) {
    int count = 0;
    for (int square : bitboard) {
        if (square == 1) {
            count += square;
        }
    }
    return count;
}

std::vector<std::vector<int>> blank_board() {
    std::vector<int> bitboard;
    for (int i = 0; i < 64; ++i)
        bitboard.push_back(0);
    std::vector<std::vector<int>> board;
    for (int i = 0; i < 12; ++i)
        board.push_back(bitboard);
    return board;
}

std::vector<std::vector<int>> new_board() {
    std::vector<std::vector<int>> board = blank_board();
    std::vector<std::vector<int>> piece_placements = {
        {48, 49, 50, 51, 52, 53, 54, 55},
        {58, 61},
        {57, 62},
        {56, 63},
        {59},
        {60},
        {8, 9, 10, 11, 12, 13, 14, 15},
        {2, 5},
        {1, 6},
        {0, 7},
        {3},
        {4}
    };
    for (int piece = 0; piece < piece_placements.size(); ++piece) {
        for (int index : piece_placements[piece]) {
            board[piece][index] = 1;
        }
    }
    return board;
}

void print_board(std::vector<std::vector<int>> board) {
    std::vector<char> printed_board;
    for (int i = 0; i < 64; ++i)
        printed_board.push_back('_');
    for (int i = 0; i < board[0].size(); ++i) {
        for (int j = 0; j < board.size(); ++j) {
            if (board[j][i] == 1) {
                switch (j) {
                    case 0:
                        printed_board[i] = 'P';
                        continue;
                    case 1:
                        printed_board[i] = 'B';
                        continue;
                    case 2:
                        printed_board[i] = 'N';
                        continue;
                    case 3:
                        printed_board[i] = 'R';
                        continue;
                    case 4:
                        printed_board[i] = 'Q';
                        continue;
                    case 5:
                        printed_board[i] = 'K';
                        continue;
                    case 6:
                        printed_board[i] = 'p';
                        continue;
                    case 7:
                        printed_board[i] = 'b';
                        continue;
                    case 8:
                        printed_board[i] = 'n';
                        continue;
                    case 9:
                        printed_board[i] = 'r';
                        continue;
                    case 10:
                        printed_board[i] = 'q';
                        continue;
                    case 11:
                        printed_board[i] = 'k';
                        continue;
                }
            }
        }
    }
    for (int i = 0; i < printed_board.size(); ++i) {
        std::cout << printed_board[i] << " ";
        if (((i + 1) % 8) == 0)
            std::cout << std::endl;
    }
}

std::string id_piece(int piece) {
    switch (piece) {
        case 0:
            return "white_pawn";
        case 1:
            return "white_bishop";
        case 2:
            return "white_knight";
        case 3:
            return "white_rook";
        case 4:
            return "white_queen";
        case 5:
            return "white_king";
        case 6:
            return "black_pawn";
        case 7:
            return "black_bishop";
        case 8:
            return "black_knight";
        case 9:
            return "black_rook";
        case 10:
            return "black_queen";
        case 11:
            return "black_king";
        default:
            return "unknown_piece";
    }
}

int get_piece(std::vector<std::vector<int>> board, int index) {
    int piece = -1;
    for (int piece_type = 0; piece_type < board.size(); ++piece_type) {
        if (board[piece_type][index] == 1) // this condition shouldn't ever be True more than once
            piece = piece_type;
    }
    return piece;
}

int main() {
    std::vector<std::vector<int>> board = new_board();
    print_board(board);
    return 0;
}