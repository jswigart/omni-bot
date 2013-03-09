#include "g_local.h"

round_awards_t roundAwards;

// extend warmup based on number of awards
void ExtendWarmup( qboolean adjust ) {
	int extend = 0;

	if ( !g_awards.integer ) {
		return;
	}

	if ( roundAwards.highAccValue > 0 ) {
		roundAwards.highAccAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highAmmoValue > 0 ) {
		roundAwards.highAmmoAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highDeathValue > 0 ) {
		roundAwards.highDeathAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highGibsValue > 0 ) {
		roundAwards.highGibsAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highGoombaValue > 0 ) {
		roundAwards.highGoombaAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highPoisonValue > 0 ) {
		roundAwards.highPoisonAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highKnifeValue > 0 ) {
		roundAwards.highKnifeAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highHSValue > 0 ) {
		roundAwards.highHSAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highKillValue > 0 ) {
		roundAwards.highKillAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highMedValue > 0 ) {
		roundAwards.highMedAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highRevValue > 0 ) {
		roundAwards.highRevAnnounce = qtrue;
		extend++;
	}
	if ( roundAwards.highKillSpreeValue > 0 ) {
		roundAwards.highKillSpreeAnnounce = qtrue;
		extend++;
	}
	/*if ( roundAwards.highDeathSpreeValue < 0 ) {
	roundAwards.highDeathSpreeAnnounce = qtrue;
	extend++;
	}*/

	// set the number of awards that will be printed
	if ( extend > 0 ) {
		roundAwards.NumAwards = extend;
		roundAwards.FirstAnnounce = qtrue;
		roundAwards.DelayPrint = qtrue;
		roundAwards.Complete = qfalse;
	}

	if ( adjust ) {
		int newWarmup = 21 + ( 3 * extend );
		trap_Cvar_Set( "g_warmup", va( "%i",newWarmup ) );
	}
}

// set the award winners for the round
void SetAwards( gclient_t *cl ) {
	float acc = 0.0f;

	if ( !g_awards.integer ) {
		return;
	}

	if ( cl->pers.acc_shots > 0 ) {
		acc = ( (float)cl->pers.acc_hits / (float)cl->pers.acc_shots ) * 100.0f;
	}

	if ( acc > roundAwards.highAccValue ) {
		roundAwards.highAccValue = acc;
		Q_strncpyz( roundAwards.highAccName, cl->pers.netname, sizeof( roundAwards.highAccName ) );
	}

	if ( cl->pers.ammoPacks > roundAwards.highAmmoValue ) {
		roundAwards.highAmmoValue = cl->pers.ammoPacks;
		Q_strncpyz( roundAwards.highAmmoName, cl->pers.netname, sizeof( roundAwards.highAmmoName ) );
	}

	if ( cl->pers.deaths > roundAwards.highDeathValue ) {
		roundAwards.highDeathValue = cl->pers.deaths;
		Q_strncpyz( roundAwards.highDeathName, cl->pers.netname, sizeof( roundAwards.highDeathName ) );
	}

	if ( cl->pers.gibs > roundAwards.highGibsValue ) {
		roundAwards.highGibsValue = cl->pers.gibs;
		Q_strncpyz( roundAwards.highGibsName, cl->pers.netname, sizeof( roundAwards.highGibsName ) );
	}

	if ( cl->pers.goomba > roundAwards.highGoombaValue ) {
		roundAwards.highGoombaValue = cl->pers.goomba;
		Q_strncpyz( roundAwards.highGoombaName, cl->pers.netname, sizeof( roundAwards.highGoombaName ) );
	}

	if ( cl->pers.poison > roundAwards.highPoisonValue ) {
		roundAwards.highPoisonValue = cl->pers.poison;
		Q_strncpyz( roundAwards.highPoisonName, cl->pers.netname, sizeof( roundAwards.highPoisonName ) );
	}

	if ( cl->pers.knifeKill > roundAwards.highKnifeValue ) {
		roundAwards.highKnifeValue = cl->pers.knifeKill;
		Q_strncpyz( roundAwards.highKnifeName, cl->pers.netname, sizeof( roundAwards.highKnifeName ) );
	}

	if ( cl->pers.headshots > roundAwards.highHSValue ) {
		roundAwards.highHSValue = cl->pers.headshots;
		Q_strncpyz( roundAwards.highHSName, cl->pers.netname, sizeof( roundAwards.highHSName ) );
	}

	if ( cl->pers.kills > roundAwards.highKillValue ) {
		roundAwards.highKillValue = cl->pers.kills;
		Q_strncpyz( roundAwards.highKillName, cl->pers.netname, sizeof( roundAwards.highKillName ) );
	}

	if ( cl->pers.medPacks > roundAwards.highMedValue ) {
		roundAwards.highMedValue = cl->pers.medPacks;
		Q_strncpyz( roundAwards.highMedName, cl->pers.netname, sizeof( roundAwards.highMedName ) );
	}

	if ( cl->pers.revives > roundAwards.highRevValue ) {
		roundAwards.highRevValue = cl->pers.revives;
		Q_strncpyz( roundAwards.highRevName, cl->pers.netname, sizeof( roundAwards.highRevName ) );
	}

	if ( cl->pers.killSpreeMax > roundAwards.highKillSpreeValue ) {
		roundAwards.highKillSpreeValue = cl->pers.killSpreeMax;
		Q_strncpyz( roundAwards.highKillSpreeName, cl->pers.netname, sizeof( roundAwards.highKillSpreeName ) );
	}

	if ( cl->pers.deathSpreeMax < roundAwards.highDeathSpreeValue ) {
		roundAwards.highDeathSpreeValue = cl->pers.deathSpreeMax;
		Q_strncpyz( roundAwards.highDeathSpreeName, cl->pers.netname, sizeof( roundAwards.highDeathSpreeName ) );
	}
}

