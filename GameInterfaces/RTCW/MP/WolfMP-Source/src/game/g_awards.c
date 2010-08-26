#include "g_local.h"

round_awards_t	roundAwards;

// extend warmup based on number of awards
void ExtendWarmup(qboolean adjust)
{
	int extend = 0;
	int multiplier = 3;
	int newWarmup = 21;

	if (!g_awards.integer)
		return;

	if (roundAwards.highAccValue > 0)
	{
		roundAwards.highAccAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highAmmoValue > 0)
	{
		roundAwards.highAmmoAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highDeathValue > 0)
	{
		roundAwards.highDeathAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highGibsValue > 0)
	{
		roundAwards.highGibsAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highGoombaValue > 0)
	{
		roundAwards.highGoombaAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highHSValue > 0)
	{
		roundAwards.highHSAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highKillValue > 0)
	{
		roundAwards.highKillAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highMedValue > 0)
	{
		roundAwards.highMedAnnounce = qtrue;
		extend++;
	}
	if (roundAwards.highRevValue > 0)
	{
		roundAwards.highRevAnnounce = qtrue;
		extend++;
	}

	// set the number of awards that will be printed
	if ( extend > 0 )
	{
		roundAwards.NumAwards = extend;
		roundAwards.FirstAnnounce = qtrue;
		roundAwards.DelayPrint = qtrue;
		roundAwards.Complete = qfalse;
	}

	if ( adjust )
	{
		newWarmup += multiplier * extend;
		trap_Cvar_Set( "g_warmup", va("%i",newWarmup) );
	}
}

// set the award winners for the round
void SetAwards(gclient_t *cl)
{
	float acc = 0.0f;

	if (!g_awards.integer)
		return;

	if ( cl->pers.acc_shots > 0 )
	{
		acc = ((float)cl->pers.acc_hits / (float)cl->pers.acc_shots) * 100.0f;
	}

	if (acc > roundAwards.highAccValue)
	{
		roundAwards.highAccValue = acc;
		Q_strncpyz( roundAwards.highAccName, cl->pers.netname, sizeof(roundAwards.highAccName) );
	}

	if (cl->pers.ammoPacks > roundAwards.highAmmoValue)
	{
		roundAwards.highAmmoValue = cl->pers.ammoPacks;
		Q_strncpyz( roundAwards.highAmmoName, cl->pers.netname, sizeof(roundAwards.highAmmoName) );
	}

	if (cl->ps.persistant[PERS_KILLED] > roundAwards.highDeathValue)
	{
		roundAwards.highDeathValue = cl->ps.persistant[PERS_KILLED];
		Q_strncpyz( roundAwards.highDeathName, cl->pers.netname, sizeof(roundAwards.highDeathName) );
	}

	if (cl->pers.gibs > roundAwards.highGibsValue)
	{
		roundAwards.highGibsValue = cl->pers.gibs;
		Q_strncpyz( roundAwards.highGibsName, cl->pers.netname, sizeof(roundAwards.highGibsName) );
	}

	if (cl->pers.goomba > roundAwards.highGoombaValue)
	{
		roundAwards.highGoombaValue = cl->pers.goomba;
		Q_strncpyz( roundAwards.highGoombaName, cl->pers.netname, sizeof(roundAwards.highGoombaName) );
	}

	if (cl->pers.headshots > roundAwards.highHSValue)
	{
		roundAwards.highHSValue = cl->pers.headshots;
		Q_strncpyz( roundAwards.highHSName, cl->pers.netname, sizeof(roundAwards.highHSName) );
	}

	if (cl->pers.kills > roundAwards.highKillValue)
	{
		roundAwards.highKillValue = cl->pers.kills;
		Q_strncpyz( roundAwards.highKillName, cl->pers.netname, sizeof(roundAwards.highKillName) );
	}

	if (cl->pers.medPacks > roundAwards.highMedValue)
	{
		roundAwards.highMedValue = cl->pers.medPacks;
		Q_strncpyz( roundAwards.highMedName, cl->pers.netname, sizeof(roundAwards.highMedName) );
	}

	if (cl->pers.revives > roundAwards.highRevValue)
	{
		roundAwards.highRevValue = cl->pers.revives;
		Q_strncpyz( roundAwards.highRevName, cl->pers.netname, sizeof(roundAwards.highRevName) );
	}
}

