#ifndef __WPSREAD_H_83011__
#define __WPSREAD_H_83011__

#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define WAYPOINT_NORMAL			0
#define WAYPOINT_CAMP			1
#define WAYPOINT_UNKNOWN		2
#define WAYPOINT_OBJECTIVE		4
#define WAYPOINT_DOOR			8
#define WAYPOINT_AXIS_UNREACH	16
#define WAYPOINT_ALLY_UNREACH	32
#define WAYPOINT_MG42			64
#define WAYPOINT_AXIS_DELIVER	128
#define WAYPOINT_ALLY_DELIVER	256

#define PATH_NORMAL				0
#define PATH_CROUCH				1
#define PATH_UNKNOWN			2
#define PATH_JUMP				4
#define PATH_WALK				8
#define PATH_BLOCKED			16
#define PATH_LADDER				32

#define OBJECTIVE_BOMB			0
#define OBJECTIVE_STEAL			1
#define OBJECTIVE_CAPTURE		2


typedef float vec3_t[3];

typedef struct
{
	int num;
	float dist;
	int flags;
} link_t;

typedef struct
{
	vec3_t pos;
	int num;
	int flags;
	int entNums[3];
	int objFlags;

	int numLinks;
	int crap;
	link_t *links;
} waypoint_t;

bool ParseWPSFile(FILE* f);
void WriteNodFile(FILE* f);

void ParseWptFlags(int flags, char* s);
void ParseObjFlags(int flags, char* s);
void ParsePathFlags(int flags, char* s);

void Cleanup();

#endif