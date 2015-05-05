

#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "snake.h"

int max_win_x, max_win_y;

int
main(int argc, char *argv[])
{
  SNAKE *snake=NULL;
  int game_level=4;   //init game level 5
  int ch;
  FOOD *food;
  

  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  clear();
  srand((int)time(0));  
  getmaxyx(stdscr, max_win_y, max_win_x); 
  snake = snake_born();
  food = new_food(max_win_y, max_win_x);
  
  do{
  getmaxyx(stdscr, max_win_y, max_win_x); 
  //halfdelay to skip waiting for input, snake auto move
  halfdelay(game_level);


  food->eaten?replace_food(food, snake, max_win_y, max_win_x):\
  	      mvaddch(food->locate[1], food->locate[0], FOOD_CHAR);

  snake_print(snake);
  game_level = check_level(snake->snake_body.eaten, game_level);

  if (snake->dead(snake)){
    mvprintw(20, 20, "you die! eaten:%d, level:%d\n", snake->snake_body.eaten, game_level);
    refresh();
    break;
    }
  refresh();
  
  if (key_control(snake))
    break;

  if (IS_EATABLE(snake->snake_body.Head.x, snake->snake_body.Head.y,\
  		  food->locate[0], food->locate[1]))
  {   
      snake->eat(snake);  
      mvaddch(food->locate[1],food->locate[0], ' ');
      food->eaten = 1;
  }
  snake_erase(snake);
  snake->smove(snake);

  }while(1);

  free_resources(food, snake);
  endwin();
  return 0;
}
