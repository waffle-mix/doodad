import time
import copy

pawn_value = 1
bishop_value = 3
knight_value = 3
rook_value = 5
queen_value = 9

def engine():
    white_turn = True
    board = new_board()
    command = ""
    while command != "quit":
        command = input()
        if command == "quit":
            return
        elif command == "uci":
            print("id name doodad 1.0")
            print("id author Waffle_Mix")
            print("uciok")
        elif command == "isready":
            print("readyok")
        elif command == "ucinewgame":
            board = new_board()
        elif command.split()[0] == "position":
            if command.split()[1] == "startpos":
                board = new_board()
            elif command.split()[1] == "fen":
                board = fen_conv([command.split()[2], command.split()[3], command.split()[4]]) # only part of the FEN string is needed
                if command.split()[3] == "w":
                    white_turn == True
                else:
                    white_turn == False
        elif command.split()[0] == "go":
            if command.split()[1] == "movetime":
                movetime = int(int(command.split()[2]) / 1000) # movetime is in seconds
                # print(notation_conv(negamax_time(board, movetime, time.time(), white_turn)))

def evaluate(board):
    check_bias = 12 # eval bonus if you're checking your opponent
    white_score = count_ones(board[0]) * pawn_value + count_ones(board[1]) * bishop_value + count_ones(board[2]) * knight_value + count_ones(board[3]) * rook_value + count_ones(board[4]) * queen_value
    black_score = count_ones(board[6]) * pawn_value + count_ones(board[7]) * bishop_value + count_ones(board[8]) * knight_value + count_ones(board[9]) * rook_value + count_ones(board[10]) * queen_value
    return white_score - black_score + check(board) * check_bias

def negamax_depth(board, depth, alpha, beta, white_player):
    if depth <= 0:
        if white_player:
            return [], evaluate(board)
        else:
            return [], -evaluate(board)
    
    moves = legal_moves(board, white_player)
    if len(moves) == 0:
        if check(board) == 0:
            return [], 0 # stalemate
        return [], -1000 # checkmate
    best_move = []
    best_eval = -10000
    for move in moves:
        _, eval = negamax_depth(make_move(board, move), depth - 1, -beta, -alpha, not white_player)
        eval = -eval
        if eval > best_eval:
            best_move = move
            best_eval = eval
        alpha = max(alpha, best_eval)
        if alpha >= beta:
            break
    return best_move, best_eval

def search(algorithm, board, limit, white_player):
    '''if algorithm == 1:
        move, _, depth = minimax_time(board, limit, time.time(), white_player, 0)
        print(f"minimax_time depth = {depth}")
        return move
    elif algorithm == 2:
        move, _ = minimax_depth(board, limit, white_player)
        return move
    elif algorithm == 3:
        move, _, depth = negamax_time(board, limit, time.time(), -10000, 10000, white_player, 0)
        print(f"negamax_time depth = {depth}")
        return move'''
    if algorithm == 4:
        move, _ = negamax_depth(board, limit, -10000, 10000, white_player)
        return move
    '''else:
        print("search: invalid algorithm choice, defaulting to negamax_time")
        move, _, depth = negamax_time(board, limit, time.time(), -10000, 10000, white_player, 0)
        print(f"negamax_time depth = {depth}")
        return move'''

# convert a FEN string to board
def fen_conv(fen):
    return 0

# convert move data in index form to a long algebraic notation string
def notation_conv(move):
    table = [
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
    ]
    return table[move[0]] + table[move[1]]

# check if the a king is under check or checkmate
def check(board):
    # if white is checking black: 1
    # if black is checking white: -1
    # if nothing: 0
    # if somehow both kings are under check: 3

    wking_index = 0
    bking_index = 0
    for square in range(len(board[5])):
        if board[5][square] == 1:
            wking_index = square
            break
    for square in range(len(board[11])):
        if board[11][square] == 1:
            bking_index = square
            break
    if not threatened(board, wking_index, True) and not threatened(board, bking_index, False):
        return 0
    if threatened(board, wking_index, True) and threatened(board, bking_index, False):
        return 3
    if threatened(board, wking_index, True):
        return -1
    if threatened(board, bking_index, False):
        return 1

