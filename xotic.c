// Copyright (c) 2021-2024 Sinan Kurtulmus
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BOARD_SIZE 3
#define EMPTY_MARK ' '
#define X_MARK 'X'
#define O_MARK 'O'
#define MAX 99
#define MIN -99

struct timespec Sleeptime = {1, 0};
int *game_setting();
int strong_engine(int turn, char board[][BOARD_SIZE]);
int normal_engine(int turn, char board[][BOARD_SIZE]);
int random_engine(int turn, char board[][BOARD_SIZE]);
int minimax_algorithm(int turn, char board[][BOARD_SIZE]);
int player_input(int player);
int make_move(int move, char mark, char board[][BOARD_SIZE]);
int check_result(char board[][BOARD_SIZE]);
int check_win(char board[][BOARD_SIZE]);
bool check_tie(char board[][BOARD_SIZE]);
void computer_think();
void clear_buffer();
void clear_console();
void draw_board(char board[][BOARD_SIZE]);
void play_turn(int turn, int players[], char board[][BOARD_SIZE]);

int main () {
	clear_console();
	printf("\nWelcome to TicTacToe!\n\n");
	int turn = 0;
	char board[BOARD_SIZE][BOARD_SIZE] = {
		EMPTY_MARK, EMPTY_MARK, EMPTY_MARK,
		EMPTY_MARK, EMPTY_MARK, EMPTY_MARK,
		EMPTY_MARK, EMPTY_MARK, EMPTY_MARK
	};
	int *players = game_setting();
	printf("\nA new game has started.\n");
	while (true) {
		draw_board(board);
		int result = check_result(board);
		if (result == 1) {
			printf("\nThe game ended in a tie.\n\n");
			break;
		} else if (result == 2) {
			printf("\nPlayer 'X' won the game!\n\n");
			break;
		} else if (result == -2) {
			printf("\nPlayer 'O' won the game!\n\n");
			break;
		}
		play_turn(turn, players, board);
		turn++;
	 }
	return 0;
}

int *game_setting() {
	static int players[2];
	while (true) {
		printf("Please choose a game mode. (1-2)\n");
		printf("1 - Player vs. player\n");
		printf("2 - Player vs. computer\n");
		printf("Your choice: ");
		char mode_choice = getchar();
		int mode = mode_choice - '0';
		clear_buffer();
		if (mode < 1 || mode > 2) {
			printf("Invalid choice. Please choose a number between 1 and 2.\n");
		} else if (mode == 1) {
			players[0] = 0;
			players[1] = 0;
			break;
		} else {
			while (true) {
				printf("Please choose an engine to play against. (1-3)\n");
				printf("1 - Strong engine (cannot be defeated)\n");
				printf("2 - Normal engine (can be defeated)\n");
				printf("3 - Random engine (makes random moves)\n");
				printf("Your choice: ");
				char engine_choice = getchar();
				int engine = engine_choice - '0';
				clear_buffer();
				if (engine < 1 || engine > 3) {
					printf("Invalid choice. Please choose a number between 1 and 3.\n");
				} else {
					while (true) {
						printf("Would you like to play with 'X' or 'O'? (1-2)\n");
						printf("1 - X (Goes first)\n");
						printf("2 - O (Goes second)\n");
						printf("Your choice: ");
						char xo_choice = getchar();
						int xo = xo_choice - '0';
						clear_buffer();
						if (xo < 1 || xo > 2) {
							printf("Invalid choice. Please choose a number between 1 and 2.\n");
						} else if (xo == 1) {
							players[0] = 0;
							players[1] = engine;
							break;
						} else {
							players[0] = engine;
							players[1] = 0;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
	return players;
}

void clear_buffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		/* discard */;
}

void clear_console() {
	printf("\e[1;1H\e[2J");
}

void play_turn(int turn, int players[], char board[][BOARD_SIZE]) {
	int move;
	int player = turn % 2;
	char mark = (player == 0) ? X_MARK : O_MARK;
	//if (player == 0) {
	//	mark = X_MARK;
	//} else {
	//	mark = O_MARK;
	//}
	if (players[player] == 0) {
		while (true) {
			move = player_input(player);
			if (make_move(move, mark, board) == 0) {
				break;
			}
		}
	} else {
		computer_think();
		if (players[player] == 1) {
			move = strong_engine(turn, board);
		} else if (players[player] == 2) {
			move = normal_engine(turn, board);
		} else if (players[player] == 3) {
			move = random_engine(turn, board);
		}
		make_move(move, mark, board);
	}
	return;
}

int player_input(int player) {
	char player_name;
	if (player == 0) {
		player_name = 'X';
	} else {
		player_name = 'O';
	}
	printf("\nPlayer '%c' - Your move: ", player_name);
	char input = getchar();
	int move = input - '0';
	clear_buffer();
	return move;
}

void computer_think() {
	printf("\nThinking...\n");
	nanosleep(&Sleeptime, NULL);
}

int strong_engine(int turn, char board[][BOARD_SIZE]) {
	int move = 0;
	int player = turn % 2;
	int optimal_value = (player == 0) ? MIN : MAX;
	char mark = (player == 0) ? X_MARK : O_MARK;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			char analysis_board[BOARD_SIZE][BOARD_SIZE];
			memcpy(analysis_board, board, BOARD_SIZE*BOARD_SIZE*sizeof(char));
			if (analysis_board[i][j] == EMPTY_MARK) {
				analysis_board[i][j] = mark;
				int testing_value = minimax_algorithm(turn + 1, analysis_board);
				if (player == 0 && testing_value > optimal_value) {
					optimal_value = testing_value;
					move = i * BOARD_SIZE + j + 1;
				}
				if (player == 1 && testing_value < optimal_value) {
					optimal_value = testing_value;
					move = i * BOARD_SIZE + j + 1;
				}
			}
		}
	}
	return move;
}

int normal_engine(int turn, char board[][BOARD_SIZE]) {
	int move;
	int player = turn % 2;
	char mark = (player == 0) ? X_MARK : O_MARK;
	char invert_mark = (mark == X_MARK) ? O_MARK : X_MARK;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			char analysis_board[BOARD_SIZE][BOARD_SIZE];
			memcpy(analysis_board, board, BOARD_SIZE*BOARD_SIZE*sizeof(char));
			int win = 0;
			if (analysis_board[i][j] == EMPTY_MARK) {
				analysis_board[i][j] = mark;
				win = abs(check_win(analysis_board));
				if (win == 2) {
					move = i * BOARD_SIZE + j + 1;
					return move;
				}
				analysis_board[i][j] = invert_mark;
				win = abs(check_win(analysis_board));
				if (win == 2) {
					move = i * BOARD_SIZE + j + 1;
					return move;
				}
			}
		}
	}
	move = random_engine(turn, board);
	return move;
}

int random_engine(int turn, char board[][BOARD_SIZE]) {
	int remaining_turns = BOARD_SIZE*BOARD_SIZE - turn;
	int available_moves[remaining_turns];
	int move_index = 0;
	for (int i = 0; i<BOARD_SIZE; i++) {
		for (int j = 0; j<BOARD_SIZE; j++) {
			if (board[i][j] == EMPTY_MARK) {
				int candidate_move = i * BOARD_SIZE + j + 1;
				available_moves[move_index] = candidate_move;
				move_index++;
			}
		}
	}
	int chosen_index = rand() % (remaining_turns);
	int move = available_moves[chosen_index];
	return move;
}

int minimax_algorithm(int turn, char board[][BOARD_SIZE]) {
	int player = turn % 2;
	int depth = BOARD_SIZE*BOARD_SIZE - turn;
	int result = check_result(board);
	if (result == 2) {
		return depth;
	} else if (result == -2) {
		return -depth;
	} else if (result == 1) {
		return 0;
	}
	char mark = (player == 0) ? X_MARK : O_MARK;
	int optimal_value = (player == 0) ? MIN : MAX;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == EMPTY_MARK) {
				board[i][j] = mark;
				int testing_value = minimax_algorithm(turn + 1, board);
				if (player == 0 && testing_value > optimal_value) {
					optimal_value = testing_value;
				} else if (player == 1 && testing_value < optimal_value) {
					optimal_value = testing_value;
				}
				board[i][j] = EMPTY_MARK;
			}
		}
	}
	return optimal_value;
}

