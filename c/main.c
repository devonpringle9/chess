#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

enum PlayerSide {
    white = 0,
    black = 1,
    none  = 2
};

enum PieceType {
    empty  = 0,
    pawn   = 1,
    rook   = 2,
    knight = 3,
    bishop = 4,
    queen  = 5,
    king   = 6,
};

struct Piece {
    enum PieceType piece_type;
    enum PlayerSide player_side;
    int moved_before;
};

struct Player {
    enum PlayerSide player_side;
    struct Piece pieces[16];
};

struct Game {
    struct Piece* board[8][8];   // (row, column). initialised pieces line up along a row
    struct Player players[2];
    struct Player players_turn;
};

void print_board(struct Game* game) {
    // printf("  a b c d e f g h\n");
    printf("  1 2 3 4 5 6 7 8\n");
    for (int row = 0; row < 8; row++) {
        printf("%d", row+1);
        for (int col = 0; col < 8; col++) {
            printf("|");
            if (game->board[row][col]->player_side) {
                printf("\e[1;91");  // black text
            } else {
                printf("\e[1;97");  // white text
            }
            if (row%2 && col%2 || (!(row%2) && !(col%2))) {
                printf(";44m");   // blue bg
            } else {
                printf(";40m");   // black bg
            }
            switch (game->board[row][col]->piece_type) {
            case empty:
                printf(" ");
                break;
            case pawn:
                printf("p");
                break;
            case rook:
                printf("r");
                break;
            case knight:
                printf("h");
                break;
            case bishop:
                printf("b");
                break;
            case queen:
                printf("Q");
                break;
            case king:
                printf("K");
                break;
            }
            printf("\e[1;49;0m");
        }
        printf("|\n");
    }
}

int request_move(int* moves) {
    /* requests input from user: x0 y0 x1 y1
    checks the inputs for correctness
    converts the user input to 0 based positions on the board
    */
    // get input. only 8 chars for the 4 digits that are expected
    char input[9];
    memset(input, 0, sizeof(input));
    setbuf(stdin, NULL);
    printf("What is your move (x0 y0 x1 y1): ");
    fgets(input, 9, stdin);
    printf("\n");

    char* moves_str[4];
    char* input_ptr;
    input_ptr = strtok(input, " \n");

    int i = 0;
    while (input_ptr != NULL) {
        // there shouldn't be more then 4 inputs
        if (i > 4) {
            printf("we reached more than 4 inputs\n");
            return 1;
        }
        if (strlen(input_ptr) != 1) {   // must be one digit
            printf("more than one digit\n");
            return 1;
        } else if (!isdigit(input_ptr[0])) {   // we expect a digit
            printf("not a digit\n");
            return 1;
        } else if (atoi(input_ptr) > 8 || atoi(input_ptr) < 1) { // board is only 8 wide
            printf("not within limits\n");
            return 1;
        }
        moves_str[i] = strdup(input_ptr);
        i++;
        input_ptr = strtok(NULL, " \n");
    }
    
    if (i < 4) {
        printf("didnt receive enough inputs\n");
        return 1;
    }

    // Convert to move to ints
    for (int move = 0; move < 4; move++) {
        moves[move] = atoi(moves_str[move]) - 1;
    }

    // DEBUG
    // for (int i = 0; i < 4; i++) {
    //     printf("move %d %d\n", i, moves[i]);
    // }
    
    // All good here
    return 0;
}


int valid_position_on_board(int row, int col) {
    if (row > 7 || col > 7 || row < 0 || col < 0) {
        return 1;
    } else {
        return 0;
    }
}


int get_piece(struct Game* game, int row, int col, struct Piece** piece) {
    if (valid_position_on_board(row, col)) {
        return 1;
    }
    *piece = game->board[row][col];
    return 0;
}


