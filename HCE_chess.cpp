#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>

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

// check if a move is legal
// remember to also make it check if the king is under check
bool check_move(std::vector<std::vector<int>> board, int piece, std::vector<int> move, bool check_king = true) {
    int start = move[0];
    int destination = move[1];

    if (piece > 5 && piece < 12)
        std::cout << "check_move: piece index is above 5, which will result in check_move automatically returning False" << std::endl;
    
    std::vector<std::vector<int>> occupied; // it's not stupid if it works
    for (int i = 0; i < board[0].size(); ++i) {
        if ((board[0][i] + board[1][i] + board[2][i] + board[3][i] + board[4][i] + board[5][i]) > 0)
            occupied[0].push_back(1);
        else
            occupied[0].push_back(0);
    }
    for (int i = 0; i < board[0].size(); ++i) {
        if ((board[6][i] + board[7][i] + board[8][i] + board[9][i] + board[10][i] + board[11][i]) > 0)
            occupied[1].push_back(1); // idk why I wrote it like this
        else
            occupied[1].push_back(0);
    }

    if (start < 0 || start > 63) {
        // std::cout << "check_move: start index is outside of board" << std::endl;
        return false;
    }
    if (destination < 0 || destination > 63) {
        // std::cout << "check_move: destination index is outside of board" << std::endl;
        return false;
    }

    // determine piece color
    int self_index = 0; // for occupied[]
    int opponent_index = 1;
    if (occupied[0][start] == 1 && occupied[1][start] == 1) {
        // std::cout << "check_move: start position is occupied by both white and black" << std::endl;
        return false;
    } else if (occupied[0][start] == 0 && occupied[1][start] == 0) {
        // std::cout << "check_move: start position is empty" << std::endl;
        return false;
    } else if (occupied[1][start] == 1) {
        self_index = 1;
        opponent_index = 0;
    }

    if (occupied[self_index][destination] == 1) {
        // std::cout << "check_move: position is already occupied by own piece" << std::endl;
        return false;
    }

    if (check_king) {
        if (check(make_move(board, move, false)) == 3) {
            // std::cout << "check_move: \"I won, but at what cost\" ahhh move" << std::endl;
            return false;
        }
        if (self_index == 0) { // player is white
            if (check(make_move(board, move, false)) == -1) {
                // std::cout << "check_move: white king is under check" << std::endl;
                return false;
            }
        } else {
            if (check(make_move(board, move, false)) == 1) {
                // std::cout << "check_move: black king is under check" << std::endl;
                return false;
            }
        }
    }

    int start_row = (int)(start / 8);
    int start_col = start % 8;
    int dest_row = (int)(destination / 8);
    int dest_col = destination % 8;
    int row_diff = std::abs(start_row - dest_row);
    int col_diff = std::abs(start_col - dest_col);
    int interval;
    if (col_diff != 0)
        interval = (int)((destination - start) / col_diff);
    else if (row_diff != 0)
        interval = (int)((destination - start) / row_diff);
    else {
        // std::cout << "check_move: start position and destination are the same" << std::endl;
        return false;
    }

    if (piece == 0) {
        if (col_diff == 0) { // no attempted capturing
            if (occupied[opponent_index][destination] == 1) {
                // std::cout << "check_move: pawn cannot capture forward" << std::endl;
                return false;
            }
            if (self_index == 0) {
                if (start >= 48 && start <= 55) {
                    if (destination == (start - 16) && occupied[self_index][start - 8] == 0 && occupied[opponent_index][start - 8] == 0)
                        return true;
                }
                if (destination == (start - 8)) // don't use else if bc both conditions should be checked
                    return true;
                else {
                    // std::cout << "check_move: not a valid pawn move 1" << std::endl;
                    return false;
                }
            } else if (self_index == 1) {
                if (start >= 8 && start <= 15) {
                    if (destination == (start + 16) && occupied[self_index][start + 8] == 0 && occupied[opponent_index][start + 8] == 0)
                        return true;
                }
                if (destination == (start + 8)) // don't use else if bc both conditions should be checked
                    return true;
                else {
                    // std::cout << "check_move: not a valid pawn move 2" << std::endl;
                    return false;
                }
            }
        } else if (col_diff == 1) { // attempt to capture a piece
            if (self_index == 0) {
                if (row_diff == 1 && (start_row - dest_row) > 0) {
                    if (occupied[opponent_index][destination] == 1)
                        return true;
                }
                // std::cout << "check_move: not a valid pawn move 3" << std::endl;
                return false;
            } else if (self_index == 1) {
                if (row_diff == 1 && (start_row - dest_row) < 0) {
                    if (occupied[opponent_index][destination] == 1)
                        return true;
                }
                // std::cout << "check_move: not a valid pawn move 4" << std::endl;
                return false;
            } else {
                // std::cout << "check_move: not a valid pawn move 5" << std::endl;
                return false;
            }
        }
    } else if (piece == 1) {
        if (row_diff == col_diff) {
            for (int i = start + interval; i < destination; i += interval) {
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: bishop cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else {
            // std::cout << "check_move: not a valid bishop move" << std::endl;
            return false;
        }
        return true;
    } else if (piece == 2) {
        if (row_diff == 1 && col_diff == 2)
            return true;
        if (row_diff == 2 and col_diff == 1)
            return true;
        // std::cout << "check_move: not a valid knight move" << std::endl;
        return false;
    } else if (piece == 3) {
        if (start_row == dest_row) {
            for (int i = start + interval; i < destination; i += interval) {
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: rook cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else if (start_col == dest_col) {
            for (int i = start + interval; i < destination; i += interval) { // interval uses row_diff here
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: rook cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else {
            // std::cout << "check_move: not a valid rook move" << std::endl;
            return false;
        }
        return true;
    } else if (piece == 4) {
        ;
    }
}

std::vector<std::vector<int>> make_move(std::vector<std::vector<int>> board, std::vector<int> move, bool c_move = true) {
    int piece = -1;
    for (int piece_type = 0; piece_type < board.size(); ++piece_type) {
        if ((board[piece_type][move[0]]) == 1) { // this condition shouldn't ever be True more than once
            if (piece != -1) {
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
    /* if (c_move) {
        if (!check_move(board, piece % 6, move)) { // check_move doesn't recognize pieces numbers > 5
            // std::cout << "make_move: check_move returned False" << std::endl;
            return board;
        }
    } */
    board[piece][move[0]] = 0;
    for (int piece_type = 0; piece_type < board.size(); ++piece_type)
        board[piece_type][move[1]] = 0; // remove (capture) any enemy pieces
    if (piece == 0 && move[1] >= 0 && move[1] <= 7) // promote white pawn to queen
        board[4][move[1]] = 1;
    else if (piece == 6 && move[1] >= 56 && move[1] <= 63) // promote black pawn to queen
        board[10][move[1]] = 1;
    else
        board[piece][move[1]] = 1;
    // send move data to GUI
    return board;
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
    board = make_move(board, {52, 36});
    print_board(board);
    return 0;
}