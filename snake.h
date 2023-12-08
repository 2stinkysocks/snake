#include "queue.h"

typedef struct Position {
	int x;
	int y;
} position;

int mvaddwstr(int y, int x, wchar_t* ch);
bool queueContainsPosition(node_t** head, int y, int x);
bool queueContainsPositionNotHead(node_t** head, int y, int x);
void pickFruitLocation(node_t** head, position** fruitPos);
void printSnake(node_t** head);
void moveSnake(node_t** head, int dir, bool eat);
char* itoa(int i, char b[]);
void printSnake(node_t** head);
bool collide(node_t** head);
void extendSnake(node_t** head, int dir);
void printSnake(node_t** head);
bool didEat(node_t** head, position* fruitpos);
position* cutSnakeIfNoFood(node_t** head);
int msleep(long msec);