void ClearAwards() {
	roundAwards.FirstAnnounce = qtrue;
	roundAwards.Complete = qtrue;
	roundAwards.NumAwards = 0;
	roundAwards.highAccValue = 0;
	roundAwards.highAmmoValue = 0;
	roundAwards.highDeathValue = 0;
	roundAwards.highGibsValue = 0;
	roundAwards.highGoombaValue = 0;
	roundAwards.highPoisonValue = 0;
	roundAwards.highKnifeValue = 0;
	roundAwards.highHSValue = 0;
	roundAwards.highKillValue = 0;
	roundAwards.highMedValue = 0;
	roundAwards.highRevValue = 0;
	roundAwards.highKillSpreeValue = 0;
	roundAwards.highDeathSpreeValue = 0;
}

void FormatName( char *in ) {
	char newname[MAX_STRING_CHARS];
	CleanseString( in, newname, qfalse );
	Q_strncpyz( in, newname, 36 );
}

// list the awards in console at the end so slow connectors can see them too ...
void ListAwards() {
	if ( roundAwards.NumAwards > 0 ) {
		trap_SendServerCommand( -1, "print \"\n^3AWARD      Amount   Player\n"
			"^7---------------------------------------------------------------------\n\"" );
	}

	if ( roundAwards.highKillValue > 0 ) {
		FormatName( roundAwards.highKillName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Kills:^7"
			,roundAwards.highKillValue
			,roundAwards.highKillName ) );
	}

	if ( roundAwards.highDeathValue > 0 ) {
		FormatName( roundAwards.highDeathName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Deaths:^7"
			,roundAwards.highDeathValue
			,roundAwards.highDeathName ) );
	}

	if ( roundAwards.highGibsValue > 0 ) {
		FormatName( roundAwards.highGibsName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Gibs:^7"
			,roundAwards.highGibsValue
			,roundAwards.highGibsName ) );
	}

	if ( roundAwards.highAccValue > 0 ) {
		FormatName( roundAwards.highAccName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6s %s\n\""
			,"^3Accuracy:^7"
			,va( "%2.2f",roundAwards.highAccValue )
			,roundAwards.highAccName ) );
	}

	if ( roundAwards.highRevValue > 0 ) {
		FormatName( roundAwards.highRevName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Revives:^7"
			,roundAwards.highRevValue
			,roundAwards.highRevName ) );
	}

	if ( roundAwards.highMedValue > 0 ) {
		FormatName( roundAwards.highMedName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Med Packs:^7"
			,roundAwards.highMedValue
			,roundAwards.highMedName ) );
	}

	if ( roundAwards.highAmmoValue > 0 ) {
		FormatName( roundAwards.highAmmoName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Ammo Packs:^7"
			,roundAwards.highAmmoValue
			,roundAwards.highAmmoName ) );
	}

	if ( roundAwards.highHSValue > 0 ) {
		FormatName( roundAwards.highHSName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Headshots:^7"
			,roundAwards.highHSValue
			,roundAwards.highHSName ) );
	}

	if ( roundAwards.highGoombaValue > 0 ) {
		FormatName( roundAwards.highGoombaName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Goombas:^7"
			,roundAwards.highGoombaValue
			,roundAwards.highGoombaName ) );
	}

	if ( roundAwards.highPoisonValue > 0 ) {
		FormatName( roundAwards.highPoisonName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Poisons:^7"
			,roundAwards.highPoisonValue
			,roundAwards.highPoisonName ) );
	}

	if ( roundAwards.highKnifeValue > 0 ) {
		FormatName( roundAwards.highKnifeName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3Knife Kills:^7"
			,roundAwards.highKnifeValue
			,roundAwards.highKnifeName ) );
	}

	if ( roundAwards.highKillSpreeValue > 0 ) {
		FormatName( roundAwards.highKillSpreeName );
		trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
			,"^3KillSpree:^7"
			,roundAwards.highKillSpreeValue
			,roundAwards.highKillSpreeName ) );
	}

	/*if ( roundAwards.highDeathSpreeValue < 0 ) {
	FormatName( roundAwards.highDeathSpreeName );
	trap_SendServerCommand( -1, va( "print \"%-16s %-6d %s\n\""
	,"^3DeathSpree:^7"
	,roundAwards.highDeathSpreeValue
	,roundAwards.highDeathSpreeName ) );
	}*/
	ClearAwards();
}

