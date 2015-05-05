
#include <stdlib.h>
#include <ncurses.h>
#include "snake.h"

extern max_win_x, max_win_y;

int
is_pos_overlay(int x, int y, POS *s)
{
  return IS_EATABLE( s->x, s->y, x, y); 
}

int
is_body_overlay(int x, int y, SNAKE *s)
{
  int i;
  POS *body=NULL;
  SNAKE *snake = (SNAKE*)s;

  body = snake->snake_body.body_list;
  for (i=0; i<snake->snake_body.length; i++)
  {
    if (is_pos_overlay(x, y, body))
      return 1;
    body=body->next;
    if (body->next == NULL)
      break;
  }
  return 0;
}

SNAKE *
snake_born()
{
  int i;
  SNAKE *s;
  POS *bodypos=NULL, *tmppos=NULL;
  s = malloc(sizeof(*s));
  s->snake_body.head_char = SNAKE_HEAD_CHAR;
  s->snake_body.body_char = SNAKE_BODY_CHAR;
  s->snake_body.length = SNAKE_INIT_LENGTH;
  s->snake_body.Head.x = COLS/2;
  s->snake_body.Head.y = LINES/2;
  s->snake_body.direct = D_LEFT;
  s->snake_body.eaten = 0;

  for (i=SNAKE_INIT_LENGTH; i>=0; i--){
	  bodypos = malloc(sizeof(POS));
	  bodypos->x = s->snake_body.Head.x+i+1;
	  bodypos->y = s->snake_body.Head.y;
	  
//	  if (tmppos == NULL){
//	    s->snake_body.Tail.x = bodypos->x;
//	    s->snake_body.Tail.y = bodypos->y;
//	  }
	  bodypos->next = tmppos;
	  tmppos = bodypos;
  }
  s->snake_body.body_list = bodypos;
  s->smove = snake_move;
  s->eat = snake_eat;
  s->dead = snake_die;
  return s;
}

int
snake_die(void *s)
{
/*  int x, y;
  SNAKE *snake= (SNAKE*)s;
  x = snake->snake_body.Head.x;
  y = snake->snake_body.Head.y;
*/
  return is_body_overlay( ((SNAKE*)s)->snake_body.Head.x, \
                          ((SNAKE*)s)->snake_body.Head.y, s);

}

void *
snake_move(void *s)
{
  int i;
  POS *body=NULL;
  SNAKE *snake=NULL;
  snake = (SNAKE*)s;
  body = malloc(sizeof(POS));
  //save old head position
  body->x = snake->snake_body.Head.x;
  body->y = snake->snake_body.Head.y;
  body->next = snake->snake_body.body_list;
  //get a new head
  snake->snake_body.body_list = body;
  //move head
  switch(snake->snake_body.direct){
  case D_LEFT:
    snake->snake_body.Head.x--;
    break;
  case D_RIGHT:
    snake->snake_body.Head.x++;
    break;
  case D_UP:
    snake->snake_body.Head.y--;
    break;
  case D_DOWN:
    snake->snake_body.Head.y++;
    break;
  default:
    break;
  }
  //implement go through screen
  if (snake->snake_body.Head.x > max_win_x)
    snake->snake_body.Head.x = 0;
  if (snake->snake_body.Head.x < 0)
    snake->snake_body.Head.x = max_win_x;
  if (snake->snake_body.Head.y > max_win_y)
    snake->snake_body.Head.y = 0;
  if (snake->snake_body.Head.y < 0)
    snake->snake_body.Head.y = max_win_y;
/*
  //cut the tail of snake
  for (i=0; i<snake->snake_body.length-1;i++){
    body=body->next;
  }
  if (body->next != NULL)
    body->next = NULL;
*/
  return NULL;

}

void *
snake_grow(void *s)
{
  SNAKE *snake = (SNAKE*)s;
  POS *body = malloc(sizeof(POS));
  body->x = snake->snake_body.Head.x;
  body->y = snake->snake_body.Head.y;
  body->next = snake->snake_body.body_list;
  snake->snake_body.body_list = body->next;
  snake->snake_body.length++;
  return NULL;  
}

