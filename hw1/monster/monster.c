/*
    Aryan Patel - monster.c
    CS214 - Ames
    Fall 2022
    HW1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// move player
int movePlayer (char direction, char **board, int board_size_x, int board_size_y) {

    /*  return values: 

        1: invalid move
        2: moved into monsters square (loss)
        3: moved into goal square (win)
        4: completed move, no winner

    */

    // find player on board
    int player_pos_x;
    int player_pos_y;

    for(int row = 0; row < board_size_y; row++){
        for(int col = 0; col < board_size_x; col++){
            if (board[row][col] == 'P') {
                player_pos_x = col;
                player_pos_y = row;
            }
        }
    }

    if (direction == 'N') {

        if ((player_pos_y - 1) < 0) { 
            return 1;

        } else {

            if (board[player_pos_y - 1][player_pos_x] == 'G') {
                return 3;

            } else if (board[player_pos_y - 1][player_pos_x] == 'M') {
                return 2;

            } else {
                board[player_pos_y][player_pos_x] = '.';
                board[player_pos_y - 1][player_pos_x] = 'P';
                return 4;
            }

        }
        
    } else if (direction == 'S') {
        
        if ((player_pos_y + 1) > (board_size_y - 1)) {
            return 1;

        } else {

            if (board[player_pos_y + 1][player_pos_x] == 'G') {
                return 3;

            } else if (board[player_pos_y + 1][player_pos_x] == 'M') {
                return 2;

            } else {
                board[player_pos_y][player_pos_x] = '.';
                board[player_pos_y + 1][player_pos_x] = 'P';
                return 4;
            }

        }

    } else if (direction == 'E') {
        
        if ((player_pos_x + 1) > (board_size_x - 1)) {
            return 1;

        } else {

            if (board[player_pos_y][player_pos_x + 1] == 'G') {
                return 3;

            } else if (board[player_pos_y][player_pos_x + 1] == 'M') {
                return 2;

            } else {
                board[player_pos_y][player_pos_x] = '.';
                board[player_pos_y][player_pos_x + 1] = 'P';
                return 4;
            }

        }

    } else if (direction == 'W') {
        
        if ((player_pos_x - 1) < 0) {
            return 1;

        } else {

            if (board[player_pos_y][player_pos_x - 1] == 'G') {
                return 3;

            } else if (board[player_pos_y][player_pos_x - 1] == 'M') {
                return 2;

            } else {
                board[player_pos_y][player_pos_x] = '.';
                board[player_pos_y][player_pos_x - 1] = 'P';
                return 4;
            }

        }

    } else {
        return 1; // unknown direction given
    }

}