void PrintAwards() {
	char *acc;

	if ( !g_awards.integer ) {
		return;
	}

	roundAwards.PrintTime = level.time + 3000;

	if ( roundAwards.Complete ) {
		return;
	}

	if ( roundAwards.DelayPrint ) {
		roundAwards.PrintTime = level.time + 9000;
		roundAwards.DelayPrint = qfalse;
		return;
	}

	if ( roundAwards.FirstAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3High Achievers Last Round:\" 3" ) );
		roundAwards.FirstAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highKillAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Kills: ^7%i\n%s\" 3",roundAwards.highKillValue,roundAwards.highKillName ) );
		roundAwards.highKillAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highDeathAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Deaths: ^7%i\n%s\" 3",roundAwards.highDeathValue,roundAwards.highDeathName ) );
		roundAwards.highDeathAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highGibsAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Gibs: ^7%i\n%s\" 3",roundAwards.highGibsValue,roundAwards.highGibsName ) );
		roundAwards.highGibsAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highAccAnnounce ) {
		acc = va( "%2.2f",roundAwards.highAccValue );
		trap_SendServerCommand( -1, va( "cp \"^3Highest Accuracy: ^7%s\n%s\" 3",acc,roundAwards.highAccName ) );
		roundAwards.highAccAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highHSAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Headshots: ^7%i\n%s\" 3",roundAwards.highHSValue,roundAwards.highHSName ) );
		roundAwards.highHSAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highRevAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Revives: ^7%i\n%s\" 3",roundAwards.highRevValue,roundAwards.highRevName ) );
		roundAwards.highRevAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highMedAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Med Given: ^7%i\n%s\" 3",roundAwards.highMedValue,roundAwards.highMedName ) );
		roundAwards.highMedAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highAmmoAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Ammo Given: ^7%i\n%s\" 3",roundAwards.highAmmoValue,roundAwards.highAmmoName ) );
		roundAwards.highAmmoAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highGoombaAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Goombas: ^7%i\n%s\" 3",roundAwards.highGoombaValue,roundAwards.highGoombaName ) );
		roundAwards.highGoombaAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highPoisonAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Poisons: ^7%i\n%s\" 3",roundAwards.highPoisonValue,roundAwards.highPoisonName ) );
		roundAwards.highPoisonAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highKnifeAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Most Knife Kills: ^7%i\n%s\" 3",roundAwards.highKnifeValue,roundAwards.highKnifeName ) );
		roundAwards.highKnifeAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highKillSpreeAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Kill Spree: ^7%i\n%s\" 3",roundAwards.highKillSpreeValue,roundAwards.highKillSpreeName ) );
		roundAwards.highKillSpreeAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highDeathSpreeAnnounce ) {
		trap_SendServerCommand( -1, va( "cp \"^3Death Spree: ^7%i\n%s\" 3",roundAwards.highDeathSpreeValue,roundAwards.highDeathSpreeName ) );
		roundAwards.highDeathSpreeAnnounce = qfalse;
		return;
	}

	ListAwards();
}

