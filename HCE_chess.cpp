#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#define PAWN_VALUE 1
#define BISHOP_VALUE 3
#define KNIGHT_VALUE 3
#define ROOK_VALUE 5
#define QUEEN_VALUE 9
#define CHECK_BIAS 12

int count_ones(std::vector<int> bitboard);
int check(std::vector<std::vector<int>> board);
std::vector<std::vector<int>> legal_moves(std::vector<std::vector<int>> board, bool white_turn);
std::vector<std::vector<int>> make_move(std::vector<std::vector<int>> board, std::vector<int> move, bool c_move = true);
bool threatened(std::vector<std::vector<int>> board, int index, bool white_player);
bool check_move(std::vector<std::vector<int>> board, int piece, std::vector<int> move, bool check_king = true, bool ignore_own = false);
std::vector<int> piece_moves(int piece, int index);
int id_piece_char(char c);
std::vector<std::vector<int>> blank_board();
bool fen_stm(std::string fen);
std::string notation_conv(std::vector<int> move);

float evaluate(std::vector<std::vector<int>> board) {
    float white_score = count_ones(board[0]) * PAWN_VALUE + count_ones(board[1]) * BISHOP_VALUE + count_ones(board[2]) * KNIGHT_VALUE + count_ones(board[3]) * ROOK_VALUE + count_ones(board[4]) * QUEEN_VALUE;
    float black_score = count_ones(board[6]) * PAWN_VALUE + count_ones(board[7]) * BISHOP_VALUE + count_ones(board[8]) * KNIGHT_VALUE + count_ones(board[9]) * ROOK_VALUE + count_ones(board[10]) * QUEEN_VALUE;
    return (white_score - black_score + check(board) * CHECK_BIAS);
}

float negamax_depth(std::vector<std::vector<int>> board, int depth, float alpha, float beta, bool white_player) {
    float best_eval = INT_MIN;  
    if (depth <= 0) {
        if (white_player)
            return evaluate(board);
        else
            return -evaluate(board);
    }
    std::vector<std::vector<int>> moves = legal_moves(board, white_player);
    if (moves.size() == 0) {
        if (check(board) == 0)
            return 0; // stalemate
        return best_eval; // checkmate
    }
    for (std::vector<int> move : moves) {
        float eval = -negamax_depth(make_move(board, move, false), depth - 1, -beta, -alpha, !white_player);
        if (eval > best_eval)
            best_eval = eval;
        alpha = std::fmax(alpha, best_eval);
        if (alpha >= beta)
            break;
    }
    return best_eval;
}

std::vector<int> search(int algorithm, std::vector<std::vector<int>> board, int limit, bool white_player) {
    std::vector<int> best_move;
    
    if (algorithm == 4) {
        if (limit <= 0)
            return best_move;
        std::vector<std::vector<int>> moves = legal_moves(board, white_player);
        if (moves.size() == 0) { // game is over
            return best_move;
        }
        float best_eval = INT_MIN;
        for (std::vector<int> move : moves) {
            float eval = -negamax_depth(make_move(board, move, false), limit - 1, INT_MIN, INT_MAX, !white_player);
            if (eval > best_eval) {
                best_move = move;
                best_eval = eval;
            }
        }
    }
    return best_move;
}

// convert a FEN string to board state
std::vector<std::vector<int>> fen_conv(std::string fen) {
    std::vector<char> board_string;
    for (char c : fen) {
        if (c == ' ')
            break;
        if (c == '/')
            continue;
        if (c > 48 && c <= 56) { // '0' shouldn't appear in the FEN string so c won't ever equal 48
            int length = c - 48;
            for (int i = 0; i < length; ++i)
                board_string.push_back('_');
            continue;
        }
        board_string.push_back(c);
    }
    // std::string str(board_string.begin(), board_string.end());
    // std::cout << str << std::endl;
    std::vector<std::vector<int>> board = blank_board();
    for (int i = 0; i < board_string.size(); ++i) {
        if (board_string[i] == '_')
            continue;
        board[id_piece_char(board_string[i])][i] = 1;
    }
    return board;
}

// get side-to-move from FEN string
bool fen_stm(std::string fen) {
    bool reached_space = false;
    for (char c: fen) {
        if (reached_space == true) {
            if (c == 'w')
                return true; // white to move
            break;
        }
        if (c == ' ')
            reached_space = true;
    }
    return false; // black to move
}

// convert move data in index form to a long algebraic notation string
std::string notation_conv(std::vector<int> move) {
    std::vector<std::string> table = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};
    return (table[move[0]] + table[move[1]]);
}

