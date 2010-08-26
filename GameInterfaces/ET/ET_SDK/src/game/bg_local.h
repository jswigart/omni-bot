// bg_local.h -- local definitions for the bg (both games) files

#define	MIN_WALK_NORMAL	0.7		// can't walk on very steep slopes

#define	STEPSIZE		18

#define	JUMP_VELOCITY	270

#define	TIMER_LAND		130
#define	TIMER_GESTURE	(34*66+50)

#define DOUBLE_TAP_DELAY	400

#define MAX_MG42_HEAT	1500.f


// all of the locals will be zeroed before each
// pmove, just to make damn sure we don't have
// any differences when running on client or server
typedef struct {
	vec3_t		forward, right, up;
	float		frametime;

	int			msec;

	qboolean	walking;
	qboolean	groundPlane;
	trace_t		groundTrace;

	float		impactSpeed;

	vec3_t		previous_origin;
	vec3_t		previous_velocity;
	int			previous_waterlevel;

	// Ridah, ladders
	qboolean	ladder;
} pml_t;

extern	pmove_t		*pm;
extern	pml_t		pml;

// movement parameters
extern	float	pm_stopspeed;
//extern	float	pm_duckScale;

//----(SA)	modified
extern	float	pm_waterSwimScale;
extern	float	pm_waterWadeScale;
extern	float	pm_slagSwimScale;
extern	float	pm_slagWadeScale;

extern	float	pm_accelerate;
extern	float	pm_airaccelerate;
extern	float	pm_wateraccelerate;
extern	float	pm_slagaccelerate;
extern	float	pm_flyaccelerate;

extern	float	pm_friction;
extern	float	pm_waterfriction;
extern	float	pm_slagfriction;
extern	float	pm_flightfriction;

//----(SA)	end

extern	int		c_pmove;

void PM_AddTouchEnt( int entityNum );
void PM_AddEvent( int newEvent );

qboolean	PM_SlideMove( qboolean gravity );
void		PM_StepSlideMove( qboolean gravity );

qboolean	PM_SlideMoveProne( qboolean gravity );
void		PM_StepSlideMoveProne( qboolean gravity );

void PM_BeginWeaponChange( int oldweapon, int newweapon, qboolean reload );