int valid_rook_move(struct Game* game, int row0, int col0, int diff_row, int diff_col) {
    struct Piece* tmp;
    if (diff_row > 0) {
        for (int towards_position = 1; towards_position < diff_row; towards_position++) {
            get_piece(game, row0 + towards_position, col0, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_row < 0) {
        for (int towards_position = 1; towards_position < diff_row; towards_position--) {
            get_piece(game, row0 - towards_position, col0, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_col > 0) {
        for (int towards_position = 1; towards_position < diff_col; towards_position++) {
            get_piece(game, row0, col0 + towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_col < 0) {
        for (int towards_position = 1; towards_position < diff_col; towards_position--) {
            get_piece(game, row0, col0 - towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    }
    return 0;
}


int valid_bishop_move(struct Game* game, int row0, int col0, int diff_row, int diff_col) {
    struct Piece* tmp;
    if (diff_row > 0 && diff_col > 0) {
        for (int towards_position = 1; towards_position < abs(diff_row); towards_position++) {
            get_piece(game, row0 + towards_position, col0 + towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_row > 0 && diff_col < 0) {
        for (int towards_position = 1; towards_position < abs(diff_row); towards_position++) {
            get_piece(game, row0 + towards_position, col0 - towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_row < 0 && diff_col < 0) {
        for (int towards_position = 1; towards_position < abs(diff_row); towards_position++) {
            get_piece(game, row0 - towards_position, col0 - towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    } else if (diff_row < 0 && diff_col > 0) {
        for (int towards_position = 1; towards_position < abs(diff_row); towards_position++) {
            get_piece(game, row0 - towards_position, col0 + towards_position, &tmp);
            if (tmp->piece_type != empty) {
                return 1;
            }
        }
    }
    return 0;
}


int move_piece(struct Game* game, int row0, int col0, int row1, int col1) {
    struct Piece *p0, *p1;
    if (get_piece(game, row0, col0, &p0) ||
            get_piece(game, row1, col1, &p1)) {
        printf("invalid position\n");
        return 1;
    }

    // Debug print pieces
    // printf("p0 side %d type %d\np1 side %d type %d\nmove %d %d %d %d\n", p0->player_side, p0->piece_type, p1->player_side, p1->piece_type, row0, col0, row1, col1);

    // Dont let a piece move on top of its own piece
    // One exception is made for the special case of casteling the king
    int castle_the_king = 0;
    if (p0->player_side == p1->player_side) {
        if (p0->piece_type == rook && p1->piece_type == king) {
            // The rook and king must not have moved prior
            if (!p0->moved_before && !p1->moved_before) {
                // there also can't be any piece between the king and rook
                // but we will check that later
                castle_the_king = 1;
            } else {
                printf("Invalid castle the king\n");
                return 1;
            }
        } else {
            printf("You are trying to move on top of your own piece\n");
            return 1;
        }
    }

    // The piece being moved must be on the movers side
    if (game->players_turn.player_side != p0->player_side) {
        printf("You have to move your own piece\n");
        return 1;
    }

    // See what piece this is and if it allowed to move there
    int diff_row = row1 - row0;
    int diff_col = col1 - col0;
    int good_move = 0;
    int bad_move = 0;
    int direction;
    struct Piece* tmp;
    switch (p0->piece_type) {
        case empty:
            printf("there was no piece at %d %d\n", row0, col0);
            return 1;
            break;
        case king:
            // The position must be only one space around the current position
            if (abs(diff_row) > 1 && abs(diff_col) > 1) {
                printf("king cannot move more than one position\n");
            } else {
                good_move = 1;
            }
            break;
        case pawn:
            if (p0->player_side == white) {
                direction = 1;
            } else {
                direction = -1;
            }
            // The pawn can move one forward
            // or two forward if it hasn't moved before
            get_piece(game, row1 - direction, col1, &tmp);
            if ((diff_row == (1*direction) && diff_col == 0) ||
                    (diff_row == (2*direction) && !p0->moved_before
                    && tmp->piece_type == empty)) {
                if (diff_col == 0) {
                    if (p1->piece_type == empty) {
                        // this is fine
                        printf("this is fine\n");
                        good_move = 1;
                        break;
                    }
                }
            } else if (abs(diff_col) == 1 && diff_row == (1*direction)
                    && p1->piece_type != empty && p1->player_side != p0->player_side) {
                good_move = 1;
            }
            break;
        case knight:
            if (((abs(diff_row) == 2 && abs(diff_col) == 1)) ||
                    (abs(diff_col) == 2 && abs(diff_row) == 1)) {
                good_move = 1;
            }
            break;
        case rook:
            if (diff_row && !diff_col || diff_col && !diff_row) {
                // there mustnt be a piece between this piece and the destination
                if (valid_rook_move(game, row0, col0, diff_row, diff_col) == 0) {
                    good_move = 1;
                }
            }
            break;
        case bishop:
            if (abs(diff_row) == abs(diff_col)) {
                // there mustnt be a piece between this piece and the destination
                if (valid_bishop_move(game, row0, col0, diff_row, diff_col) == 0) {
                    good_move = 1;
                }
            }
            break;
        case queen:
            if (abs(diff_row) == abs(diff_col)) {
                // moving like a bishop
                if (valid_bishop_move(game, row0, col0, diff_row, diff_col) == 0) {
                    good_move = 1;
                }
            } else if (diff_row && !diff_col || diff_col && !diff_row) {
                // moving like a rook
                if (valid_rook_move(game, row0, col0, diff_row, diff_col) == 0) {
                    good_move = 1;
                }
            }
            break;
    }

    if (!good_move || bad_move) return 1;

    // If the piece was the oppositions then it has been
    // taken and must be removed from the board
    if (p0->player_side != none && p0->player_side != p1->player_side) {
        p1->player_side = none;
        p1->piece_type = empty;
    }

    // p0 has been moved
    p0->moved_before = 1;

    // Move piece - swap piece 0 with piece 1
    tmp = p1;
    game->board[row1][col1] = p0;
    game->board[row0][col0] = tmp;
    return 0;
}

// Functions for testing and running games from a file
int get_moves_from_file(char* filename, int** moves) {
    FILE* f = fopen(filename, "r");
    char move[9];
    int line = 0;
    while (fgets(move, 9, f) != NULL) {
        moves[line][0] = atoi(strtok(move, " ")) - 1;
        for (int i = 1; i < 4; i++) {
            moves[line][i] = atoi(strtok(NULL, " ")) - 1;
        }
        line++;
    }
    fclose(f);
    return line;
}


int run_game(int from_moves_list, int** moves_list, int move_count) {
    printf("Welcome to chess\n");
    struct Game game;
    int piece_row;
    // Initialise all squares on board to be an empty piece
    struct Piece p;
    p.player_side = none;
    p.piece_type = empty;
    p.moved_before = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            game.board[row][col] = &p;
        }
    }
    for (int player = white; player <= black; player++) {   // TODO: iterate through enums instead
        game.players[player].player_side = player;
        // The pawns will be placed on rows 1 or 6
        piece_row = game.players[player].player_side ? 6 : 1;
        for (int pawn_count = 0; pawn_count < 8; pawn_count++) {
            game.players[player].pieces[pawn_count].piece_type = pawn;
            game.board[piece_row][pawn_count] = &game.players[player].pieces[pawn_count];
        }

        // The non pawn pieces will be placed on rows 0 or 7
        piece_row = game.players[player].player_side ? 7 : 0;
        game.players[player].pieces[8].piece_type = rook;
        game.board[piece_row][0] = &game.players[player].pieces[8];
        game.players[player].pieces[9].piece_type = rook;
        game.board[piece_row][7] = &game.players[player].pieces[9];
        game.players[player].pieces[10].piece_type = knight;
        game.board[piece_row][1] = &game.players[player].pieces[10];
        game.players[player].pieces[11].piece_type = knight;
        game.board[piece_row][6] = &game.players[player].pieces[11];
        game.players[player].pieces[12].piece_type = bishop;
        game.board[piece_row][2] = &game.players[player].pieces[12];
        game.players[player].pieces[13].piece_type = bishop;
        game.board[piece_row][5] = &game.players[player].pieces[13];
        game.players[player].pieces[14].piece_type = queen;
        game.players[player].pieces[15].piece_type = king;
        if (game.players[player].player_side) {
            game.board[piece_row][3] = &game.players[player].pieces[14];
            game.board[piece_row][4] = &game.players[player].pieces[15];
        } else {
            game.board[piece_row][4] = &game.players[player].pieces[14];
            game.board[piece_row][3] = &game.players[player].pieces[15];
        }
        for (int piece = 0; piece < 16; piece++) {
            game.players[player].pieces[piece].moved_before = 0;
            game.players[player].pieces[piece].player_side = player;
        }
    }
    game.players_turn = game.players[0];    // set white as the first player
    print_board(&game);

    // keep requesting moves until the game is over
    int move[4];
    int game_over = 0;
    int debug = 0;
    int completed_move; // has the player made a valid move?
    int total_move_count = 0;
    while ((!game_over && debug < 13) && (move_count > total_move_count)) {
        // go through each player
        for (int player = 0; player < 2; player++) {
            // keep requesting a move from the player until it is valid
            if (!(move_count > total_move_count)) break;
            printf("Player %d\n", player);
            completed_move = 0;
            while (!completed_move) {
                if (from_moves_list) {
                    // Run from file
                    for (int i = 0; i < 4; i++) {
                        move[i] = moves_list[total_move_count][i];
                    }
                    total_move_count++;
                    completed_move = 1;
                } else {
                    // Run from user input
                    while (request_move(move)) {}
                }

                // do the move
                if (!move_piece(&game, move[0], move[1], move[2], move[3])) {
                    completed_move = 1;
                    game.players_turn = game.players[(player+1)%2];
                }
            }
            print_board(&game);
        }
        debug++;
    }
    return 0;
}


void main(int argc, char** argv) {
    int run_tests = 0;
    char* test_filename;
    if (argc > 1) {
        run_tests = 1;
        test_filename = argv[1];
    }
    if (run_tests) {
        int size = 400;
        int* moves_list[size];
        for (int i = 0; i < size; i++) {
            moves_list[i] = (int*)malloc(sizeof(int) * 4);
        }
        int move_count = get_moves_from_file(test_filename, moves_list);
        run_game(1, moves_list, move_count);
    } else {
        run_game(0, NULL, 0);
    }
}