void ClearAwards()
{
	roundAwards.FirstAnnounce = qtrue;
	roundAwards.Complete = qtrue;
	roundAwards.NumAwards = 0;
	roundAwards.highAccValue = 0;
	roundAwards.highAmmoValue = 0;
	roundAwards.highDeathValue = 0;
	roundAwards.highGibsValue = 0;
	roundAwards.highGoombaValue = 0;
	roundAwards.highHSValue = 0;
	roundAwards.highKillValue = 0;
	roundAwards.highMedValue = 0;
	roundAwards.highRevValue = 0;
}

void FormatName(char *in)
{
	char newname[MAX_STRING_CHARS];
	CleanseString(in, newname, qfalse);
	Q_strncpyz( in, newname, 36 );
}

// list the awards in console at the end so slow connectors can see them too ...
void ListAwards()
{
	if(roundAwards.NumAwards > 0)
	{
		trap_SendServerCommand(-1, "print \"\n^3AWARD      Amount   Player\n"
			"^7---------------------------------------------------------------------\n\"");
	}

	if(roundAwards.highKillValue > 0)
	{
		FormatName(roundAwards.highKillName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Kills:^7"
										,roundAwards.highKillValue
										,roundAwards.highKillName));
	}

	if(roundAwards.highDeathValue > 0)
	{
		FormatName(roundAwards.highDeathName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Deaths:^7"
										,roundAwards.highDeathValue
										,roundAwards.highDeathName));
	}

	if(roundAwards.highGibsValue > 0)
	{
		FormatName(roundAwards.highGibsName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Gibs:^7"
										,roundAwards.highGibsValue
										,roundAwards.highGibsName));
	}

	if(roundAwards.highAccValue > 0)
	{
		FormatName(roundAwards.highAccName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6s %s\n\""
										,"^3Accuracy:^7"
										,va("%2.2f",roundAwards.highAccValue)
										,roundAwards.highAccName));
	}

	if(roundAwards.highRevValue > 0)
	{
		FormatName(roundAwards.highRevName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Revives:^7"
										,roundAwards.highRevValue
										,roundAwards.highRevName));
	}

	if(roundAwards.highMedValue > 0)
	{
		FormatName(roundAwards.highMedName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Med Packs:^7"
										,roundAwards.highMedValue
										,roundAwards.highMedName));
	}

	if(roundAwards.highAmmoValue > 0)
	{
		FormatName(roundAwards.highAmmoName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Ammo Packs:^7"
										,roundAwards.highAmmoValue
										,roundAwards.highAmmoName));
	}

	if(roundAwards.highHSValue > 0)
	{
		FormatName(roundAwards.highHSName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Headshots:^7"
										,roundAwards.highHSValue
										,roundAwards.highHSName));
	}

	if(roundAwards.highGoombaValue > 0)
	{
		FormatName(roundAwards.highGoombaName);
		trap_SendServerCommand(-1, va("print \"%-16s %-6d %s\n\""
										,"^3Goombas:^7"
										,roundAwards.highGoombaValue
										,roundAwards.highGoombaName));
	}

	ClearAwards();
}