// convert a long algebraic notation string to move data
std::vector<int> move_conv(std::string str) {
    std::string start = str.substr(0, 2);
    std::string dest = str.substr(2, 4);
    std::vector<int> move;
    std::vector<std::string> table = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};
    for (int i = 0; i < 64; ++i) {
        if (start == table[i])
            move.push_back(i);
    }
    for (int i = 0; i < 64; ++i) {
        if (dest == table[i])
            move.push_back(i);
    }
    return move;
}

int check(std::vector<std::vector<int>> board) {
    // if white is checking black: 1
    // if black is checking white: -1
    // if somehow both kings are under check: 2
    // if nobody is under check: 0

    int wking_index = 0;
    int bking_index = 0;
    for (int square = 0; square < 64; ++square) {
        if (board[5][square] == 1) {
            wking_index = square;
            break;
        }
    }
    for (int square = 0; square < 64; ++square) {
        if (board[11][square] == 1) {
            bking_index = square;
            break;
        }
    }
    if (!threatened(board, wking_index, true) && !threatened(board, bking_index, false))
        return 0;
    if (threatened(board, wking_index, true) && threatened(board, bking_index, false))
        return 2;
    if (threatened(board, wking_index, true))
        return -1;
    if (threatened(board, bking_index, false))
        return 1;
    return 0;
}

// check if a square is being threatened by the enemy
bool threatened(std::vector<std::vector<int>> board, int index, bool white_player) {
    if (white_player) {
        for (int piece = 6; piece < 12; ++piece) {
            for (int square = 0; square < 64; ++square) {
                if (board[piece][square] == 1) {
                    int old = board[0][index];
                    board[0][index] = 1; // to test if pawns threaten it
                    if (check_move(board, piece % 6, {square, index}, false, true))
                        return true;
                    else
                        board[0][index] = old;
                }
            }
        }
    } else {
        for (int piece = 0; piece < 6; ++piece) {
            for (int square = 0; square < 64; ++square) {
                if (board[piece][square] == 1) {
                    int old = board[6][index];
                    board[6][index] = 1; // to test if pawns threaten it
                    if (check_move(board, piece % 6, {square, index}, false, true))
                        return true;
                    else
                        board[6][index] = old;
                }
            }
        }
    }
    return false;
}

// returns list of legal moves
std::vector<std::vector<int>> legal_moves(std::vector<std::vector<int>> board, bool white_turn) {
    std::vector<std::vector<int>> moves; // list of legal moves
    if (check(board) == 2 || check(board) == -2) {
        // std::cout << "legal_moves: game is over" << std::endl;
        return moves;
    }
    if (white_turn) {
        for (int piece = 0; piece < 6; ++piece) {
            for (int i = 0; i < 64; ++i) { // look for a piece to move
                if (board[piece][i] == 1) { // a piece is selected
                    std::vector<int> dests = piece_moves(piece, i);
                    for (int dest : dests) {
                        if (check_move(board, piece, {i, dest}))
                            moves.push_back({i, dest});
                    }
                }
            }
        }
    } else {
        for (int piece = 0; piece < 6; ++piece) {
            for (int i = 0; i < 64; ++i) { // look for a piece to move
                if (board[piece + 6][i] == 1) { // a piece is selected
                    std::vector<int> dests = piece_moves(piece, i);
                    for (int dest : dests) {
                        if (check_move(board, piece, {i, dest}))
                            moves.push_back({i, dest});
                    }
                }
            }
        }
    }
    return moves;
}

