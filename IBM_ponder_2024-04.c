#include <stdio.h>
#include <stdlib.h>

// Number of sets, each composed of 3 rods
#define NUMBER_SETS 3

// Fake disc number to simulate bottom of rod
#define BOTTOM 1000

// Discs are stored with a linked list, top disc first
typedef struct disc {
	int number;
	struct disc *next;
} Disc;

// Each rod has a pointer to its top disc and is chained (double linked list)
//  with the other two rods
typedef struct rod {
	Disc *discs;
	struct rod *clockwise;
	struct rod *counterclock;
} Rod;

// A set has a number (used for display) and a pointer to the initial rod
//  (holding the discs) and another to the rod holding disc number 1
//  (makes it easier to move discs)
typedef struct set {
	int number;
	Rod *initialRod;
	Rod *rodWith1;
} Set;

// Function prototypes
void insertNewDisc(int number, Rod *r);
Rod *initRods(void); 
Set *initSet(int n, int numberOfDiscs);
void insertNewDisc(int number, Rod *r);
void display(Set *s);
int isWinning(Set *s);
void moveDisc(Rod* source, Rod*dest);
void Move0(Set *s);
void Move1(Set *s);
void Move2(Set *s);
void executeMove(char m, Set *s);

/* Initialise a set (of a given number) with three rods
 * and the given number of discs.
 */
Set *initSet(int n, int numberOfDiscs) {
	int i;
	Set *s = malloc(sizeof(Set));
	s->number = n;
	s->initialRod = initRods();
	for (i = numberOfDiscs; i > 0; i--)
		insertNewDisc(i, s->initialRod);
	s->rodWith1 = s->initialRod;
	return s;
}

/* Initialise three empty rods chained with a double linked list
 * and return pointer to first rod
 */
Rod *initRods(void) {
	Rod *p = malloc(sizeof(Rod));
	p->discs = NULL;
	p->clockwise = malloc(sizeof(Rod));
	p->clockwise->discs = NULL;
	p->counterclock = malloc(sizeof(Rod));
	p->counterclock->discs = NULL;
	p->clockwise->clockwise = p->counterclock;
	p->clockwise->counterclock = p;
	p->counterclock->counterclock = p->clockwise;
	p->counterclock->clockwise = p;
	
	// Bottom virtual discs
	Disc *vd = malloc(sizeof(Disc));
	vd->number = BOTTOM;
	vd->next = NULL;
	p->discs = p->clockwise->discs = p->counterclock->discs = vd;
	return p;
}

/* Insert a new disc of a given number (=size) on a given rod.
 */
void insertNewDisc(int number, Rod *r) {
	Disc *d = malloc(sizeof(Disc));
	d->number = number;
	d->next = r->discs;
	r->discs = d;
}

/* display a set, useful for testing
 */
void display(Set *s) {
	int i;
	printf("Set %d\n", s->number);
	Rod *r = s->initialRod;
	Disc *d = r->discs;
	for (i = 0; i < 3; i++) {
		printf("\t%d: [ ", i);
		while (d && d->number != BOTTOM) {
			printf("%d ", d->number);
			d = d->next;
		}
		printf("]\n");
		r = r->clockwise;
		d = r->discs;
	}
	printf("\n");
}

/* A set is winning if all discs are on the clockwise rod. 
 * So this function checks whether the initial rod and its counterclockwise
 * rod are empty. Returns 1 in this case, otherwise 0.
 */
int isWinning(Set *s) {
	return (s->initialRod->discs->number == BOTTOM) &&
			(s->initialRod->counterclock->discs->number == BOTTOM);
}

/* Move top disc from the source rod to the destination one.
 */
void moveDisc(Rod* source, Rod*dest) {
	Disc *d = source->discs;
	source->discs = d->next;
	
	d->next = dest->discs;
	dest->discs = d;	
}

/* Execute move 0: move disc 1 clockwise.
 */
void Move0(Set *s) {
	Rod *r1 = s->rodWith1;
	moveDisc(r1, r1->clockwise);
	s->rodWith1 = r1->clockwise;
}

/* Execute move 1: move disc 1 counterclockwise.
 */
void Move1(Set *s) {
	Rod *r1 = s->rodWith1;
	moveDisc(r1, r1->counterclock);
	s->rodWith1 = r1->counterclock;
}

/* Execute move 2: move disc which is not 1 to another rod.
 *  If the two rods not holding disc 1 are empty, this move does nothing
 *  Otherwise it moves the smallest disc to the other rod not holding disc 1.
 */
void Move2(Set *s) {
	Rod *r1 = s->rodWith1;
	Disc *dcw  = r1->clockwise->discs;
	Disc *dccw = r1->counterclock->discs;
	
	if ((dcw->number == BOTTOM) && (dccw->number == BOTTOM))
		return; // two other rods empty
	if (dcw->number < dccw->number)
		moveDisc(r1->clockwise, r1->counterclock);
	else
		moveDisc(r1->counterclock, r1->clockwise);
}

/* Execute a move for a given set, move should be character 0, 1, or 2.
 */
void executeMove(char m, Set *s) {
	switch (m) {
		case '0': Move0(s);
				  break;
		case '1': Move1(s);
				  break;
		case '2': Move2(s);
				  break;
		default:  printf("%c(%d): Move ERROR!\n", m, m);
				  exit(1);
		}
}

int main(void) {
	int i;
	
	// Create array of sets, number of discs and list of moves.
	Set *sets[NUMBER_SETS];
	int number_discs[NUMBER_SETS] = {7, 10, 9};
	for (i = 0; i < NUMBER_SETS; i++)
		sets[i] = initSet(i, number_discs[i]);
	char *moves[NUMBER_SETS];
	int indexes[NUMBER_SETS] = {0};
	moves[0] = "12021121120020211202121";
	moves[1] = "0211202112002";
	moves[2] = "20202020021212121121202120200202002121120202112021120020021120211211202002112021120211200212112020212120211";
	long long movesCounter = 0;
	
	// Just execute one move for each set and check winning positions
	while(1) {
		for (i = 0; i < NUMBER_SETS; i++)
			if (!moves[i][indexes[i]])
				indexes[i] = 0;
		for (i = 0; i < NUMBER_SETS; i++)
			executeMove(moves[i][indexes[i]++], sets[i]);
		movesCounter++;
		for (i = 0; i < NUMBER_SETS; i++)
			if (!isWinning(sets[i]))
				break;
		if (i == NUMBER_SETS) {
			printf("Winning in %lld moves.\n", movesCounter);
			break;
		}
	}
}