void PrintAwards()
{
	char *acc;

	if (!g_awards.integer)
		return;
	
	roundAwards.PrintTime = level.time + 3000;

	if ( roundAwards.Complete )
	{
		return;
	}

	if ( roundAwards.DelayPrint )
	{
		roundAwards.PrintTime = level.time + 9000;
		roundAwards.DelayPrint = qfalse;
		return;
	}

	if ( roundAwards.FirstAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3High Achievers Last Round:"));
		roundAwards.FirstAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highKillAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Kills: ^7%i\n%s",roundAwards.highKillValue,roundAwards.highKillName));
		roundAwards.highKillAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highDeathAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Deaths: ^7%i\n%s",roundAwards.highDeathValue,roundAwards.highDeathName));
		roundAwards.highDeathAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highGibsAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Gibs: ^7%i\n%s",roundAwards.highGibsValue,roundAwards.highGibsName));
		roundAwards.highGibsAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highAccAnnounce )
	{
		acc = va("%2.2f",roundAwards.highAccValue);
		trap_SendServerCommand(-1, va("cp \"^3Highest Accuracy: ^7%s\n%s",acc,roundAwards.highAccName));
		roundAwards.highAccAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highHSAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Headshots: ^7%i\n%s",roundAwards.highHSValue,roundAwards.highHSName));
		roundAwards.highHSAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highRevAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Revives: ^7%i\n%s",roundAwards.highRevValue,roundAwards.highRevName));
		roundAwards.highRevAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highMedAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Med Given: ^7%i\n%s",roundAwards.highMedValue,roundAwards.highMedName));
		roundAwards.highMedAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highAmmoAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Ammo Given: ^7%i\n%s",roundAwards.highAmmoValue,roundAwards.highAmmoName));
		roundAwards.highAmmoAnnounce = qfalse;
		return;
	}

	if ( roundAwards.highGoombaAnnounce )
	{
		trap_SendServerCommand(-1, va("cp \"^3Most Goombas: ^7%i\n%s",roundAwards.highGoombaValue,roundAwards.highGoombaName));
		roundAwards.highGoombaAnnounce = qfalse;
		return;
	}

	ListAwards();
}

void WriteAwards()
{
	FILE	*awards;
	char	buf[2048];

	if (!g_awards.integer)
		return;

	buf[0] = 0;

	awards = fopen("awards.txt","w+");

	if (!awards)
	{
		G_Printf("^1ERROR: Unable to open awards file!\n");
		return;
	}

	if ( roundAwards.highKillValue > 0 )
	{
		strcat(buf, va("KillValue %i \n", roundAwards.highKillValue));
		strcat(buf, va("KillName %s \n", roundAwards.highKillName));
	}

	if ( roundAwards.highDeathValue > 0 )
	{
		strcat(buf, va("DeathValue %i \n", roundAwards.highDeathValue));
		strcat(buf, va("DeathName %s \n", roundAwards.highDeathName));
	}

	if ( roundAwards.highGibsValue > 0 )
	{
		strcat(buf, va("GibsValue %i \n", roundAwards.highGibsValue));
		strcat(buf, va("GibsName %s \n", roundAwards.highGibsName));
	}

	if ( roundAwards.highAccValue > 0 )
	{
		strcat(buf, va("AccValue %2.2f \n", roundAwards.highAccValue));
		strcat(buf, va("AccName %s \n", roundAwards.highAccName));
	}

	if ( roundAwards.highRevValue > 0 )
	{
		strcat(buf, va("RevValue %i \n", roundAwards.highRevValue));
		strcat(buf, va("RevName %s \n", roundAwards.highRevName));
	}

	if ( roundAwards.highMedValue > 0 )
	{
		strcat(buf, va("MedValue %i \n", roundAwards.highMedValue));
		strcat(buf, va("MedName %s \n", roundAwards.highMedName));
	}

	if ( roundAwards.highAmmoValue > 0 )
	{
		strcat(buf, va("AmmoValue %i \n", roundAwards.highAmmoValue));
		strcat(buf, va("AmmoName %s \n", roundAwards.highAmmoName));
	}

	if ( roundAwards.highHSValue > 0 )
	{
		strcat(buf, va("HSValue %i \n", roundAwards.highHSValue));
		strcat(buf, va("HSName %s \n", roundAwards.highHSName));
	}

	if ( roundAwards.highGoombaValue > 0 )
	{
		strcat(buf, va("GoombaValue %i \n", roundAwards.highGoombaValue));
		strcat(buf, va("GoombaName %s \n", roundAwards.highGoombaName));
	}

	fputs(buf,awards);
	fclose(awards);
}