# check if a square is being threatened by the enemy
def threatened(board, index, white_player):
    new_board = copy.deepcopy(board)
    if white_player:
        for piece in range(6, 12):
            for square in range(len(board[piece])):
                if board[piece][square] == 1:
                    new_board[0][index] = 1 # to test if pawns threaten it
                    if check_move(new_board, piece % 6, [square, index], False):
                        return True
                    else:
                        new_board[0][index] = board[0][index]
    else:
        for piece in range(6):
            for square in range(len(board[piece])):
                if board[piece][square] == 1:
                    new_board[6][index] = 1 # to test if pawns threaten it
                    if check_move(new_board, piece % 6, [square, index], False):
                        return True
                    else:
                        new_board[6][index] = board[6][index]
    return False

# returns list of legal moves
def legal_moves(board, white_turn):
    if check(board) == 2 or check(board) == -2:
        # print("legal_moves: game is over")
        return []
    occupied = [] # if 1 then the space is occupied by a piece
    for i in range(len(board[0])):
        if (board[0][i] + board[1][i] + board[2][i] + board[3][i] + board[4][i] + board[5][i] + board[6][i] + board[7][i] + board[8][i] + board[9][i] + board[10][i] + board[11][i]) > 0:
            occupied.append(1)
        else:
            occupied.append(0)

    if white_turn:
        moves = [] # list of legal moves
        for piece in range(6):
            for i in range(len(board[piece])): # look for pieces to move
                if board[piece][i] == 1: # once a piece is selected, look for legal places to move it to
                    # print("debug: found a piece to move")
                    for dest in range(len(board[piece])): # you could technically replace 'piece' with '0'
                        # print("debug: looking for places to move it to")
                        if check_move(board, piece, [i, dest]):
                            # print("debug: successfully found a place")
                            moves.append([i, dest])
                            # print(f"debug: moves list so far = {moves}")
        # if len(moves) == 0:
            # print("legal_moves: stalemate")
        return moves
    else: # remember to edit any 'piece' indices when it's black's turn
        moves = [] # list of legal moves
        for piece in range(6):
            for i in range(len(board[piece + 6])): # look for pieces to move
                if board[piece + 6][i] == 1: # once a piece is selected, look for legal places to move it to
                    for dest in range(len(board[piece + 6])): # you could technically replace 'piece' with '0'
                        if check_move(board, piece, [i, dest]):
                            # print("debug: successfully found a place")
                            moves.append([i, dest])
        # if len(moves) == 0:
            # print("legal_moves: stalemate")
        return moves