void WriteAwards() {
	FILE    *awards;
	char buf[2048];

	if ( !g_awards.integer ) {
		return;
	}

	buf[0] = 0;

	awards = fopen( "awards.txt","w+" );

	if ( !awards ) {
		G_Printf( "^1ERROR: Unable to open awards file!\n" );
		return;
	}

	if ( roundAwards.highKillValue > 0 ) {
		strcat( buf, va( "KillValue %i \n", roundAwards.highKillValue ) );
		strcat( buf, va( "KillName %s \n", roundAwards.highKillName ) );
	}

	if ( roundAwards.highDeathValue > 0 ) {
		strcat( buf, va( "DeathValue %i \n", roundAwards.highDeathValue ) );
		strcat( buf, va( "DeathName %s \n", roundAwards.highDeathName ) );
	}

	if ( roundAwards.highGibsValue > 0 ) {
		strcat( buf, va( "GibsValue %i \n", roundAwards.highGibsValue ) );
		strcat( buf, va( "GibsName %s \n", roundAwards.highGibsName ) );
	}

	if ( roundAwards.highAccValue > 0 ) {
		strcat( buf, va( "AccValue %2.2f \n", roundAwards.highAccValue ) );
		strcat( buf, va( "AccName %s \n", roundAwards.highAccName ) );
	}

	if ( roundAwards.highRevValue > 0 ) {
		strcat( buf, va( "RevValue %i \n", roundAwards.highRevValue ) );
		strcat( buf, va( "RevName %s \n", roundAwards.highRevName ) );
	}

	if ( roundAwards.highMedValue > 0 ) {
		strcat( buf, va( "MedValue %i \n", roundAwards.highMedValue ) );
		strcat( buf, va( "MedName %s \n", roundAwards.highMedName ) );
	}

	if ( roundAwards.highAmmoValue > 0 ) {
		strcat( buf, va( "AmmoValue %i \n", roundAwards.highAmmoValue ) );
		strcat( buf, va( "AmmoName %s \n", roundAwards.highAmmoName ) );
	}

	if ( roundAwards.highHSValue > 0 ) {
		strcat( buf, va( "HSValue %i \n", roundAwards.highHSValue ) );
		strcat( buf, va( "HSName %s \n", roundAwards.highHSName ) );
	}

	if ( roundAwards.highGoombaValue > 0 ) {
		strcat( buf, va( "GoombaValue %i \n", roundAwards.highGoombaValue ) );
		strcat( buf, va( "GoombaName %s \n", roundAwards.highGoombaName ) );
	}

	if ( roundAwards.highPoisonValue > 0 ) {
		strcat( buf, va( "PoisonValue %i \n", roundAwards.highPoisonValue ) );
		strcat( buf, va( "PoisonName %s \n", roundAwards.highPoisonName ) );
	}

	if ( roundAwards.highKnifeValue > 0 ) {
		strcat( buf, va( "KnifeValue %i \n", roundAwards.highKnifeValue ) );
		strcat( buf, va( "KnifeName %s \n", roundAwards.highKnifeName ) );
	}

	if ( roundAwards.highKillSpreeValue > 0 ) {
		strcat( buf, va( "KillSpreeValue %i \n", roundAwards.highKillSpreeValue ) );
		strcat( buf, va( "KillSpreeName %s \n", roundAwards.highKillSpreeName ) );
	}

	if ( roundAwards.highDeathSpreeValue < 0 ) {
		strcat( buf, va( "DeathSpreeValue %i \n", roundAwards.highDeathSpreeValue ) );
		strcat( buf, va( "DeathSpreeName %s \n", roundAwards.highDeathSpreeName ) );
	}

	fputs( buf,awards );
	fclose( awards );
}

// from sandmod
void ParseStr( const char *strInput, char *strCmd, char *strArgs ) {
	int i = 0, j = 0;
	int foundspace = 0;

	while ( strInput[i] != 0 ) {
		if ( !foundspace ) {
			if ( strInput[i] == ' ' ) {
				foundspace = 1;
				strCmd[i] = 0;
			} else {
				strCmd[i] = strInput[i];
			}
			i++;
		} else { // We have found a space
			strArgs[j++] = strInput[i++];
		}
	}
	if ( !foundspace ) {
		strCmd[i] = 0;
	}

	strArgs[j] = 0;
}