// from sandmod
void ParseStr(const char *strInput, char *strCmd, char *strArgs)
{
	int i = 0, j=0;
	int foundspace=0;

	while(strInput[i] != 0){
		if(!foundspace){
			if(strInput[i] == ' '){
				foundspace = 1;
				strCmd[i]=0;
			}else
				strCmd[i]=strInput[i];
			i++;
		}else{ // We have found a space
			strArgs[j++]=strInput[i++];
		}
	}
	if(!foundspace)
		strCmd[i]=0;

	strArgs[j]=0;
}

void ReadAwards()
{
	FILE* file;
	char info[1024];
	char arg[1024];
	char typechar[1024];

	if (!g_awards.integer)
		return;

	file = fopen("awards.txt","a+");

	if (!file)
	{
		G_Printf("^1ERROR: Unable to open awards file!\n");
		return;
	}

	while (fgets(info,1024,file)!= NULL)
	{
		ParseStr(info,typechar,arg);

		if (!Q_stricmp(typechar, "KillValue"))
			roundAwards.highKillValue = atoi(arg);
		else if (!Q_stricmp(typechar, "KillName"))
			Q_strncpyz( roundAwards.highKillName, arg, sizeof(roundAwards.highKillName) );
		else if (!Q_stricmp(typechar, "DeathValue"))
			roundAwards.highDeathValue = atoi(arg);
		else if (!Q_stricmp(typechar, "DeathName"))
			Q_strncpyz( roundAwards.highDeathName, arg, sizeof(roundAwards.highDeathName) );
		else if (!Q_stricmp(typechar, "GibsValue"))
			roundAwards.highGibsValue = atoi(arg);
		else if (!Q_stricmp(typechar, "GibsName"))
			Q_strncpyz( roundAwards.highGibsName, arg, sizeof(roundAwards.highGibsName) );
		else if (!Q_stricmp(typechar, "AccValue"))
			roundAwards.highAccValue = atof(arg);
		else if (!Q_stricmp(typechar, "AccName"))
			Q_strncpyz( roundAwards.highAccName, arg, sizeof(roundAwards.highAccName) );
		else if (!Q_stricmp(typechar, "RevValue"))
			roundAwards.highRevValue = atoi(arg);
		else if (!Q_stricmp(typechar, "RevName"))
			Q_strncpyz( roundAwards.highRevName, arg, sizeof(roundAwards.highRevName) );
		else if (!Q_stricmp(typechar, "MedValue"))
			roundAwards.highMedValue = atoi(arg);
		else if (!Q_stricmp(typechar, "MedName"))
			Q_strncpyz( roundAwards.highMedName, arg, sizeof(roundAwards.highMedName) );
		else if (!Q_stricmp(typechar, "AmmoValue"))
			roundAwards.highAmmoValue = atoi(arg);
		else if (!Q_stricmp(typechar, "AmmoName"))
			Q_strncpyz( roundAwards.highAmmoName, arg, sizeof(roundAwards.highAmmoName) );
		else if (!Q_stricmp(typechar, "HSValue"))
			roundAwards.highHSValue = atoi(arg);
		else if (!Q_stricmp(typechar, "HSName"))
			Q_strncpyz( roundAwards.highHSName, arg, sizeof(roundAwards.highHSName) );
		else if (!Q_stricmp(typechar, "GoombaValue"))
			roundAwards.highGoombaValue = atoi(arg);
		else if (!Q_stricmp(typechar, "GoombaName"))
			Q_strncpyz( roundAwards.highGoombaName, arg, sizeof(roundAwards.highGoombaName) );
	}

	fclose(file);
	remove("awards.txt");
	ExtendWarmup(qfalse);
}