# check if a move is legal
# remember to also make it check if the king is under check
def check_move(board, piece, move, check_king = True):
    start = move[0]
    destination = move[1]

    if piece > 5 and piece < 12:
        print("check_move: piece index is above 5, which will result in check_move automatically returning False")

    occupied = [[], []] # it's not stupid if it works
    for i in range(len(board[0])):
        if (board[0][i] + board[1][i] + board[2][i] + board[3][i] + board[4][i] + board[5][i]) > 0:
            occupied[0].append(1)
        else:
            occupied[0].append(0)
    for i in range(len(board[0])):
        if (board[6][i] + board[7][i] + board[8][i] + board[9][i] + board[10][i] + board[11][i]) > 0:
            occupied[1].append(1) # idk why I wrote it like this
        else:
            occupied[1].append(0)

    if (start < 0) or (start > 63):
        # print("check_move: start index is outside of board")
        return False
    if (destination < 0) or (destination > 63):
        # print("check_move: destination index is outside of board")
        return False

    # determine piece color
    self_index = 0 # for occupied[]
    opponent_index = 1
    if (occupied[0][start] == 1) and (occupied[1][start] == 1):
        # print("check_move: start position is occupied by both white and black")
        return False
    elif (occupied[0][start] == 0) and (occupied[1][start] == 0):
        # print("check_move: start position is empty")
        return False
    elif occupied[1][start] == 1:
        self_index = 1
        opponent_index = 0

    if occupied[self_index][destination] == 1:
        # print("check_move: position is already occupied by own piece")
        return False
    
    if check_king:
        if check(make_move(board, move, False)) == 3:
            # print("check_move: \"I won, but at what cost\" ahhh move")
            return False
        if self_index == 0: # player is white
            if check(make_move(board, move, False)) == -1:
                # print("check_move: white king is under check")
                return False
        elif self_index == 1: # player is black
            if check(make_move(board, move, False)) == 1:
                # print("check_move: black king is under check")
                return False

    start_row = int(start / 8)
    start_col = start % 8
    dest_row = int(destination / 8)
    dest_col = destination % 8
    row_diff = abs(start_row - dest_row)
    col_diff = abs(start_col - dest_col)
    if col_diff != 0:
        interval = int((destination - start) / col_diff)
    elif row_diff != 0:
        interval = int((destination - start) / row_diff)
    else:
        # print("check_move: start position and destination are the same")
        return False

    if piece == 0:
        if col_diff == 0: # no attempted capturing
            if occupied[opponent_index][destination] == 1:
                # print("check_move: pawn cannot capture forward")
                return False
            if self_index == 0:
                if (start >= 48) and (start <= 55):
                    if (destination == start - 16) and (occupied[self_index][start - 8] == 0) and (occupied[opponent_index][start - 8] == 0):
                        return True
                if destination == start - 8: # don't use elif bc both conditions should be checked
                    return True
                else:
                    # print("check_move: not a valid pawn move 1")
                    return False
            elif self_index == 1:
                if (start >= 8) and (start <= 15):
                    if (destination == start + 16) and (occupied[self_index][start + 8] == 0) and (occupied[opponent_index][start + 8] == 0):
                        return True
                if destination == start + 8: # don't use elif bc both conditions should be checked
                    return True
                else:
                    # print("check_move: not a valid pawn move 2")
                    return False
        elif col_diff == 1: # attempt to capture a piece
            if self_index == 0:
                if row_diff == 1 and (start_row - dest_row) > 0:
                    if occupied[opponent_index][destination] == 1:
                        return True
                # print("check_move: not a valid pawn move 3")
                return False
            elif self_index == 1:
                if row_diff == 1 and (start_row - dest_row) < 0:
                    if occupied[opponent_index][destination] == 1:
                        return True
                # print("check_move: not a valid pawn move 4")
                return False
        else:
            # print("check_move: not a valid pawn move 5")
            return False
    elif piece == 1:
        if row_diff == col_diff:
            for i in range(start + interval, destination, interval):
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: bishop cannot move through other pieces")
                    return False
        else:
            # print("check_move: not a valid bishop move")
            return False
        return True
    elif piece == 2:
        if row_diff == 1 and col_diff == 2:
            return True
        if row_diff == 2 and col_diff == 1:
            return True
        # print("check_move: not a valid knight move")
        return False
    elif piece == 3:
        if start_row == dest_row:
            for i in range(start + interval, destination, interval):
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: rook cannot move through other pieces")
                    return False
        elif start_col == dest_col:
            for i in range(start + interval, destination, interval): # interval uses row_diff here
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: rook cannot move through other pieces")
                    return False
        else:
            # print("check_move: not a valid rook move")
            return False
        return True
    elif piece == 4:
        if start_row == dest_row:
            for i in range(start + interval, destination, interval):
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: queen cannot move through other pieces")
                    return False
        elif start_col == dest_col:
            for i in range(start + interval, destination, interval): # interval uses row_diff here
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: queen cannot move through other pieces")
                    return False
        elif row_diff == col_diff:
            for i in range(start + interval, destination, interval):
                if occupied[0][i] == 1 or occupied[1][i] == 1:
                    # print("check_move: queen cannot move through other pieces")
                    return False
        else:
            # print("check_move: not a valid queen move")
            return False
        return True
    elif piece == 5:
        if row_diff <= 1 and col_diff <= 1:
            return True # check_move already checks if the destination square is threatened before this line of code
        else:
            # print("check_move: not a valid king move")
            return False
    else:
        # print(f"check_move: piece type {piece} is invalid")
        return False

