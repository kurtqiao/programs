
#define FOOD_CHAR	'%'
#define SNAKE_HEAD_CHAR '@'
#define SNAKE_BODY_CHAR '#'
#define SNAKE_INIT_LENGTH 7


#define IS_EATABLE(x,y,fx, fy)    ((x == fx)&&(y == fy))

enum DIRECT{
D_LEFT,
D_RIGHT,
D_UP,
D_DOWN
};

typedef struct _food{
  int locate[2];
  int eaten;
}FOOD;

typedef struct _pos{
  int x, y;
  struct _pos *next;
}POS;

typedef struct _snake_body{
  char head_char;
  char body_char;
  POS Head;
  POS Tail;
  int length;
  int direct;
  int eaten;
  POS *body_list;
}SNAKE_BODY;

typedef struct _snake{
  SNAKE_BODY snake_body;
//  void * (*init)(void *self);
  void * (*smove)(void *self);
  void * (*eat)(void *self);
  void * (*grow)(void *self);
  int (*dead)(void *self);
}SNAKE;

SNAKE *
snake_born();

int
snake_die(void *s);

void *
snake_move(void *s);

void *
snake_eat(void *s);

int
snake_erase(SNAKE *snake);

int
snake_print(SNAKE *snake);

FOOD *
new_food(int y, int x);

void 
replace_food(FOOD *f, SNAKE *s, int y, int x);

int
key_control();

int
check_level(int eaten, int level);

int
free_resources(FOOD *food, SNAKE *snake);