// move monster
char moveMonster (char **board, int board_size_x, int board_size_y) {

    /*  return values: 

        N, S, E, W: direction move completed by monster
        G: moved onto player (win)
        F: forfeit move
        I: move error

    */

    // find player on board
    int player_pos_x;
    int player_pos_y;

    for(int row = 0; row < board_size_y; row++){
        for(int col = 0; col < board_size_x; col++){
            if (board[row][col] == 'P') {
                player_pos_x = col;
                player_pos_y = row;
            }
        }
    }

    // find monster on board
    int monster_pos_x;
    int monster_pos_y;

    for(int row = 0; row < board_size_y; row++){
        for(int col = 0; col < board_size_x; col++){
            if (board[row][col] == 'M') {
                monster_pos_x = col;
                monster_pos_y = row;
            }
        }
    }

    // compare vertical and horizontal distance from player
    int horizontal_d = abs(monster_pos_x - player_pos_x);
    int vertical_d = abs(monster_pos_y - player_pos_y);

    if (horizontal_d > vertical_d) { // move closer x dir

        if((monster_pos_x - player_pos_x) > 0) { // move West

            if ((monster_pos_x - 1) > 0) {
                
                if (board[monster_pos_y][monster_pos_x - 1] == 'P') {
                    printf("monster moves W\n");
                    return 'G';

                } else if (board[monster_pos_y][monster_pos_x - 1] == 'G') {
                    return 'F';

                } else {
                    board[monster_pos_y][monster_pos_x] = '.';
                    board[monster_pos_y][monster_pos_x - 1] = 'M';
                    return 'W';
                }

            }

        } else { // move East

            if ((monster_pos_x + 1) < board_size_x) {
                
                if (board[monster_pos_y][monster_pos_x + 1] == 'P') {
                    printf("monster moves E\n");
                    return 'G';

                } else if (board[monster_pos_y][monster_pos_x + 1] == 'G') {
                    return 'F';
                    
                } else {
                    board[monster_pos_y][monster_pos_x] = '.';
                    board[monster_pos_y][monster_pos_x + 1] = 'M';
                    return 'E';
                }

            }

        }
        
    } else if (horizontal_d < vertical_d) { // move closer y dir

        if((monster_pos_y - player_pos_y) > 0) { // move North

            if ((monster_pos_y - 1) > 0) {
                
                if (board[monster_pos_y - 1][monster_pos_x] == 'P') {
                    printf("monster moves N\n");
                    return 'G';

                } else if (board[monster_pos_y - 1][monster_pos_x] == 'G') {
                    return 'F';

                } else {
                    board[monster_pos_y][monster_pos_x] = '.';
                    board[monster_pos_y - 1][monster_pos_x] = 'M';
                    return 'N';
                }

            }

        } else { // move South

            if ((monster_pos_y + 1) < board_size_y) {
                
                if (board[monster_pos_y + 1][monster_pos_x] == 'P') {
                    printf("monster moves S\n");
                    return 'G';

                } else if (board[monster_pos_y + 1][monster_pos_x] == 'G') {
                    return 'F';

                } else {
                    board[monster_pos_y][monster_pos_x] = '.';
                    board[monster_pos_y + 1][monster_pos_x] = 'M';
                    return 'S';
                }

            }

        }

    } else { //pick axis randomly

        int r = rand() % 2;

        if (r == 0) { // move x dir

            if((monster_pos_x - player_pos_x) > 0) { // move West

                if ((monster_pos_x - 1) > 0) {
                    
                    if (board[monster_pos_y][monster_pos_x - 1] == 'P') {
                        printf("monster moves W\n");
                        return 'G';

                    } else if (board[monster_pos_y][monster_pos_x - 1] == 'G') {
                        return 'F';

                    } else {
                        board[monster_pos_y][monster_pos_x] = '.';
                        board[monster_pos_y][monster_pos_x - 1] = 'M';
                        return 'W';
                    }
                }

            } else { // move East

                if ((monster_pos_x + 1) < board_size_x) {
                    
                    if (board[monster_pos_y][monster_pos_x + 1] == 'P') {
                        printf("monster moves E\n");
                        return 'G';

                    } else if (board[monster_pos_y][monster_pos_x + 1] == 'G') {
                        return 'F';
                        
                    } else {
                        board[monster_pos_y][monster_pos_x] = '.';
                        board[monster_pos_y][monster_pos_x + 1] = 'M';
                        return 'E';
                    }
                }

            }
            
        } else { // move y dir

            if((monster_pos_y - player_pos_y) > 0) { // move North

                if ((monster_pos_y - 1) > 0) {
                    
                    if (board[monster_pos_y - 1][monster_pos_x] == 'P') {
                        printf("monster moves N\n");
                        return 'G';

                    } else if (board[monster_pos_y - 1][monster_pos_x] == 'G') {
                        return 'F';

                    } else {
                        board[monster_pos_y][monster_pos_x] = '.';
                        board[monster_pos_y - 1][monster_pos_x] = 'M';
                        return 'N';
                    }

                }

            } else { // move South

                if ((monster_pos_y + 1) < board_size_y) {
                    
                    if (board[monster_pos_y + 1][monster_pos_x] == 'P') {
                        printf("monster moves S\n");
                        return 'G';

                    } else if (board[monster_pos_y + 1][monster_pos_x] == 'G') {
                        return 'F';

                    } else {
                        board[monster_pos_y][monster_pos_x] = '.';
                        board[monster_pos_y + 1][monster_pos_x] = 'M';
                        return 'S';
                    }

                }

            }

        }

    }

    return 'I';

}

// print board
void printBoard (char** board, int board_size_x, int board_size_y) {

    int row, col;
    for (row = 0; row < board_size_y; row++) {
        for(col = 0; col < board_size_x; col++) {

            if(col == 0) {
                printf("%c", board[row][col]);
            } else {
                printf(" %c", board[row][col]);
            }

        }
        printf("\n");
    }
}

void freeMatrix (char **arr, int row) {
    for (int i = 0; i < row; i++) {
        free(arr[i]);
    }
    free(arr);
}
                