int make_move(int move, char mark, char board[][BOARD_SIZE]) {
	if (move >= 1 && move <= BOARD_SIZE*BOARD_SIZE) {
		move--;
		int boardColumn = move % BOARD_SIZE;
		int boardRow = (move - boardColumn + 1) / BOARD_SIZE;
		if (board[boardRow][boardColumn] == EMPTY_MARK) {
			board[boardRow][boardColumn] = mark;
		} else {
			printf("You can only play on empty squares!\n");
			return 1;
		}
	} else {
		printf("Please type a valid square! (1-9)\n");
		return 2;
	}
	return 0;
}

int check_result(char board[][BOARD_SIZE]) {
	int is_decisive = check_win(board);
	if (is_decisive != 0) {
		return is_decisive;
	}
	else {
		bool isDrawn = check_tie(board);
		if (isDrawn == true) {
			return 1;
		}
	}
	return 0;
}

int check_win(char board[][BOARD_SIZE]) {
	for(int i = 0; i<BOARD_SIZE; i++) {
		if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
			if (board[i][0] == X_MARK ) {
				return 2;
			} else if (board[i][0] == O_MARK ) {
				return -2;
			}
		} else if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
			if (board[0][i] == X_MARK ) {
				return 2;
			} else if (board[0][i] == O_MARK ) {
				return -2;
			}
		}
	}
	if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
		if (board[0][0] == X_MARK ) {
			return 2;
		} else if (board[0][0] == O_MARK ) {
			return -2;
		}
	} else if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
		if (board[0][2] == X_MARK ) {
			return 2;
		} else if (board[0][2] == O_MARK ) {
			return -2;
		}
	}
	return 0;
}

bool check_tie(char board[][BOARD_SIZE]) {
	for (int i = 0; i<BOARD_SIZE; i++) {
		for (int j = 0; j<BOARD_SIZE; j++) {
			if (board[i][j] == EMPTY_MARK) {
				return false;
			}
		}
	}
	return true;
}

void draw_board(char board[][BOARD_SIZE]) {
	clear_console();
	printf("\n  ______________________________\n");
	printf(" /_____________________________/|\n");
	printf(" |1        |2        |3        ||\n");
	printf(" |         |         |         ||\n");
	printf(" |    %c    |    %c    |    %c    ||\n", board[0][0], board[0][1], board[0][2]);
	printf(" |         |         |         ||\n");
	printf(" |_________|_________|_________||\n");
	printf(" |4        |5        |6        ||\n");
	printf(" |         |         |         ||\n");
	printf(" |    %c    |    %c    |    %c    ||\n", board[1][0], board[1][1], board[1][2]);
	printf(" |         |         |         ||\n");
	printf(" |_________|_________|_________||\n");
	printf(" |7        |8        |9        ||\n");
	printf(" |         |         |         ||\n");
	printf(" |    %c    |    %c    |    %c    ||\n", board[2][0], board[2][1], board[2][2]);
	printf(" |         |         |         ||\n");
	printf(" |_________|_________|_________|/\n");
}