// returns list of potentially legal piece destinations. this will prevent check_move from being called unnecessarily
std::vector<int> piece_moves(int piece, int index) {
    std::vector<int> dests;
    if (piece == 0) {
        dests = {index - 16, index - 9, index - 8, index - 7, index + 7, index + 8, index + 9, index + 16};
    } else if (piece == 1) {
        int nw = std::min(index % 8, (int)(index / 8));
        int ne = std::min(7 - (index % 8), (int)(index / 8));
        int sw = std::min(index % 8, 7 - (int)(index / 8));
        int se = std::min(7 - (index % 8), 7 - (int)(index / 8));
        for (int i = 1; i <= nw; ++i)
            dests.push_back(index - 9 * i);
        for (int i = 1; i <= ne; ++i)
            dests.push_back(index - 7 * i);
        for (int i = 1; i <= sw; ++i)
            dests.push_back(index + 7 * i);
        for (int i = 1; i <= se; ++i)
            dests.push_back(index + 9 * i);
    } else if (piece == 2) {
        dests = {index - 17, index - 15, index - 10, index - 6, index + 6, index + 10, index + 15, index + 17};
        return dests;
    } else if (piece == 3) {
        int n = (int)(index / 8);
        int e = 7 - (index % 8);
        int s = 7 - (int)(index / 8);
        int w = index % 8;
        for (int i = 1; i <= n; ++ i)
            dests.push_back(index - 8 * i);
        for (int i = 1; i <= e; ++ i)
            dests.push_back(index + i);
        for (int i = 1; i <= s; ++ i)
            dests.push_back(index + 8 * i);
        for (int i = 1; i <= w; ++ i)
            dests.push_back(index - i);
    } else if (piece == 4) {
        int n = (int)(index / 8);
        int e = 7 - (index % 8);
        int s = 7 - (int)(index / 8);
        int w = index % 8;
        int nw = std::min(n, w);
        int ne = std::min(n, e);
        int sw = std::min(s, w);
        int se = std::min(s, e);
        for (int i = 1; i <= n; ++ i)
            dests.push_back(index - 8 * i);
        for (int i = 1; i <= e; ++ i)
            dests.push_back(index + i);
        for (int i = 1; i <= s; ++ i)
            dests.push_back(index + 8 * i);
        for (int i = 1; i <= w; ++ i)
            dests.push_back(index - i);
        for (int i = 1; i <= nw; ++i)
            dests.push_back(index - 9 * i);
        for (int i = 1; i <= ne; ++i)
            dests.push_back(index - 7 * i);
        for (int i = 1; i <= sw; ++i)
            dests.push_back(index + 7 * i);
        for (int i = 1; i <= se; ++i)
            dests.push_back(index + 9 * i);
    } else if (piece == 5) {
        dests = {index - 9, index - 8, index - 7, index - 1, index + 1, index + 7, index + 8, index + 9};
    } else {
        std::cout << "piece_moves: piece index is above 5, which will result in piece_moves automatically returning all squares" << std::endl;
        for (int i = 0; i < 64; )
            dests.push_back(i);
    }
    return dests;
}

