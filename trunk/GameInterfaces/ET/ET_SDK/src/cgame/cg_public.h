

#define	CMD_BACKUP			64	
#define	CMD_MASK			(CMD_BACKUP - 1)
// allow a lot of command backups for very fast systems
// multiple commands may be combined into a single packet, so this
// needs to be larger than PACKET_BACKUP


#define	MAX_ENTITIES_IN_SNAPSHOT	512

// snapshots are a view of the server at a given time

// Snapshots are generated at regular time intervals by the server,
// but they may not be sent if a client's rate level is exceeded, or
// they may be dropped by the network.
typedef struct {
	int				snapFlags;			// SNAPFLAG_RATE_DELAYED, etc
	int				ping;

	int				serverTime;		// server time the message is valid for (in msec)

	byte			areamask[MAX_MAP_AREA_BYTES];		// portalarea visibility bits

	playerState_t	ps;						// complete information about the current player at this time

	int				numEntities;			// all of the entities that need to be presented
	entityState_t	entities[MAX_ENTITIES_IN_SNAPSHOT];	// at the time of this snapshot

	int				numServerCommands;		// text based server commands to execute when this
	int				serverCommandSequence;	// snapshot becomes current
} snapshot_t;

typedef enum cgameEvent_e {
	CGAME_EVENT_NONE,
	CGAME_EVENT_GAMEVIEW,
	CGAME_EVENT_SPEAKEREDITOR,
	CGAME_EVENT_CAMPAIGNBREIFING,
	CGAME_EVENT_DEMO,				// OSP
	CGAME_EVENT_FIRETEAMMSG,
} cgameEvent_t;

/*
==================================================================

functions imported from the main executable

==================================================================
*/

#define	CGAME_IMPORT_API_VERSION	3

typedef enum {
	CG_PRINT,
	CG_ERROR,
	CG_MILLISECONDS,
	CG_CVAR_REGISTER,
	CG_CVAR_UPDATE,
	CG_CVAR_SET,
	CG_CVAR_VARIABLESTRINGBUFFER,
	CG_CVAR_LATCHEDVARIABLESTRINGBUFFER,
	CG_ARGC,
	CG_ARGV,
	CG_ARGS,
	CG_FS_FOPENFILE,
	CG_FS_READ,
	CG_FS_WRITE,
	CG_FS_FCLOSEFILE,
	CG_FS_GETFILELIST,
	CG_FS_DELETEFILE,
	CG_SENDCONSOLECOMMAND,
	CG_ADDCOMMAND,
	CG_SENDCLIENTCOMMAND,
	CG_UPDATESCREEN,
	CG_CM_LOADMAP,
	CG_CM_NUMINLINEMODELS,
	CG_CM_INLINEMODEL,
	CG_CM_LOADMODEL,
	CG_CM_TEMPBOXMODEL,
	CG_CM_POINTCONTENTS,
	CG_CM_TRANSFORMEDPOINTCONTENTS,
	CG_CM_BOXTRACE,
	CG_CM_TRANSFORMEDBOXTRACE,
// MrE:
	CG_CM_CAPSULETRACE,
	CG_CM_TRANSFORMEDCAPSULETRACE,
	CG_CM_TEMPCAPSULEMODEL,
// done.
	CG_CM_MARKFRAGMENTS,
	CG_R_PROJECTDECAL,			// ydnar: projects a decal onto brush models
	CG_R_CLEARDECALS,			// ydnar: clears world/entity decals
	CG_S_STARTSOUND,
	CG_S_STARTSOUNDEX,	//----(SA)	added
	CG_S_STARTLOCALSOUND,
	CG_S_CLEARLOOPINGSOUNDS,
	CG_S_CLEARSOUNDS,
	CG_S_ADDLOOPINGSOUND,
	CG_S_UPDATEENTITYPOSITION,
// Ridah, talking animations
	CG_S_GETVOICEAMPLITUDE,
// done.
	CG_S_RESPATIALIZE,
	CG_S_REGISTERSOUND,
	CG_S_STARTBACKGROUNDTRACK,
	CG_S_FADESTREAMINGSOUND,	//----(SA)	modified
	CG_S_FADEALLSOUNDS,			//----(SA)	added for fading out everything
	CG_S_STARTSTREAMINGSOUND,
	CG_S_GETSOUNDLENGTH,		// xkan - get the length (in milliseconds) of the sound
	CG_S_GETCURRENTSOUNDTIME,
	CG_R_LOADWORLDMAP,
	CG_R_REGISTERMODEL,
	CG_R_REGISTERSKIN,
	CG_R_REGISTERSHADER,

 	CG_R_GETSKINMODEL,			// client allowed to view what the .skin loaded so they can set their model appropriately
	CG_R_GETMODELSHADER,		// client allowed the shader handle for given model/surface (for things like debris inheriting shader from explosive)

	CG_R_REGISTERFONT,
	CG_R_CLEARSCENE,
	CG_R_ADDREFENTITYTOSCENE,
	CG_GET_ENTITY_TOKEN,
	CG_R_ADDPOLYTOSCENE,
// Ridah
	CG_R_ADDPOLYSTOSCENE,
// done.
	CG_R_ADDPOLYBUFFERTOSCENE,
	CG_R_ADDLIGHTTOSCENE,

	CG_R_ADDCORONATOSCENE,
	CG_R_SETFOG,
	CG_R_SETGLOBALFOG,

	CG_R_RENDERSCENE,
	CG_R_SAVEVIEWPARMS,
	CG_R_RESTOREVIEWPARMS,
	CG_R_SETCOLOR,
	CG_R_DRAWSTRETCHPIC,
	CG_R_DRAWSTRETCHPIC_GRADIENT,	//----(SA)	added
	CG_R_MODELBOUNDS,
	CG_R_LERPTAG,
	CG_GETGLCONFIG,
	CG_GETGAMESTATE,
	CG_GETCURRENTSNAPSHOTNUMBER,
	CG_GETSNAPSHOT,
	CG_GETSERVERCOMMAND,
	CG_GETCURRENTCMDNUMBER,
	CG_GETUSERCMD,
	CG_SETUSERCMDVALUE,
	CG_SETCLIENTLERPORIGIN,			// DHM - Nerve
	CG_R_REGISTERSHADERNOMIP,
	CG_MEMORY_REMAINING,

	CG_KEY_ISDOWN,
	CG_KEY_GETCATCHER,
	CG_KEY_SETCATCHER,
	CG_KEY_GETKEY,
	CG_KEY_GETOVERSTRIKEMODE,
	CG_KEY_SETOVERSTRIKEMODE,

 	CG_PC_ADD_GLOBAL_DEFINE,
	CG_PC_LOAD_SOURCE,
	CG_PC_FREE_SOURCE,
	CG_PC_READ_TOKEN,
	CG_PC_SOURCE_FILE_AND_LINE,
	CG_PC_UNREAD_TOKEN,

	CG_S_STOPBACKGROUNDTRACK,
	CG_REAL_TIME,
	CG_SNAPVECTOR,
	CG_REMOVECOMMAND,
	CG_R_LIGHTFORPOINT,

	CG_CIN_PLAYCINEMATIC,
	CG_CIN_STOPCINEMATIC,
	CG_CIN_RUNCINEMATIC,
	CG_CIN_DRAWCINEMATIC,
	CG_CIN_SETEXTENTS,
	CG_R_REMAP_SHADER,
	CG_S_ADDREALLOOPINGSOUND,
	CG_S_STOPSTREAMINGSOUND,	//----(SA)	added

	CG_LOADCAMERA,
	CG_STARTCAMERA,
	CG_STOPCAMERA,
	CG_GETCAMERAINFO,

	CG_MEMSET = 150,
	CG_MEMCPY,
	CG_STRNCPY,
	CG_SIN,
	CG_COS,
	CG_ATAN2,
	CG_SQRT,
	CG_FLOOR,
	CG_CEIL,

	CG_TESTPRINTINT,
	CG_TESTPRINTFLOAT,
	CG_ACOS,

	CG_INGAME_POPUP,		//----(SA)	added

	// NERVE - SMF
	CG_INGAME_CLOSEPOPUP,

	CG_R_DRAWROTATEDPIC,
	CG_R_DRAW2DPOLYS,

	CG_KEY_GETBINDINGBUF,
	CG_KEY_SETBINDING,
	CG_KEY_KEYNUMTOSTRINGBUF,
	CG_KEY_BINDINGTOKEYS,

	CG_TRANSLATE_STRING,
	// -NERVE - SMF

	CG_R_INPVS,
	CG_GETHUNKDATA,

	CG_PUMPEVENTLOOP,

	// zinx
	CG_SENDMESSAGE,
	CG_MESSAGESTATUS,
	// -zinx

	// bani
	CG_R_LOADDYNAMICSHADER,
	// -bani

	// fretn
	CG_R_RENDERTOTEXTURE,
	// -fretn
	// bani
	CG_R_GETTEXTUREID,
	// -bani
	// bani
	CG_R_FINISH,
	// -bani
} cgameImport_t;


