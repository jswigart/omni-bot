// WpsRead.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WpsRead.h"


waypoint_t *waypoints;
int numWaypoints = 0;
char mapname[32] = {0};


int main(int argc, char* argv[])
{
	char outname[256];
	FILE *fWpsFile, *fTxtFile;
	bool b;

	if (argc < 2)
	{
		printf("Usage: %s <wps-file> [txt-file]\n", argv[0]);
		return -1;
	}

	if (argc == 3)
		strcpy(outname, argv[2]);			//if they supplied a name for the txt file, use it
	else
	{
		int pos;
		strcpy(outname, argv[1]);
		pos = strlen(outname) - 3;
		strcpy(outname+pos, "gm");			//otherwise use the input name with .txt
	}

	if (!(fWpsFile = fopen(argv[1], "rb")))					//make sure we can open the wps file
	{
		printf("Unable to open waypoint file %s\n", argv[1]);
		return -2;
	}

	b = ParseWPSFile(fWpsFile);			//read/save all the info
	fclose(fWpsFile);					//close it cuz we don't need it anymore

	if (!b)
		return -3;

	if (!(fTxtFile = fopen(outname, "w")))			//try to create txt file
	{
		printf("Unable to create node file %s\n", outname);
		Cleanup();
		return -4;
	}

	WriteNodFile(fTxtFile);						//write data to output file
	fclose(fTxtFile);							//close it

	Cleanup();									//free memory that was dynamically allocated

	return 0;
}

bool ParseWPSFile(FILE* f)
{
	char buf[256];
	char name[8];
	int ver;

	int i, j;


	fread(name, sizeof(char), 8, f);
	fread(&ver, sizeof(int), 1, f);

	if ((strncmp(name, "kajibot", 7) != 0) || (ver != 1))
	{
		printf("Not a valid wolfbot waypoint file\n");
		return false;
	}

	fread(&numWaypoints, sizeof(int), 1, f);

	fread(buf, sizeof(char), 4, f);				//unknown header data

	fread(&mapname, sizeof(char), 32, f);		//read in all 32 bytes, though most are unused

	waypoints = new waypoint_t[numWaypoints];	//allocate memory for the waypoints
	for (i = 0; i < numWaypoints; i++)
	{
		fread(&waypoints[i].pos, sizeof(vec3_t), 1, f);
		fread(&waypoints[i].num, sizeof(int), 1, f);
		fread(&waypoints[i].flags, sizeof(int), 1, f);
		fread(&waypoints[i].entNums[0], sizeof(int), 1, f);
		waypoints[i].entNums[1] = waypoints[i].entNums[0];		//0 and 1 are stored in the same #, extracted later
		fread(&waypoints[i].entNums[2], sizeof(int), 1, f);
		fread(&waypoints[i].objFlags, sizeof(int), 1, f);
		fread(&waypoints[i].numLinks, sizeof(int), 1, f);
		fread(&waypoints[i].crap, sizeof(int), 1, f);			//unknown data

		waypoints[i].links = new link_t[waypoints[i].numLinks];	//allocate memory for links

		for (j = 0; j < waypoints[i].numLinks; j++)
		{
			fread(&waypoints[i].links[j].num, sizeof(int), 1, f);
			fread(&waypoints[i].links[j].dist, sizeof(float), 1, f);
			fread(&waypoints[i].links[j].flags, sizeof(int), 1, f);
		}
	}

	return true;
}

void WriteNodFile(FILE* f)
{
	int i, j;
	char flagsStr[512] = {0};
	char objStr[512] = {0};

	//fprintf(f, "Map %s, %d waypoints\n", mapname, numWaypoints);
	fputc('\n', f);

	for (i = 0; i < numWaypoints; i++)
	{
		waypoints[i].entNums[0] &= 0x000FFFFF;
		waypoints[i].entNums[0] >>= 10;
		waypoints[i].entNums[1] &= 0xFFF00000;
		waypoints[i].entNums[1] >>= 20;				//weird bit math...does what the explanation file says
		//entnums[2] doesn't get changed

		fprintf(f, "Wp.AddWaypoint(Vector3(%f,%f,%f),Vector3(0,0,0));", 
			waypoints[i].pos[0], waypoints[i].pos[1], waypoints[i].pos[2]);

		fputc('\n', f);
		//door flag
		//Wp.SetWaypointFlag(409,"sneak",true);
		if ( waypoints[i].flags & 8 )
		{
			fprintf(f, "Wp.SetWaypointFlag(%d,\"door\",true);",waypoints[i].num + 1);
			fputc('\n', f);
		}
	}

	//second pass for link to flags and connections
	for (i = 0; i < numWaypoints; i++)
	{
		waypoints[i].entNums[0] &= 0x000FFFFF;
		waypoints[i].entNums[0] >>= 10;
		waypoints[i].entNums[1] &= 0xFFF00000;
		waypoints[i].entNums[1] >>= 20;				//weird bit math...does what the explanation file says
		//entnums[2] doesn't get changed

		for (j = 0; j < waypoints[i].numLinks; j++)
		{
			fprintf(f, "Wp.Connect(%d,%d);",waypoints[i].num + 1 ,waypoints[i].links[j].num + 1);
			fputc('\n', f);

			if ( waypoints[i].links[j].flags & 1 )
			{
				fprintf(f, "Wp.SetWaypointFlag(%d,\"crouch\",true);",waypoints[i].links[j].num + 1);
				fputc('\n', f);
			}

			if ( waypoints[i].links[j].flags & 4 )
			{
				fprintf(f, "Wp.SetWaypointFlag(%d,\"jump\",true);",waypoints[i].links[j].num + 1);
				fputc('\n', f);
			}

			if ( waypoints[i].links[j].flags & 8 )
			{
				fprintf(f, "Wp.SetWaypointFlag(%d,\"sneak\",true);",waypoints[i].links[j].num + 1);
				fputc('\n', f);
			}

			if ( waypoints[i].links[j].flags & 16 )
			{
				fprintf(f, "Wp.SetWaypointFlag(%d,\"wall\",true);",waypoints[i].links[j].num + 1);
				fputc('\n', f);
			}

			if ( waypoints[i].links[j].flags & 32 )
			{
				fprintf(f, "Wp.SetWaypointFlag(%d,\"climb\",true);",waypoints[i].links[j].num + 1);
				fputc('\n', f);
			}
		}
		//fputc('\n', f);
	}
}

