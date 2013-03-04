// Copyright (C) 2004 Id Software, Inc.
//

#ifndef __USERCMDGEN_H__
#define __USERCMDGEN_H__

/*
===============================================================================

	Samples a set of user commands from player input.

===============================================================================
*/

const int USERCMD_HZ_SP			= 60;			// 60 frames per second
const int USERCMD_MSEC_SP		= 1000 / USERCMD_HZ_SP;

//const int USERCMD_HZ_MP			= 30;			// 30 frames per second
//const int USERCMD_MSEC_MP		= 1000 / USERCMD_HZ_MP;

//const int USERCMD_HZ_MP			= 60;			// 60 frames per second
//const int USERCMD_MSEC_MP		= 1000 / USERCMD_HZ_MP;

// usercmd_t->button bits
const int BUTTON_ATTACK			= BIT(0);
const int BUTTON_RUN			= BIT(1);
const int BUTTON_ZOOM			= BIT(2);
const int BUTTON_SCORES			= BIT(3);
const int BUTTON_MLOOK			= BIT(4);
// RAVEN BEGIN
// ddynerman: stats
const int BUTTON_INGAMESTATS	= BIT(5);
// jscott: for voicechat
const int BUTTON_VOICECHAT		= BIT(6);
// ddynerman: tourney display
const int BUTTON_TOURNEY		= BIT(7);
// twhitaker: strafe
const int BUTTON_STRAFE			= BIT(8);
// RAVEN END


// usercmd_t->impulse commands
const int IMPULSE_0				= 0;			// weap 0
const int IMPULSE_1				= 1;			// weap 1
const int IMPULSE_2				= 2;			// weap 2
const int IMPULSE_3				= 3;			// weap 3
const int IMPULSE_4				= 4;			// weap 4
const int IMPULSE_5				= 5;			// weap 5
const int IMPULSE_6				= 6;			// weap 6
const int IMPULSE_7				= 7;			// weap 7
const int IMPULSE_8				= 8;			// weap 8
const int IMPULSE_9				= 9;			// weap 9
const int IMPULSE_10			= 10;			// weap 10
const int IMPULSE_11			= 11;			// weap 11
const int IMPULSE_12			= 12;			// weap 12
const int IMPULSE_13			= 13;			// weap reload
const int IMPULSE_14			= 14;			// weap next
const int IMPULSE_15			= 15;			// weap prev
const int IMPULSE_16			= 16;			// <unused>
const int IMPULSE_17			= 17;			// ready to play ( toggles ui_ready )
const int IMPULSE_18			= 18;			// center view
const int IMPULSE_19			= 19;			// show PDA/INV/MAP
const int IMPULSE_20			= 20;			// toggle team ( toggles ui_team )
const int IMPULSE_21			= 21;			// tourney toggle waiting room/spec
const int IMPULSE_22			= 22;			// spectate
const int IMPULSE_23			= 23;			// <unused>
const int IMPULSE_24			= 24;			// <unused>
const int IMPULSE_25			= 25;			// <unused>
const int IMPULSE_26			= 26;			// <unused>
const int IMPULSE_27			= 27;			// <unused>
const int IMPULSE_28			= 28;			// vote yes
const int IMPULSE_29			= 29;			// vote no
const int IMPULSE_40			= 40;			// repeast last radio chatter

// RAVEN BEGIN
// bdube: added flashlight
const int IMPULSE_50			= 50;			// activate flashlight
const int IMPULSE_51			= 51;			// switch to last weapon
// ddynerman: mp stats
const int IMPULSE_52			= 52;			// mp statistics
// RAVEN END


// RAVEN BEGIN
// asalmon: impulses for weapons combos for xbox
#ifdef _XBOX
const int IMPULSE_70			= 70;			//Weapon switch up
const int IMPULSE_71			= 71;			//Weapon switch down
const int IMPULSE_72			= 72;			//Weapon switch right
const int IMPULSE_73			= 73;			//Weapon Switch left
#endif
//RAVEN END