/*
==================================================================

functions exported to the main executable

==================================================================
*/

typedef enum {
	CG_INIT,
//	void CG_Init( int serverMessageNum, int serverCommandSequence )
	// called when the level loads or when the renderer is restarted
	// all media should be registered at this time
	// cgame will display loading status by calling SCR_Update, which
	// will call CG_DrawInformation during the loading process
	// reliableCommandSequence will be 0 on fresh loads, but higher for
	// demos, tourney restarts, or vid_restarts

	CG_SHUTDOWN,
//	void (*CG_Shutdown)( void );
	// oportunity to flush and close any open files

	CG_CONSOLE_COMMAND,
//	qboolean (*CG_ConsoleCommand)( void );
	// a console command has been issued locally that is not recognized by the
	// main game system.
	// use Cmd_Argc() / Cmd_Argv() to read the command, return qfalse if the
	// command is not known to the game

	CG_DRAW_ACTIVE_FRAME,
//	void (*CG_DrawActiveFrame)( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback );
	// Generates and draws a game scene and status information at the given time.
	// If demoPlayback is set, local movement prediction will not be enabled

	CG_CROSSHAIR_PLAYER,
//	int (*CG_CrosshairPlayer)( void );

	CG_LAST_ATTACKER,
//	int (*CG_LastAttacker)( void );

	CG_KEY_EVENT, 
//	void	(*CG_KeyEvent)( int key, qboolean down );

	CG_MOUSE_EVENT,
//	void	(*CG_MouseEvent)( int dx, int dy );
	CG_EVENT_HANDLING,
//	void (*CG_EventHandling)(int type, qboolean fForced);

	CG_GET_TAG,
//	qboolean CG_GetTag( int clientNum, char *tagname, orientation_t *or );

	CG_CHECKEXECKEY,

	CG_WANTSBINDKEYS,

	// zinx
	CG_MESSAGERECEIVED,
//	void (*CG_MessageReceived)( const char *buf, int buflen, int serverTime );
	// -zinx

} cgameExport_t;

//----------------------------------------------