def make_move(board, move, c_move = True):
    piece = -1
    for piece_type in range(len(board)):
        if board[piece_type][move[0]] == 1: # this condition shouldn't ever be True more than once
            if piece != (-1):
                # print("make_move: multiple piece types/colors occupy the same square")
                return board
            piece = piece_type
    if piece == -1:
        # print("make_move: start square is empty")
        return board
    if c_move:
        if check_move(board, piece % 6, move) == False: # check_move doesn't recognize piece numbers > 5
            # print("make_move: check_move returned False")
            return board
    new_board = copy.deepcopy(board) # to prevent it from modifying the original board data >:(
    new_board[piece][move[0]] = 0
    for piece_type in range(len(board)):
        new_board[piece_type][move[1]] = 0 # remove (capture) any enemy pieces
    if piece == 0 and move[1] >= 0 and move[1] <= 7: # promote white pawn to queen
        new_board[4][move[1]] = 1
    elif piece == 6 and move[1] >= 56 and move[1] <= 63: # promote black pawn to queen
        new_board[10][move[1]] = 1
    else:
        new_board[piece][move[1]] = 1
    # send move data to GUI (remember to include promotion data)
    return new_board
        
def count_ones(piece_list):
    count = 0
    for square in piece_list:
        if square == 1:
            count += 1
    return count

def blank_board():
    return [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]

def new_board():
    board = blank_board()
    board[0] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0]
    board[1] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0]
    board[2] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0]
    board[3] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1]
    board[4] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0]
    board[5] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0]
    board[6] = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    board[7] = [0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    board[8] = [0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    board[9] = [1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    board[10] = [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    board[11] = [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    return board

def print_board(board):
    printed_board = []
    for i in range(64):
        printed_board.append("_")
    for square in range(len(board[0])):
        for i in range(len(board)):
            if board[i][square] == 1:
                match i:
                    case 0:
                        printed_board[square] = "P"
                    case 1:
                        printed_board[square] = "B"
                    case 2:
                        printed_board[square] = "N"
                    case 3:
                        printed_board[square] = "R"
                    case 4:
                        printed_board[square] = "Q"
                    case 5:
                        printed_board[square] = "K"
                    case 6:
                        printed_board[square] = "p"
                    case 7:
                        printed_board[square] = "b"
                    case 8:
                        printed_board[square] = "n"
                    case 9:
                        printed_board[square] = "r"
                    case 10:
                        printed_board[square] = "q"
                    case 11:
                        printed_board[square] = "k"
    for i in range(len(printed_board)):
        print(f"{printed_board[i]} ", end = "")
        if (i + 1) % 8 == 0:
            print()

def id_piece(piece):
    match piece:
        case 0:
            return "white_pawn"
        case 1:
            return "white_bishop"
        case 2:
            return "white_knight"
        case 3:
            return "white_rook"
        case 4:
            return "white_queen"
        case 5:
            return "white_king"
        case 6:
            return "black_pawn"
        case 7:
            return "black_bishop"
        case 8:
            return "black_knight"
        case 9:
            return "black_rook"
        case 10:
            return "black_queen"
        case 11:
            return "black_king"
    return "unknown_piece"

def get_piece(board, index):
    piece = -1
    for piece_type in range(len(board)):
        if board[piece_type][index] == 1: # this condition shouldn't ever be True more than once
            piece = piece_type
    return piece

board = new_board()
white_turn = True

while True: # game loop
    print_board(board)
    print()
    if white_turn:
        start_time = time.time()
        # move = search(4, board, 2, white_turn)
        move = [int(num) for num in input("Enter input: ").split()]
        print(f"elapsed time = {time.time() - start_time}")
    else:
        start_time = time.time()
        move = search(4, board, 2, white_turn)
        # move = [int(num) for num in input("Enter input: ").split()]
        print(f"elapsed time = {time.time() - start_time}")
    print(f"move = {move}")
    print(f"move = {notation_conv(move)}")
    if len(move) == 0:
        break
    piece = -1
    for piece_type in range(len(board)):
        if board[piece_type][move[0]] == 1:
            piece = piece_type
    if check_move(board, piece % 6, move) == False:
        print("check_move returned False. try again.")
        continue
    board = make_move(board, move)
    print(f"eval = {evaluate(board)}")
    print(f"check: {check(board)}")
    white_turn = not white_turn
print("GAME OVER")