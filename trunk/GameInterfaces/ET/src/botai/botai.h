//===========================================================================
//
// Name:         botai.h
// Function:     bot AI
// Programmer:   Mr Elusive (MrElusive@idsoftware.com)
// Last update:  1999-08-18
// Tab Size:     3
//===========================================================================

#ifndef __BOTAI_H__
#define __BOTAI_H__

//debug line colors
#define LINECOLOR_NONE			-1
#define LINECOLOR_RED			1
#define LINECOLOR_GREEN			2
#define LINECOLOR_BLUE			3
#define LINECOLOR_YELLOW		4
#define LINECOLOR_ORANGE		5

//Print types
#define PRT_MESSAGE				1
#define PRT_WARNING				2
#define PRT_ERROR				3
#define PRT_FATAL				4
#define PRT_EXIT				5

//console message types
#define CMS_NORMAL				0
#define CMS_CHAT				1

//some maxs
#define MAX_NETNAME				36
#define MAX_CLIENTSKINNAME		128
#define MAX_FILEPATH			144
#define MAX_CHARACTERNAME		144

#ifndef BSPTRACE

//bsp_trace_t hit surface
typedef struct bsp_surface_s
{
	char name[16];
	int flags;
	int value;
} bsp_surface_t;

//remove the bsp_trace_s structure definition l8r on
//a trace is returned when a box is swept through the world
typedef struct bsp_trace_s
{
	qboolean		allsolid;	// if true, plane is not valid
	qboolean		startsolid;	// if true, the initial point was in a solid area
	float			fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t		plane;		// surface normal at impact
	float			exp_dist;	// expanded plane distance
	int			sidenum;		// number of the brush side hit
	bsp_surface_t surface;	// the hit point surface
	int			contents;	// contents on other side of surface hit
	int			ent;			// number of entity hit
} bsp_trace_t;

#define BSPTRACE
#endif	// BSPTRACE

//
// imported functions used for the BotAI
//


// from the server
/*
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetServerinfo( char *buffer, int bufferSize );
int		trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
int		trap_BotAllocateClient( int clientNum );
void	trap_BotFreeClient( int clientNum );
*/

#endif
