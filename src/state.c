#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t* default_state = malloc(sizeof(game_state_t));

  default_state->num_rows = 18;
  default_state->board = malloc(sizeof(char*) * default_state->num_rows);
  for (int i = 0; i < default_state->num_rows; i ++ )
  {
    default_state->board[i] = malloc(sizeof(char) * 21);
    if (i == 0 || i == default_state->num_rows - 1)
    {
      strcpy(default_state->board[i], "####################");
    }
    else if (i == 2)
    {
      strcpy(default_state->board[i], "# d>D    *         #");
    }
    else 
    {
      strcpy(default_state->board[i], "#                  #");
    }
  }
  default_state->num_snakes = 1;
  default_state->snakes = malloc(sizeof(snake_t) * 1);
  default_state->snakes[0] = (snake_t){2, 2, 2, 4, true};

  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++ )
  {
    free(state->board[i]);
    state->board[i] = NULL;
  }
  free(state->board);
  state->board = NULL;
  free(state->snakes);
  state->snakes = NULL;
  free(state);
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++ )
  {
    fprintf(fp, "%s\n", state->board[i]);
  }
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'a' || 
        c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'A' || c == 'W' ||
        c == 'S' || c == 'D' || 
        c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_head(c) || is_tail(c) ||
        c == '^' || c == 'v' ||
        c == '<' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') return 'w';
  if (c == '<') return 'a';
  if (c == 'v') return 's';
  if (c == '>') return 'd';
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') return '^';
  if (c == 'A') return '<';
  if (c == 'S') return 'v';
  if (c == 'D') return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  char c = get_board_at(state, snake->head_row, snake->head_col);
  uint32_t next_row = get_next_row(snake->head_row, c);
  uint32_t next_col = get_next_col(snake->head_col, c);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  char c = get_board_at(state, snake->head_row, snake->head_col);
  uint32_t next_row = get_next_row(snake->head_row, c);
  uint32_t next_col = get_next_col(snake->head_col, c);

  set_board_at(state, snake->head_row, snake->head_col, head_to_body(c));
  set_board_at(state, next_row, next_col, c);

  snake->head_row = next_row;
  snake->head_col = next_col;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];
  char c = get_board_at(state, snake->tail_row, snake->tail_col);
  uint32_t next_row = get_next_row(snake->tail_row, c);
  uint32_t next_col = get_next_col(snake->tail_col, c);

  set_board_at(state, next_row, next_col, 
          body_to_tail(get_board_at(state, next_row, next_col)));
  set_board_at(state, snake->tail_row, snake->tail_col, ' ');

  snake->tail_row = next_row;
  snake->tail_col = next_col;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (uint32_t i = 0; i < state->num_snakes; i ++ )
  {
    snake_t* snake = &state->snakes[i];
    char c = get_board_at(state, snake->head_row, snake->head_col);
    uint32_t next_row = get_next_row(snake->head_row, c);
    uint32_t next_col = get_next_col(snake->head_col, c);
    char next_c = get_board_at(state, next_row, next_col);
    if (is_snake(next_c) || next_c == '#')
    {
      snake->live = false;
      set_board_at(state, snake->head_row, snake->head_col, 'x');
    }
    else
    {
      update_head(state, i);
      if (next_c == '*') add_food(state);
      else update_tail(state, i);
    }
  }
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = 1;
  state->board = malloc(sizeof(char*));
  state->board[0] = malloc(sizeof(char));
  state->board[0][0] = '\0';
  state->num_snakes = 0;
  state->snakes = NULL;

  int row = 0, col = 0;
  char c;
  while ((c = (char)fgetc(fp)) != EOF)
  {
    if (c == '\n')
    {
      row ++;
      col = 0;
      continue;
    }
    if (row == state->num_rows)
    {
      state->board = realloc(state->board, sizeof(char*) * (size_t)(row + 1));
      state->board[row] = malloc(sizeof(char));
      state->board[row][0] = '\0';
      state->num_rows ++;
    }
    col ++;
    state->board[row] = realloc(state->board[row], sizeof(char) * (size_t)(col + 1));
    state->board[row][col - 1] = c;
    state->board[row][col] = '\0';
  }  

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  uint32_t row = snake->tail_row, col = snake->tail_col;
  char c = get_board_at(state, row, col);
  while (!is_head(c))
  {
    row = get_next_row(row, c);
    col = get_next_col(col, c);
    c = get_board_at(state, row, col);
  }
  snake->head_row = row;
  snake->head_col = col;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++ )
  {
    for (int j = 0; state->board[i][j]; j ++ )
    {
      if (is_tail(state->board[i][j]))
        state->num_snakes ++;
    }
  }
  state->snakes = malloc(sizeof(snake_t) * state->num_snakes);
  snake_t* snakes = state->snakes;
  uint32_t k = 0;
  for (uint32_t i = 0; i < state->num_rows; i ++ )
  {
    for (uint32_t j = 0; state->board[i][j]; j ++ )
    {
      if (is_tail(state->board[i][j]))
      {
        snakes[k].tail_row = i;
        snakes[k].tail_col = j;
        find_head(state, k);
        snakes[k].live = true;
        k ++;
      }
    }
  }
  return state;
}