void Cleanup()
{
	int i;

	for (i = 0; i < numWaypoints; i++)
		delete[] waypoints[i].links;

	delete[] waypoints;
}

void ParseWptFlags(int flags, char* s)
{
	if (flags == 0)
	{
		strcpy(s, "None");
		return;
	}

	strcpy(s, "(");

	if (flags & WAYPOINT_CAMP)		//names are defined in WpsRead.h
		strcat(s, "camp, ");

	if (flags & WAYPOINT_UNKNOWN)	//bitwise ANDs (&) are necessary, could use more than one flag
		strcat(s, "unknown, ");

	if (flags & WAYPOINT_OBJECTIVE)
		strcat(s, "objective, ");

	if (flags & WAYPOINT_DOOR)
		strcat(s, "door, ");

	if (flags & WAYPOINT_AXIS_UNREACH)
		strcat(s, "axis unreachable, ");

	if (flags & WAYPOINT_ALLY_UNREACH)
		strcat(s, "allies unreachable, ");

	if (flags & WAYPOINT_MG42)
		strcat(s, "mg42, ");

	if (flags & WAYPOINT_AXIS_DELIVER)
		strcat(s, "axis deliver, ");

	if (flags & WAYPOINT_ALLY_DELIVER)
		strcat(s, "allies deliver, ");

	if (flags >= 512)
		strcat(s, "undiscovered, ");

	s[strlen(s)-1] = '\0';			//these lines are NOT redundant...take one out and watch it f up :)
	s[strlen(s)-1] = ')';			//gets rid of the last comma and space and replaces it with a ) to look better
}

void ParseObjFlags(int flags, char* s)
{
	int team, type, link, priority, obj;
	char buf[32];

	if (flags == 0)
	{
		strcpy(s, "None");
		return;
	}

	strcpy(s, "(team: ");

	link = ((flags  & 0xF0000) >> 17);
	obj = ((flags & 0x0FFF0) >> 7);
	priority = ((flags & 0x0FFF0) >> 6);
	priority -= (obj << 1);
	type = ((flags & 0x0000F) >> 2);
	team = (flags & 1);						//REALLY weird bit math...read the explanation file

	if (team == 0)
		strcat(s, "allies, ");
	else if (team == 1)
		strcat(s, "axis, ");
	else
		strcat(s, "unknown team, ");

	strcat(s, "type: ");

	if (type == OBJECTIVE_BOMB)
		strcat(s, "bomb, ");
	else if (type == OBJECTIVE_STEAL)
		strcat(s, "steal, ");
	else if (type == OBJECTIVE_CAPTURE)
		strcat(s, "capture, ");
	else
		strcat(s, "unknown type, ");

	strcat(s, "priority: ");
	strcat(s, itoa(priority, buf, 10));
	strcat(s, ", ");

	strcat(s, "linked objective: ");
	if (link == 0)
		strcat(s, "none");
	else
		strcat(s, itoa(link, buf, 10));

	strcat(s, ", func_explosive: ");
	if (obj == 0)
		strcat(s, "none");
	else
		strcat(s, itoa(obj, buf, 10));

	strcat(s, ")");
}

void ParsePathFlags(int flags, char* s)
{
	if ((flags & 0xFFFFFF00) == 0xCDCDCD00)
		flags &= 0xFF;						//sets 'unknown' if link doesn't end with CDCDCD
	else
	{
		strcpy(s, "Unknown");
		return;
	}

	if (flags == 0)
	{
		strcpy(s, "None");
		return;
	}

	strcpy(s, "(");

	if (flags & PATH_CROUCH)
		strcat(s, "crouch, ");

	if (flags & PATH_UNKNOWN)
		strcat(s, "unknown, ");

	if (flags & PATH_JUMP)
		strcat(s, "jump, ");

	if (flags & PATH_WALK)
		strcat(s, "walk, ");

	if (flags & PATH_BLOCKED)
		strcat(s, "blocked, ");

	if (flags & PATH_LADDER)
		strcat(s, "ladder, ");

	if (flags >= 64)
		strcat(s, "undiscovered, ");

	s[strlen(s)-1] = '\0';
	s[strlen(s)-1] = ')';
}