// RITUAL BEGIN
// squirrel: Mode-agnostic buymenus
const int IMPULSE_100			= 100;			// Buy weapon_shotgun
const int IMPULSE_101			= 101;			// Buy weapon_machinegun
const int IMPULSE_102			= 102;			// Buy weapon_hyperblaster
const int IMPULSE_103			= 103;			// Buy weapon_grenadelauncher
const int IMPULSE_104			= 104;			// Buy weapon_nailgun
const int IMPULSE_105			= 105;			// Buy weapon_rocketlauncher
const int IMPULSE_106			= 106;			// Buy weapon_railgun
const int IMPULSE_107			= 107;			// Buy weapon_lightninggun
const int IMPULSE_108			= 108;			// UNUSED
const int IMPULSE_109			= 109;			// Buy weapon_napalmgun
const int IMPULSE_110			= 110;			// Buy weapon_dmg
const int IMPULSE_111			= 111;			// UNUSED
const int IMPULSE_112			= 112;			// UNUSED
const int IMPULSE_113			= 113;			// UNUSED
const int IMPULSE_114			= 114;			// UNUSED
const int IMPULSE_115			= 115;			// UNUSED
const int IMPULSE_116			= 116;			// UNUSED
const int IMPULSE_117			= 117;			// UNUSED
const int IMPULSE_118			= 118;			// Buy item_armor_small
const int IMPULSE_119			= 119;			// Buy item_armor_large
const int IMPULSE_120			= 120;			// Buy ammorefill
const int IMPULSE_121			= 121;			// UNUSED
const int IMPULSE_122			= 122;			// UNUSED
const int IMPULSE_123			= 123;			// Buy team powerup: ammo_regen
const int IMPULSE_124			= 124;			// Buy team powerup: health_regen
const int IMPULSE_125			= 125;			// Buy team powerup: damage_boost
const int IMPULSE_126			= 126;			// UNUSED
const int IMPULSE_127			= 127;			// UNUSED
// RITUAL END


const int KEY_MOVESPEED			= 127;

// usercmd_t->flags
const int UCF_IMPULSE_SEQUENCE	= 0x0001;		// toggled every time an impulse command is sent

class usercmd_t {
public:
	int			gameFrame;						// frame number
	int			gameTime;						// game time
	int			realTime;						// real game time
	int			duplicateCount;					// duplication count for networking
// RAVEN BEGIN
// ddynerman: expand buttons to 2 bytes
	short		buttons;						// buttons
// RAVEN END
	signed char	forwardmove;					// forward/backward movement
	signed char	rightmove;						// left/right movement
	signed char	upmove;							// up/down movement
	short		angles[3];						// view angles
	short		mx;								// mouse delta x
	short		my;								// mouse delta y
	signed char impulse;						// impulse command
	byte		flags;							// additional flags
	int			sequence;						// just for debugging

public:
	void		ByteSwap();						// on big endian systems, byte swap the shorts and ints
	bool		operator==( const usercmd_t &rhs ) const;
};

typedef enum {
	INHIBIT_SESSION = 0,
	INHIBIT_ASYNC
} inhibit_t;

const int MAX_BUFFERED_USERCMD = 64;

class idUsercmdGen {
public:
	virtual				~idUsercmdGen( void ) {}

	// Sets up all the cvars and console commands.
	virtual	void		Init( void ) = 0;

	// Prepares for a new map.
	virtual void		InitForNewMap( void ) = 0;

	// Shut down.
	virtual void		Shutdown( void ) = 0;

	// Clears all key states and face straight.
	virtual	void		Clear( void ) = 0;

	// Clears view angles.
	virtual void		ClearAngles( void ) = 0;

	// When the console is down or the menu is up, only emit default usercmd, so the player isn't moving around.
	// Each subsystem (session and game) may want an inhibit will OR the requests.
	virtual void		InhibitUsercmd( inhibit_t subsystem, bool inhibit ) = 0;

	// Returns a buffered command for the given game tic.
	virtual usercmd_t	TicCmd( int ticNumber ) = 0;

	// Called async at regular intervals.
	virtual	void		UsercmdInterrupt( void ) = 0;

	// Set a value that can safely be referenced by UsercmdInterrupt() for each key binding.
	virtual	int			CommandStringUsercmdData( const char *cmdString ) = 0;

	// Returns the number of user commands.
	virtual int			GetNumUserCommands( void ) = 0;

	// Returns the name of a user command via index.
	virtual const char *GetUserCommandName( int index ) = 0;

	// Continuously modified, never reset. For full screen guis.
	virtual void		MouseState( int *x, int *y, int *button, bool *down ) = 0;

	// Directly sample a button.
	virtual int			ButtonState( int key ) = 0;

	// Directly sample a keystate.
	virtual int			KeyState( int key ) = 0;

	// Directly sample a usercmd.
	virtual usercmd_t	GetUsercmd( void ) = 0;

//RAVEN BEGIN
//asalmon: slow down the joystick movement for aim assist on xenon
	virtual void		SetSlowJoystick(int slow) = 0;

// nrausch: Stuff an arbitrary impulse in, which will override any other impulse this cycle
	virtual void		StuffImpulse( int impulse ) = 0;
	
	virtual void		StuffKey( const char *keyName, bool down ) = 0;

#ifdef _XENON	
	virtual void		GetInputs( int &y, int &p, int &r, int &f ) = 0;
#endif

//RAVEN END
};

extern idUsercmdGen	*usercmdGen;

#endif /* !__USERCMDGEN_H__ */
