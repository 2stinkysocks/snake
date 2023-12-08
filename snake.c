#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <errno.h>
#include "queue.h"
#include "snake.h"
#include "keyboard.h"

#define borderChar L"\u2588"
#define snakeChar L"\u25A0"
#define foodChar L"\u25CF" 

#define boardHeight 18
#define boardWidth 34

/*
The data for the game is stored like this:
int dir: the direction that the snake is moving, 0=right, 1=down, 2=left, 3=up
A queue of positions (x,y coords in screenspace) that represents the snake's body. 
When the snake moves, dequeue removes the last element, and enqueue adds a new one onto the front, depending on the direction
When the snake moves into an apple, dequeue is not called, extending the queue's length by 1.
*/

int main(void) {
	srand(time(0));

	node_t* head = NULL;
	position* fruitpos = malloc(sizeof(position));
	char* lengthStr = malloc(16);


	setkbmode(1);

	setlocale(LC_ALL, "");
    
	WINDOW* mainwin;
	if((mainwin = initscr()) == NULL) {
		fprintf(stderr, "Error initialising ncurses.\n");
		exit(EXIT_FAILURE);
	}

	curs_set(0);
	nodelay(mainwin, TRUE);
	noecho();

	for(int i = 0; i < boardHeight; i++) {
		if(i == 0 || i == boardHeight-1) {
			for(int j = 0; j < boardWidth; j++) {
				mvaddwstr(i,j, borderChar);
			}
		} else {
			mvaddwstr(i,0, borderChar);
			mvaddwstr(i,boardWidth-1, borderChar);
		}
	}
	refresh();

	int dir = 0; // 0=right, 1=down, 2=left, 3=up
	int length = 1;

	// add head location to display queue
	position* snakeHeadPos = malloc(sizeof(position));
	snakeHeadPos->x = 16;
	snakeHeadPos->y = 8;
	enqueue(&head, (void*)(snakeHeadPos));

	fruitpos->x = 20;
	fruitpos->y = 8;
	mvaddwstr(fruitpos->y, fruitpos->x, foodChar);

	mvaddstr(boardHeight, 8, "Snake");
	mvaddstr(boardHeight, 18, "Length: ");
	mvaddstr(boardHeight, 26, itoa(length, lengthStr));
	printSnake(&head);

	

	refresh();

	sleep(3);

	// game loop
	while(!collide(&head)) {
		extendSnake(&head, dir); // move the snake
		bool ate = didEat(&head, fruitpos); // if it doesn't, check if it eats
		if(ate) { // if ate, spawn a new fruit
			mvaddstr(fruitpos->y, fruitpos->x, " ");
			pickFruitLocation(&head, &fruitpos);
			mvaddwstr(fruitpos->y, fruitpos->x, foodChar);
		} else { // if did not eat, remove the end of the snake
			position* cutPos = cutSnakeIfNoFood(&head);
			mvaddstr(cutPos->y, cutPos->x, " ");
			free(cutPos);
		}
		printSnake(&head);
		refresh();
		// take input
		if (getch() == '\033') { // if the first value is esc
			getch(); // skip the [
			switch(getch()) { // the real value
				case 'A':
					dir = 3;
					break;
				case 'B':
					dir = 1;
					break;
				case 'C':
					dir = 0;
					break;
				case 'D':
					dir = 2;
					break;
			}
		}
		msleep(150);
	}

	mvaddstr(boardHeight+1, 12, "You lose!");
	refresh();
	sleep(3);
	delwin(mainwin);
	endwin();
	refresh();
	setkbmode(0);
	free(lengthStr);
	free(head);
	return EXIT_SUCCESS;
}

void printSnake(node_t** head) {
	node_t *current, *prev = NULL;

	if (head == NULL || *head == NULL) return;

	current = *head;
	mvaddwstr(((position*)(current->val))->y, ((position*)(current->val))->x, snakeChar);
	while (current->next != NULL) {
		prev = current;
		current = current->next;
		mvaddwstr(((position*)(current->val))->y, ((position*)(current->val))->x, snakeChar);
	}
}

// adds to the front of the snake, ignoring collision, based on direction
void extendSnake(node_t** head, int dir) {
	position* newPos = malloc(sizeof(position));
	position* oldPos = peekBack(head);
	switch (dir) {
	case 0: // r
		newPos->x = oldPos->x + 1;
		newPos->y = oldPos->y;
		break;
	case 1: // d
		newPos->x = oldPos->x;
		newPos->y = oldPos->y + 1;
		break;
	case 2: // l
		newPos->x = oldPos->x - 1;
		newPos->y = oldPos->y;
		break;
	case 3: // u
		newPos->x = oldPos->x;
		newPos->y = oldPos->y - 1;
		break;
	}
	enqueue(head, (void*)newPos);
}

position* cutSnakeIfNoFood(node_t** head) {
	return (position*)dequeue(head);
}

bool collide(node_t** head) {
	position* headPosition = (position*)(peekBack(head));  // back of the queue is the last element added, which is the front of the snake
	return headPosition->x >= boardWidth-1 || headPosition->x <= 0 || headPosition->y >= boardHeight-1 || headPosition->y <= 0 || queueContainsPositionNotHead(head, headPosition->y, headPosition->x);
}

// tells if that iteration ate a food
bool didEat(node_t** head, position* fruitPos) {
	position* headPosition = (position*)(peekBack(head));
	return headPosition->x == fruitPos->x && headPosition->y == fruitPos->y;
}

bool queueContainsPosition(node_t** head, int y, int x) {
	node_t *current, *prev = NULL;
	position* retval = NULL;

	if (head == NULL || *head == NULL) return false;

	if((((position*)((*head)->val))->x) == x && (((position*)((*head)->val))->y) == y) return true;

	current = *head;
	while (current->next != NULL) {
		prev = current;
		current = current->next;
		if((((position*)((current)->val))->x) == x && (((position*)((current)->val))->y) == y) return true;
	}

	return false;
}

bool queueContainsPositionNotHead(node_t** head, int y, int x) {
	node_t *current, *prev = NULL;
	position* retval = NULL;

	if (head == NULL || *head == NULL) return false;

	current = *head;
	while (current->next != NULL) {
		prev = current;
		current = current->next;
		if((((position*)((current)->val))->x) == x && (((position*)((current)->val))->y) == y) return true;
	}

	return false;
}

void pickFruitLocation(node_t** head, position** fruitPos) {
	position* newPos = malloc(sizeof(position));

	int x = 16;
	int y = 8;
	do {
		x = (rand() % (boardWidth-2 - 1 + 1)) + 1;
		y = (rand() % (boardHeight-2 - 1 + 1)) + 1;
	} while (queueContainsPosition(head, y, x));
	newPos->x = x;
	newPos->y = y;
	free(*fruitPos);
	*fruitPos = newPos;
}

// https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

// https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

/*
node_t* head = NULL;
	int* first = malloc(sizeof(int)); // pointer to an integer
	int* second = malloc(sizeof(int));
	int* third = malloc(sizeof(int));
	*first = 1;						  // set the value in the pointer to 1
	*second = 2;
	*third = 3;
	enqueue(&head, (void*)first);     // pass the address of head, and the int pointer cast to a void pointer (unknown type)
	enqueue(&head, (void*)second);
	enqueue(&head, (void*)third);
	dequeue(&head);					  // pass address of head
	printf("%i\n", *((int*)dequeue(&head)));  // pass address of head to get a void pointer, convert to an int pointer, and follow the pointer
	printf("%i\n", *((int*)dequeue(&head)));
	free(first);					  // free pointer of value
	free(second);
	free(third);
	*/