int main (int argc, char** argv){

    if (argc == 9) {

        // read board info
        int boardX = atoi(argv[1]);
        int boardY = atoi(argv[2]);

        int plrX = atoi(argv[3]);
        int plrY = atoi(argv[4]);

        int goalX = atoi(argv[5]);
        int goalY = atoi(argv[6]);

        int monX = atoi(argv[7]);
        int monY = atoi(argv[8]);

        // create board
        char **game_board = malloc(boardY * sizeof(char *));

        for(int i = 0; i < boardY; i++) {
            game_board[i] = malloc(boardX * sizeof(char));
        }

        // init board values
        for(int row = 0; row < boardY; row++){
            for(int col = 0; col < boardX; col++){
                game_board[row][col] = '.';
            }
        }

        game_board[(boardY - 1) - plrY][plrX] = 'P';
        game_board[(boardY - 1) - goalY][goalX] = 'G';
        game_board[(boardY - 1) - monY][monX] = 'M';

        printBoard(game_board, boardX, boardY);

        while(1) {

            int res;
            char monster_move;
            char direction = getchar();

            if (direction == EOF) {
                break;
            }

            switch (direction) {

            case 'N':
                
                res = movePlayer('N', game_board, boardX, boardY);

                if (res == 1) {
                    printf("invalid move\n");

                } else if (res == 2) {
                    printf("monster wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else if (res == 3) {
                    printf("player wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else {

                    monster_move = moveMonster(game_board, boardX, boardY);

                    if (monster_move == 'G') {
                        printf("monster wins!\n");
                        freeMatrix(game_board, boardY);
                        return (EXIT_SUCCESS);

                    } else if (monster_move == 'N' || monster_move == 'S' || monster_move == 'E' || monster_move == 'W') {
                        printf("monster moves %c\n", monster_move);
                        printBoard(game_board, boardX, boardY);
                        printf("\n");

                    } else {
                        printBoard(game_board, boardX, boardY);
                        printf("\n");
                    }


                } 

                break;

            case 'S':
                
                res = movePlayer('S', game_board, boardX, boardY);

                if (res == 1) {
                    printf("invalid move\n");

                } else if (res == 2) {
                    printf("monster wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else if (res == 3) {
                    printf("player wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else {
                    
                    monster_move = moveMonster(game_board, boardX, boardY);

                    if (monster_move == 'G') {
                        printf("monster wins!\n");
                        freeMatrix(game_board, boardY);
                        return (EXIT_SUCCESS);

                    } else if (monster_move == 'N' || monster_move == 'S' || monster_move == 'E' || monster_move == 'W') {
                        printf("monster moves %c\n", monster_move);
                        printBoard(game_board, boardX, boardY);
                        printf("\n");

                    } else {
                        printBoard(game_board, boardX, boardY);
                        printf("\n");
                    }

                }

                break;

            case 'E':

                res = movePlayer('E', game_board, boardX, boardY);

                if (res == 1) {
                    printf("invalid move\n");

                } else if (res == 2) {
                    printf("monster wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else if (res == 3) {
                    printf("player wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);

                } else {
                    
                    monster_move = moveMonster(game_board, boardX, boardY);

                    if (monster_move == 'G') {
                        printf("monster wins!\n");
                        freeMatrix(game_board, boardY);
                        return (EXIT_SUCCESS);
                    

                    } else if (monster_move == 'N' || monster_move == 'S' || monster_move == 'E' || monster_move == 'W') {
                        printf("monster moves %c\n", monster_move);
                        printBoard(game_board, boardX, boardY);
                        printf("\n");

                    } else {
                        printBoard(game_board, boardX, boardY);
                        printf("\n");
                    }

                }

                break;

            case 'W':

                res = movePlayer('W', game_board, boardX, boardY);

                if (res == 1) {
                    printf("invalid move\n");

                } else if (res == 2) {
                    printf("monster wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);
                    
                } else if (res == 3) {
                    printf("player wins!\n");
                    freeMatrix(game_board, boardY);
                    return (EXIT_SUCCESS);
                    
                } else {
                    
                    monster_move = moveMonster(game_board, boardX, boardY);

                    if (monster_move == 'G') {
                        printf("monster wins!\n");
                        freeMatrix(game_board, boardY);
                        return (EXIT_SUCCESS);

                    } else if (monster_move == 'N' || monster_move == 'S' || monster_move == 'E' || monster_move == 'W') {
                        printf("monster moves %c\n", monster_move);
                        printBoard(game_board, boardX, boardY);
                        printf("\n");

                    } else {
                        printBoard(game_board, boardX, boardY);
                        printf("\n");
                    }

                }

                break;

            default:
                break;
            
            }

        }
        
        freeMatrix(game_board, boardY);

    } else {

        return (EXIT_FAILURE); // unexpected arguments

    }

    return (EXIT_SUCCESS);
}