// check if a move is legal
bool check_move(std::vector<std::vector<int>> board, int piece, std::vector<int> move, bool check_king, bool ignore_own) {
    int start = move[0];
    int destination = move[1];

    if (piece > 5 && piece < 12)
        std::cout << "check_move: piece index is above 5, which will result in check_move automatically returning False" << std::endl;
    
    std::vector<int> white_occupied;
    std::vector<int> black_occupied;
    std::vector<std::vector<int>> occupied = {white_occupied, black_occupied}; // it's not stupid if it works
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

    if (ignore_own == false) {
        if (occupied[self_index][destination] == 1) {
            // std::cout << "check_move: position is already occupied by own piece" << std::endl;
            return false;
        }
    }

    if (check_king) {
        if (piece == 5) { // might help speed up some stuff
            std::vector<std::vector<int>> new_board = board;
            new_board[5][move[0]] = 0; // to prevent the king from "blocking" the check
            if (self_index == 0) { // white
                if (threatened(new_board, destination, true)) {
                    // std::cout << "check_move: white king cannot move into a check" << std::endl;
                    return false;
                }
            } else {
                if (threatened(new_board, destination, false)) {
                    // std::cout << "check_move: black king cannot move into a check" << std::endl;
                    return false;
                }
            }
        } else {
            if (check(make_move(board, move, false)) == 2) {
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
            for (int i = start + interval; i != destination; i += interval) {
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
            for (int i = start + interval; i != destination; i += interval) {
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: rook cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else if (start_col == dest_col) {
            for (int i = start + interval; i != destination; i += interval) { // interval uses row_diff here
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
        if (start_row == dest_row) {
            for (int i = start + interval; i != destination; i += interval) {
                if (occupied[0][i] == 1 or occupied[1][i] == 1) {
                    // std::cout << "check_move: queen cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else if (start_col == dest_col) {
            for (int i = start + interval; i != destination; i += interval) { // interval uses row_diff here
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: queen cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else if (row_diff == col_diff) {
            for (int i = start + interval; i != destination; i += interval) {
                if (occupied[0][i] == 1 || occupied[1][i] == 1) {
                    // std::cout << "check_move: queen cannot move through other pieces" << std::endl;
                    return false;
                }
            }
        } else {
            // std::cout << "check_move: not a valid queen move" << std::endl;
            return false;
        }
        return true;
    } else if (piece == 5) {
        if (row_diff <= 1 && col_diff <= 1)
            return true; // check_move already checks if the destination square is threatened before this line of code
        else {
            // std::cout << "check_move: not a valid king move" << std::endl;
            return false;
        }
    } else {
        // std::cout << "check_move: piece type " << piece << " is invalid" << std::endl;
        return false;
    }
    // std::cout << "check_move: reached end of function" << std::endl;
    return false;
}

std::vector<std::vector<int>> make_move(std::vector<std::vector<int>> board, std::vector<int> move, bool c_move) {
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
    if (c_move) {
        if (!check_move(board, piece % 6, move)) { // check_move doesn't recognize pieces numbers > 5
            // std::cout << "make_move: check_move returned False" << std::endl;
            return board;
        }
    }
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

// return a piece index based on a character
int id_piece_char(char c) {
    if (c == 'P')
        return 0;
    else if (c == 'B')
        return 1;
    else if (c == 'N')
        return 2;
    else if (c == 'R')
        return 3;
    else if (c == 'Q')
        return 4;
    else if (c == 'K')
        return 5;
    else if (c == 'p')
        return 6;
    else if (c == 'b')
        return 7;
    else if (c == 'n')
        return 8;
    else if (c == 'r')
        return 9;
    else if (c == 'q')
        return 10;
    else if (c == 'k')
        return 11;
    else
        return -1;
}

void move_debug(std::vector<std::vector<int>> board, bool white_player) {
    std::vector<std::vector<int>> moves = legal_moves(board, true);
    for (std::vector<int> move : moves) {
        std::cout << id_piece(get_piece(board, move[0])) << " from " << move[0] << " to " << move[1] << std::endl;
    }
}

// get move input from user
std::vector<int> get_move() {
    int start;
    int dest;
    std::cout << ": ";
    std::cin >> start;
    std::cout << ": ";
    std::cin >> dest;
    return {start, dest};
}

std::vector<std::string> split(std::string input, char delimiter = ' ') {
    std::vector<std::string> output = {{""}};
    int index = 0;
    for (char c : input) {
        if (c == delimiter) {
            output.push_back("");
            ++index;
            continue;
        }
        output[index] += c;
    }
    return output;
}

// main engine function
void uci() {
    bool white_turn = true;
    std::vector<std::vector<int>> board = new_board();
    std::string command;
    while (command != "quit") {
        std::getline(std::cin, command);
        if (command == "quit")
            break;
        else if (command == "uci") {
            std::cout << "id name doodad 1.0" << std::endl;
            std::cout << "id author Waffle_Mix" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (command == "isready")
            std::cout << "readyok" << std::endl;
        else if (command == "ucinewgame") {
            board = new_board();
            white_turn = true;
        } else if (split(command)[0] == "position") {
            std::vector<std::string> parsed = split(command);
            if (parsed[1] == "startpos") {
                board = new_board();
                white_turn = true;
                if (parsed[2] == "moves") {
                    for (int i = 3; i < parsed.size(); ++i) {
                        if (parsed[i] == "O-O") { // makeshift castling fix
                            if (white_turn) {
                                board = make_move(board, {60, 62}, false);
                                board = make_move(board, {63, 61}, false);
                                continue;
                            } else {
                                board = make_move(board, {4, 6}, false);
                                board = make_move(board, {7, 5}, false);
                                continue;
                            }
                        } else if (parsed[i] == "O-O-O") {
                            if (white_turn) {
                                board = make_move(board, {60, 58}, false);
                                board = make_move(board, {56, 59}, false);
                                continue;
                            } else {
                                board = make_move(board, {4, 2}, false);
                                board = make_move(board, {0, 3}, false);
                                continue;
                            }
                        }
                        board = make_move(board, move_conv(parsed[i]));
                        white_turn = !white_turn;
                    }
                }
            } else if (parsed[1] == "fen") {
                board = fen_conv(parsed[2]);
                if (parsed[3] == "w")
                    white_turn = true;
                else
                    white_turn = false;
            }
        } else if (split(command)[0] == "go") {
            std::vector<std::string> parsed = split(command);
            if (parsed[1] == "depth") {
                std::string move = notation_conv(search(4, board, parsed[2][0] - 48, white_turn));
                std::cout << "bestmove " << move << std::endl;
            }
        }
    }
}

int main() {
    uci();

    /* std::vector<std::vector<int>> board = new_board();
    bool white_turn = true;
    bool white_player = true;

    while (true) { // testing game loop
        print_board(board);
        std::cout << std::endl;
        std::vector<int> move;
        std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (white_turn) {
            if (white_player)
                move = get_move();
            else
                move = search(4, board, 2, white_turn);
            // move = {52, 36};
        } else {
            if (white_player)
                move = search(4, board, 2, white_turn);
            else
                move = get_move();
            // move = {11, 27};
        }
        std::time_t finish_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (move.size() == 0)
            break;
        std::cout << "elapsed time: " << (finish_time - start_time) << std::endl;
        std::cout << "move: " << notation_conv(move) << std::endl;
        int piece = get_piece(board, move[0]);
        if (!check_move(board, piece % 6, move)) {
            std::cout << "check_move returned False. try again." << std::endl;
            continue;
        }
        board = make_move(board, move);
        std::cout << "eval: " << evaluate(board) << std::endl;
        std::cout << "check: " << check(board) << std::endl;
        white_turn = !white_turn;
    }
    std::cout << "GAME OVER" << std::endl;
    std::string tmp;
    std::cout << "enter anything to quit: ";
    std::cin >> tmp; */
}