void *
snake_eat(void *s)
{
  SNAKE *snake = (SNAKE*)s;
  snake->snake_body.eaten++;
  snake_grow(snake);
  return NULL;
}

int
snake_erase(SNAKE *snake)
{
  int i;
  POS *body=NULL;
  //print head
  mvaddch(snake->snake_body.Head.y, snake->snake_body.Head.x,' ');
  //snake body link	
  body = snake->snake_body.body_list;
  
  //print snake body
  for (i=0; i<snake->snake_body.length;i++){
  mvaddch(body->y, body->x, ' ');
  body = body->next;
  }
  return 0;
}

int
snake_print(SNAKE *snake)
{
  int i;
  POS *body=NULL;
  //print head
  mvaddch(snake->snake_body.Head.y, snake->snake_body.Head.x,\
  	   snake->snake_body.head_char);
  //snake body link	
  body = snake->snake_body.body_list;
  
  //print snake body
  for (i=0; i<snake->snake_body.length;i++){
  mvaddch(body->y, body->x, snake->snake_body.body_char);
  body = body->next;
  }
  return 0;
}

FOOD *
new_food(int y, int x)
{
  FOOD *f;
  f = malloc(sizeof(FOOD));
  f->locate[1] = rand()%(y/2);
  f->locate[0] = rand()%(x/2);
  f->eaten = 0;
  return f;
}

int
check_overlay(void *f, void *s)
{
  int i;
  POS *body;
  FOOD *food = (FOOD*)f;
  SNAKE *snake = (SNAKE*)s;

  if (IS_EATABLE(snake->snake_body.Head.x, snake->snake_body.Head.y,\
  		  food->locate[0], food->locate[1]))
    return 1;

  body = snake->snake_body.body_list;
  for (i=0; i<snake->snake_body.length; i++)
  {
    if (is_pos_overlay(food->locate[0], food->locate[1], body))
      return 1;
    body=body->next;
    if (body->next == NULL)
      break;
  }

  return 0;
}

void 
replace_food(FOOD *f, SNAKE *s, int ymax, int xmax)
{
  int x, y;
  SNAKE *snake=(SNAKE*)s;

  f->eaten = 0;
  do{
    x = rand()%(xmax/2);
    y = rand()%(ymax/2);
    f->locate[1] = y; 
    f->locate[0] = x;
    if (!check_overlay(f, snake))
      break;
  }while(1);
}

int
key_control(SNAKE *snake)
{
  int ch;
  int rc=0;

  ch=getch();

  switch(ch){
  case 0x61: //KEY_LEFT:
    if (snake->snake_body.direct != D_RIGHT)
    snake->snake_body.direct = D_LEFT;
    break;
  case 0x64: //KEY_RIGHT:
    if (snake->snake_body.direct != D_LEFT)
    snake->snake_body.direct = D_RIGHT;
    break;
  case 0x77: //KEY_UP:
    if (snake->snake_body.direct != D_DOWN)
    snake->snake_body.direct = D_UP;
    break;
  case 0x73: //KEY_DOWN:
    if (snake->snake_body.direct != D_UP)
    snake->snake_body.direct = D_DOWN;
    break;
  case 0x71: //KEY_BACKSPACE:
    rc = 1;
    break;
  default:
    break;
  }

  return rc;
}

int
free_resources(FOOD *food, SNAKE *snake)
{
  int i;
  POS *body, *tmpptr;
  if (food)
    free(food);
  if (snake){
   body = snake->snake_body.body_list;
   free(snake);

  for (i=0; i<snake->snake_body.length; i++)
  {
    tmpptr = body;
    body=body->next;
    free(tmpptr);
    if (body->next == NULL)
      break;
  }
  if (body)
    free(body);
  }
  return 0;
}

int
check_level(int eaten, int level)
{
  int rc=level;
  if (eaten >= 3)
    rc = 3;
  if (eaten >= 5)
    rc = 2;
  if (eaten >= 7)
    rc = 1;

  return rc;
}
