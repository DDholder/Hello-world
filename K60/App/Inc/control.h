#ifndef CONTROL_H
#define CONTROL_H


typedef struct
{
	int x;
	int y;
}location;
extern location pos_set[5][5],pos_ball;

location SelectTarget(location pos_begin, location pos_next);
#endif