void ReadAwards() {
	FILE* file;
	char info[1024];
	char arg[1024];
	char typechar[1024];
	long hash;

	if ( !g_awards.integer ) {
		return;
	}

	file = fopen( "awards.txt","a+" );

	if ( !file ) {
		G_Printf( "^1ERROR: Unable to open awards file!\n" );
		return;
	}

	while ( fgets( info,1024,file ) != NULL )
	{
		ParseStr( info,typechar,arg );

		hash = BG_StringHashValue( typechar );
		switch ( hash )
		{
		case 119188: /*KillValue*/
			roundAwards.highKillValue = atoi( arg );
			break;
		case 103486: /*KillName*/
			Q_strncpyz( roundAwards.highKillName, arg, sizeof( roundAwards.highKillName ) );
			break;
		case 130853: /*DeathValue*/
			roundAwards.highDeathValue = atoi( arg );
			break;
		case 115027: /*DeathName*/
			Q_strncpyz( roundAwards.highDeathName, arg, sizeof( roundAwards.highDeathName ) );
			break;
		case 118356: /*GibsValue*/
			roundAwards.highGibsValue = atoi( arg );
			break;
		case 102654: /*GibsName*/
			Q_strncpyz( roundAwards.highGibsName, arg, sizeof( roundAwards.highGibsName ) );
			break;
		case 102472: /*AccValue*/
			roundAwards.highAccValue = atof( arg );
			break;
		case 86894: /*AccName*/
			Q_strncpyz( roundAwards.highAccName, arg, sizeof( roundAwards.highAccName ) );
			break;
		case 107034: /*RevValue*/
			roundAwards.highRevValue = atoi( arg );
			break;
		case 91456: /*RevName*/
			Q_strncpyz( roundAwards.highRevName, arg, sizeof( roundAwards.highRevName ) );
			break;
		case 104261: /*MedValue*/
			roundAwards.highMedValue = atoi( arg );
			break;
		case 88683: /*MedName*/
			Q_strncpyz( roundAwards.highMedName, arg, sizeof( roundAwards.highMedName ) );
			break;
		case 118965: /*AmmoValue*/
			roundAwards.highAmmoValue = atoi( arg );
			break;
		case 103263: /*AmmoName*/
			Q_strncpyz( roundAwards.highAmmoName, arg, sizeof( roundAwards.highAmmoName ) );
			break;
		case 92705: /*HSValue*/
			roundAwards.highHSValue = atoi( arg );
			break;
		case 77251: /*HSName*/
			Q_strncpyz( roundAwards.highHSName, arg, sizeof( roundAwards.highHSName ) );
			break;
		case 145081: /*GoombaValue*/
			roundAwards.highGoombaValue = atoi( arg );
			break;
		case 129131: /*GoombaName*/
			Q_strncpyz( roundAwards.highGoombaName, arg, sizeof( roundAwards.highGoombaName ) );
			break;
		case 149369: /*PoisonValue*/
			roundAwards.highPoisonValue = atoi( arg );
			break;
		case 133419: /*PoisonName*/
			Q_strncpyz( roundAwards.highPoisonName, arg, sizeof( roundAwards.highPoisonName ) );
			break;
		case 131657: /*KnifeValue*/
			roundAwards.highKnifeValue = atoi( arg );
			break;
		case 115831: /*KnifeName*/
			Q_strncpyz( roundAwards.highKnifeName, arg, sizeof( roundAwards.highKnifeName ) );
			break;
		case 201937: /*DeathSpreeValue*/
			roundAwards.highDeathSpreeValue = atoi( arg );
			break;
		case 185491: /*DeathSpreeName*/
			Q_strncpyz( roundAwards.highDeathSpreeName, arg, sizeof( roundAwards.highDeathSpreeName ) );
			break;
		case 189729: /*KillSpreeValue*/
			roundAwards.highKillSpreeValue = atoi( arg );
			break;
		case 173407: /*KillSpreeName*/
			Q_strncpyz( roundAwards.highKillSpreeName, arg, sizeof( roundAwards.highKillSpreeName ) );
			break;
		}
	}

	fclose( file );
	remove( "awards.txt" );
	ExtendWarmup( qfalse );
}