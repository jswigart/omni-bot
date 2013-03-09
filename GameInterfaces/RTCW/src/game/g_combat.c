/*
* name:		g_combat.c
*
* desc:
*
*/

#include "g_local.h"
#include  "g_rtcwbot_interface.h"

#ifdef WITH_LUA
#include "g_lua.h"
#endif

static qboolean firstblood;

int BG_GetKillSpree( playerState_t *ps ) {
	if ( ps != NULL ) {
		return ps->persistant[PERS_KILLSPREE];
	}
	return 0;
}

int BG_SetKillSpree( playerState_t *ps, int value ) {
	if ( ps != NULL ) {
		ps->persistant[PERS_KILLSPREE] = value;
	}
	return 0;
}

const char *spreeMessage[] = {
	"^dis on a ^2Killing Spree^d!",
	"^dis on a ^2Rampage^d!",
	"^dis ^2Dominating^d!",
	"^drevels in his ^2Bloodbath^d!",
	"^dis a walking ^2Slaughterhouse^d!",
	"^dwreaks ^2Havoc ^dupon his foes!",
	"^dcuts through enemies like a ^2God ^2of ^2War^d!",
	"^dis the ^2Prophet of Doom^d!",
	"^dunleashes the ^>Shitstorm^d!",
	"^dis still unleashing the ^>Shitstorm^d!"
};

const char *humiliationMessage[] = {
	"^dmust be having a bad day!",
	"^dis getting his ass kicked!",
	"^dis a death magnet!",
	"^dneeds remedial combat training!",
	"^dseems well-aquainted with Death!",
	"^dstill can't kill shit!"
};

const char *spreeSound[] = {
	"sound/spree/killingspree.wav",
	"sound/spree/rampage.wav",
	"sound/spree/dominating.wav",
	"sound/spree/unstoppable.wav",
	"sound/spree/godlike.wav",
	"sound/spree/wickedsick.wav",
	"sound/spree/potter.wav",
	"sound/spree/holyshit.wav",
	"sound/spree/holyshit.wav",
	"sound/spree/holyshit.wav"
};

static void G_UpdateKillingSpree( gentity_t *ent, gentity_t *att, qboolean death ) {
	int spree = 0, old_spree = 0, snd_idx = 0;
	qboolean skipBotEnt = (ent && (ent->r.svFlags & SVF_BOT) && (g_OmniBotFlags.integer & OBF_NO_SPREE_ANNOUNCE));

	if ( &ent->client->ps ) {
		old_spree = ent->client->ps.persistant[PERS_KILLSPREE];
	}

	// Get new spree
	// Increment spree if the player makes a kill, if spree is still less than one, set it to one.
	// Set spree to zero if the player dies, unless it's already zero or less, in which case decrement it
	spree = death ? ( old_spree > 0 ? 0 : old_spree - 1 ) : ( ( old_spree < 0 ) ? 1 : old_spree + 1 );

	// set max kill / death sprees
	if ( spree < ent->client->pers.deathSpreeMax ) {
		ent->client->pers.deathSpreeMax = spree;
	} else if ( spree > ent->client->pers.killSpreeMax ) {
		ent->client->pers.killSpreeMax = spree;
	}

	// end of killing spree
	if ( old_spree >= 5 && death ) {
		if ( g_announcer.integer & ANNOUNCE_KILLINGSPREES && !skipBotEnt ) {
			if ( att == NULL || !att->client || ent == att ) {
				AP( va( "chat \"%s^d's killing spree ended after ^3%d^d kills! ^d(^1SUICIDE!^d)\"",
					ent->client->pers.netname, old_spree ) );
			} else {
				AP( va( "chat \"%s^d's killing spree was brought to an end by ^*%s%s ^dafter ^3%d ^dkills!\"",
					ent->client->pers.netname, OnSameTeam( ent, att ) ? "^1TEAMMATE^7 " : "",
					att->client->pers.netname, old_spree ) );
			}
		}
	}

	// end of death spree
	if ( (g_announcer.integer & ANNOUNCE_DEATHSPREES) && !skipBotEnt && old_spree <= -10 && !death ) {
		AP( va( "chat \"%s^d's death spree has ended after ^3%d ^ddeaths!\"",
			ent->client->pers.netname, -old_spree ) );
	}

	// new killing spree (killing spree every 5 kills, % 5)
	if ( (g_announcer.integer & ANNOUNCE_KILLINGSPREES) && !skipBotEnt && spree >= 5 && (spree % 5) == 0 ) {
		gentity_t *te;
		snd_idx = ( spree / 5 ) - 1;
		snd_idx = ( snd_idx <= 9 ) ? snd_idx : 9;
		AP( va( "chat \"^1KILLING SPREE! ^*%s ^*%s ^d(^3%d ^dkills in a row!)\"",
			ent->client->pers.netname, spreeMessage[snd_idx], spree ) );
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( spreeSound[snd_idx] );
		te->r.svFlags |= SVF_BROADCAST;
	}
	// new death spree (death spree every 10 deaths, % 10)
	else if ( (g_announcer.integer & ANNOUNCE_DEATHSPREES) && !skipBotEnt && spree <= -10 && (-spree % 10) == 0 ) {
		gentity_t *te;
		snd_idx = ( -spree / 10 ) - 1;
		snd_idx = ( snd_idx < 6 ) ? snd_idx : 5;
		AP( va( "chat \"^1DEATH SPREE! ^*%s ^*%s ^d(^3%d ^ddeaths without a kill!)\"",
			ent->client->pers.netname,  humiliationMessage[snd_idx], -spree ) );
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/spree/humiliation.wav" );
		te->r.svFlags |= SVF_BROADCAST;
	}

	ent->client->ps.persistant[PERS_KILLSPREE] = spree;
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score ) {
	if ( !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

	ent->client->ps.persistant[PERS_SCORE] += score;
	if ( g_gametype.integer >= GT_TEAM || g_deathmatch.integer ) { // JPW NERVE changed to >=
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and items for the killed player
=================
*/
void MagicSink( gentity_t *self );
void TossClientItems( gentity_t *self ) {
	gitem_t     *item;
	int weapon;
	gentity_t   *drop = 0;

	weapon = self->s.weapon;

	// make a special check to see if they are changing to a new
	// weapon that isn't the mg or gauntlet.  Without this, a client
	// can pick up a weapon, be killed, and not drop the weapon because
	// their weapon change hasn't completed yet and they are still holding the MG.

	// (SA) always drop what you were switching to
	if ( self->client->ps.weaponstate == WEAPON_DROPPING ) {
		weapon = self->client->pers.cmd.weapon;
	}
	if ( !( COM_BitCheck( self->client->ps.weapons, weapon ) ) ) {
		weapon = WP_NONE;
	}

	// JPW NERVE don't drop these weapon types.
	if ( !g_unlockWeapons.integer && (( weapon == WP_FLAMETHROWER ) || ( weapon == WP_GARAND ) || ( weapon == WP_MAUSER ) || ( weapon == WP_VENOM )) ) {
		weapon = WP_NONE;
	}
	// jpw

	// don't drop these weapons in any mode
	weapon = (weapon == WP_KNIFE || weapon == WP_KNIFE2 ||
		weapon == WP_LUGER || weapon == WP_COLT ||
		weapon == WP_GRENADE_PINEAPPLE ||
		weapon == WP_GRENADE_LAUNCHER)
		? WP_NONE : weapon;

	if ( weapon > WP_NONE && weapon < WP_MEDIC_SYRINGE &&
		(self->client->ps.ammo[ BG_FindAmmoForWeapon( (weapon_t)weapon )] || self->client->ps.ammoclip[BG_FindClipForWeapon( (weapon_t)weapon )]) ) {
			// find the item type for this weapon
			item = BG_FindItemForWeapon( (weapon_t)weapon );
			// spawn the item

			// Rafael
			if ( item && !( self->client->ps.persistant[PERS_HWEAPON_USE] ) ) {
				drop = Drop_Item( self, item, 0, qfalse );
				// JPW NERVE -- fix ammo counts
				drop->count = self->client->ps.ammoclip[BG_FindClipForWeapon( (weapon_t)weapon )];
				drop->item->quantity = self->client->ps.ammo[BG_FindClipForWeapon( (weapon_t)weapon )];
				// jpw
			}
	}

	if ( g_dropItems.integer ) {
		int itemId = 0;
		int numToDrop = 2;

		switch(self->client->ps.stats[STAT_PLAYER_CLASS])
		{
		case PC_MEDIC:
			itemId = IL_ITEM_HEALTH;
			break;
		case PC_LT:
			itemId = IL_WEAPON_MAGICAMMO;
			break;
		case PC_ENGINEER:
			{
				int teamGrenade = WP_GRENADE_LAUNCHER;
				itemId = IL_AMMO_GRENADES;

				if ( self->client->sess.sessionTeam == TEAM_BLUE ) {
					itemId = IL_AMMO_GRENADES_AMERICAN;
					teamGrenade = WP_GRENADE_PINEAPPLE;
				}

				// drop a nade pack if they have 4 or more nades. no bots, we don't want too many laying around
				numToDrop = (int)(self->client->ps.ammoclip[BG_FindClipForWeapon( (weapon_t)teamGrenade )] / 4);
				numToDrop = (numToDrop > 1 && !(self->r.svFlags & SVF_BOT)) ? 1 : 0;
				break;
			}
		default:
			break;
		}

		if (itemId > 0)
		{
			int i;
			item = BG_FindItemByIndex( itemId );

			for ( i = 0; i < numToDrop; i++ )
			{
				// cs: revisit this if allowed to drop more than 2 items ...
				drop = Drop_Item( self, item, (float)(i + 60), i > 0 ? qtrue : qfalse );
				drop->think = MagicSink;
				drop->timestamp = level.time + 31200;
				drop->parent = NULL;
			}
		}
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
	vec3_t dir;

	if ( attacker && attacker != self ) {
		VectorSubtract( attacker->s.pos.trBase, self->s.pos.trBase, dir );
	} else if ( inflictor && inflictor != self ) {
		VectorSubtract( inflictor->s.pos.trBase, self->s.pos.trBase, dir );
	} else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw( dir );
}

/*
==============
GibHead
==============
*/
void GibHead( gentity_t *self, int killer ) {
	G_AddEvent( self, EV_GIB_HEAD, killer );
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer ) {
	gentity_t *other = &g_entities[killer];
	vec3_t dir;

	VectorClear( dir );
	if ( other->inuse ) {
		if ( other->client ) {
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		} else if ( !VectorCompare( other->s.pos.trDelta, vec3_origin ) ) {
			VectorNormalize2( other->s.pos.trDelta, dir );
		}
	}

	G_AddEvent( self, EV_GIB_PLAYER, DirToByte( dir ) );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	if ( self->health > GIB_HEALTH ) {
		return;
	}

	GibEntity( self, 0 );
}

// these are just for logging, the client prints its own messages
//CS: this should be correct
char    *modNames[] = {
	"MOD_UNKNOWN",              //0
	"MOD_SHOTGUN",              //1
	"MOD_GAUNTLET",             //2
	"MOD_MACHINEGUN",           //3
	"MOD_GRENADE",              //4
	"MOD_GRENADE_SPLASH",       //5
	"MOD_ROCKET",               //6
	"MOD_ROCKET_SPLASH",        //7
	"MOD_RAILGUN",              //8
	"MOD_LIGHTNING",            //9
	"MOD_BFG",                  //10
	"MOD_BFG_SPLASH",           //11
	"MOD_KNIFE",                //12
	"MOD_KNIFE2",               //13
	"MOD_KNIFE_STEALTH",        //14
	"MOD_LUGER",                //15
	"MOD_COLT",                 //16
	"MOD_MP40",                 //17
	"MOD_THOMPSON",             //18
	"MOD_STEN",                 //19
	"MOD_MAUSER",               //20
	"MOD_SNIPERRIFLE",          //21
	"MOD_GARAND",               //22
	"MOD_SNOOPERSCOPE",         //23
	"MOD_SILENCER",             //24
	"MOD_AKIMBO",               //25
	"MOD_BAR",                  //26
	"MOD_FG42",                 //27
	"MOD_FG42SCOPE",            //28
	"MOD_PANZERFAUST",          //29
	"MOD_ROCKET_LAUNCHER",      //30
	"MOD_GRENADE_LAUNCHER",     //31
	"MOD_VENOM",                //32
	"MOD_VENOM_FULL",           //33
	"MOD_FLAMETHROWER",         //34
	"MOD_TESLA",                //35
	"MOD_SPEARGUN",             //36
	"MOD_SPEARGUN_CO2",         //37
	"MOD_GRENADE_PINEAPPLE",    //38
	"MOD_CROSS",                //39
	"MOD_MORTAR",               //40
	"MOD_MORTAR_SPLASH",        //41
	"MOD_KICKED",               //42
	"MOD_GRABBER",              //43
	"MOD_DYNAMITE",             //44
	"MOD_DYNAMITE_SPLASH",      //45
	"MOD_AIRSTRIKE",            //46
	"MOD_SYRINGE",              //47
	"MOD_AMMO",                 //48
	"MOD_ARTY",                 //49
	"MOD_WATER",                //50
	"MOD_SLIME",                //51
	"MOD_LAVA",                 //52
	"MOD_CRUSH",                //53
	"MOD_TELEFRAG",             //54
	"MOD_FALLING",              //55
	"MOD_SUICIDE",              //56
	"MOD_TARGET_LASER",         //57
	"MOD_TRIGGER_HURT",         //58
	"MOD_GRAPPLE",              //59
	"MOD_EXPLOSIVE",            //60
	"MOD_POISONGAS",            //61
	"MOD_ZOMBIESPIT",           //62
	"MOD_ZOMBIESPIT_SPLASH",    //63
	"MOD_ZOMBIESPIRIT",         //64
	"MOD_ZOMBIESPIRIT_SPLASH",  //65
	"MOD_LOPER_LEAP",           //66
	"MOD_LOPER_GROUND",         //67
	"MOD_LOPER_HIT",            //68
	"MOD_LT_AMMO",              //69
	"MOD_LT_AIRSTRIKE",         //70
	"MOD_ENGINEER",             //71
	"MOD_MEDIC",                //72
	"MOD_BAT",                  //73
	"MOD_KNIFE_THROWN",         //74
	"MOD_GOOMBA",               //75
	"MOD_POISON",               //76
};

/*
==================
player_die
==================
*/
void limbo( gentity_t *ent, qboolean makeCorpse ); // JPW NERVE

void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	gentity_t   *ent;
	// TTimo might be used uninitialized
	int contents = 0;
	int killer;
	char        *killerName, *obit;
	qboolean nogib = qtrue;
	gitem_t     *item = NULL; // JPW NERVE for flag drop
	vec3_t launchvel;      // JPW NERVE
	gentity_t   *flag; // JPW NERVE

	if ( self->client->ps.pm_type == PM_DEAD ) {
		return;
	}

	if ( level.intermissiontime ) {
		return;
	}

	G_UpdateKillingSpree( self, attacker, qtrue );

	self->client->ps.pm_type = PM_DEAD;
	self->client->ps.persistant[PERS_HWEAPON_USE] = 0; // dead players can't be mounted

	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0 );

	// g_shortcuts
	if ( attacker && attacker->client ) {
		self->client->pers.lastkiller_client = attacker->s.number;
		attacker->client->pers.lastkilled_client = self->s.number;
	}

	if ( attacker ) {
		killer = attacker->s.number;
		if ( attacker->client ) {
			killerName = attacker->client->pers.netname;
		} else {
			killerName = "<non-client>";
		}
	} else {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( killer < 0 || killer >= MAX_CLIENTS ) {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
		obit = "<bad obituary>";
	} else {
		obit = modNames[ meansOfDeath ];

#ifdef WITH_LUA
		G_LuaHook_Obituary( self->s.number, killer, meansOfDeath, obit );
#endif
	}

	G_LogPrintf( "Kill: %i %i %i: %s killed %s by %s\n",
		killer, self->s.number, meansOfDeath, killerName,
		self->client->pers.netname, obit );

	//stats
	if ( attacker && attacker->client ) {
		if ( !OnSameTeam( attacker, self ) ) {
			switch(meansOfDeath)
			{
			case MOD_GOOMBA:
				attacker->client->pers.goomba++;
				break;
			case MOD_POISON:
				attacker->client->pers.poison++;
				break;
			case MOD_KNIFE:
			case MOD_KNIFE_STEALTH:
			case MOD_KNIFE_THROWN:
				attacker->client->pers.knifeKill++;
				break;
			default:
				break;
			}

			attacker->client->pers.kills++;
			attacker->client->ps.persistant[PERS_KILLS]++;
			attacker->client->multikill++;
			G_UpdateKillingSpree( attacker, NULL, qfalse );

			// credits
			attacker->client->sess.credits += CREDITS_KILLBONUS;
			if ( attacker->client->sess.currentBetTarget == self ) {
				// transfer the credits
				attacker->client->sess.credits += attacker->client->sess.currentBetAmount;
				self->client->sess.credits -= self->client->sess.currentBetAmount;
				// let them know about it
				trap_SendServerCommand( attacker - g_entities, va( "chat \"^3You have won ^2%i ^3credits!\"",attacker->client->sess.currentBetAmount ) );
				trap_SendServerCommand( self - g_entities, va( "chat \"^3You lost ^1%i ^3credits!\"",self->client->sess.currentBetAmount ) );
				// clear the bet
				G_ClearBet(attacker);
			}
		} else if ( meansOfDeath == MOD_SUICIDE ) {
			attacker->client->pers.suicides++;
		} else {
			attacker->client->pers.teamKills++;
		}
	}

	//send the events to the bot
	Bot_Event_Death( self - g_entities, &g_entities[attacker - g_entities], obit );
	Bot_Event_KilledSomeone( attacker - g_entities, &g_entities[self - g_entities], obit );

	//////////////////////////////////////////////////////////////////////////

	// broadcast the death event to everyone
	ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
	ent->s.eventParm = meansOfDeath;
	ent->s.otherEntityNum = self->s.number;
	ent->s.otherEntityNum2 = killer;
	ent->r.svFlags = SVF_BROADCAST; // send to everyone

	self->enemy = attacker;

	self->client->pers.deaths++;

	if ( ( self->client->ps.grenadeTimeLeft ) && ( self->s.weapon != WP_DYNAMITE ) ) {
		vec3_t launchspot;
		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = random();
		VectorScale( launchvel, 160, launchvel );
		VectorCopy( self->r.currentOrigin, launchspot );
		launchspot[2] += 40;
		fire_grenade( self, launchspot, launchvel, self->s.weapon );
	}

	if ( attacker && attacker->client ) {
		if ( attacker == self || OnSameTeam( self, attacker ) ) {
			// DHM - Nerve :: Complaint lodging
			if ( attacker != self && level.warmupTime <= 0 ) {
				if ( attacker->client->pers.localClient || ( attacker->r.svFlags & SVF_BOT ) ) {
					//trap_SendServerCommand( self-g_entities, "complaint -4" );
				} else if ( !(self->r.svFlags &SVF_BOT) ){
					trap_SendServerCommand( self - g_entities, va( "complaint %i", attacker->s.number ) );
					self->client->pers.complaintClient = attacker->s.clientNum;
					self->client->pers.complaintEndTime = level.time + 20500;
				}
			}
			// dhm

			// JPW NERVE
			if ( !g_deathmatch.integer && g_gametype.integer >= GT_WOLF ) { // high penalty to offset medic heal
				AddScore( attacker, WOLF_FRIENDLY_PENALTY );
			} else {
				// jpw
				AddScore( attacker, -1 );
			}
		} else {
			// JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
			if ( g_gametype.integer >= GT_WOLF ) {
				AddScore( attacker, WOLF_FRAG_BONUS );
			} else {
				// jpw
				AddScore( attacker, 1 );
			}

			attacker->client->lastKillTime = level.time;
		}
	} else {
		AddScore( self, -1 );
	}

	// Add team bonuses
	Team_FragBonuses( self, inflictor, attacker );

	// drop flag regardless
	if ( self->client->ps.powerups[PW_REDFLAG] ) {
		item = BG_FindItemByIndex( IL_TEAM_CTF_REDFLAG );

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	if ( self->client->ps.powerups[PW_BLUEFLAG] ) {
		item = BG_FindItemByIndex( IL_TEAM_CTF_BLUEFLAG );

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if ( item ) {
		launchvel[0] = crandom() * 20;
		launchvel[1] = crandom() * 20;
		launchvel[2] = 10 + random() * 10;

		flag = LaunchItem( item,self->r.currentOrigin,launchvel,self->s.number );
		flag->s.modelindex2 = self->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;  // DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}

	// send a fancy "MEDIC!" scream.  Sissies, ain' they?
	if ( self->client != NULL ) {
		if ( self->health > GIB_HEALTH && meansOfDeath != MOD_SUICIDE ) {
			if ( self->client->sess.sessionTeam == TEAM_RED ) {
				if ( random() > 0.5 ) {
					G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic2.wav" ) );
				} else {
					G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic3.wav" ) );
				}
			} else {
				if ( random() > 0.5 ) {
					G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic3.wav" ) );
				} else {
					G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic2.wav" ) );
				}
			}

			// ATM: only register the goal if the target isn't in water.
			if ( self->waterlevel <= 1 /*|| g_waterRevive.integer*/ ) {
				Bot_AddFallenTeammateGoals( self, self->client->sess.sessionTeam );
			}
		}
	}

	Cmd_Score_f( self );        // show scores

	self->takedamage = qtrue;   // can still be gibbed
	self->r.contents = CONTENTS_CORPSE;
	self->s.powerups = 0;
	self->client->limboDropWeapon = self->s.weapon; // store this so it can be dropped in limbo
	self->s.angles[2] = 0;

	if ( !( self->r.svFlags & SVF_BOT ) ) {
		VectorCopy( self->s.angles, self->client->ps.viewangles );
		LookAtKiller( self, inflictor, attacker );
	}

	self->s.loopSound = 0;

	//trap_UnlinkEntity( self );
	self->r.maxs[2] = 8;
	self->client->ps.maxs[2] = 8;
	trap_LinkEntity( self );

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.time + 800;

	// remove powerups
	memset( self->client->ps.powerups, 0, sizeof( self->client->ps.powerups ) );

	// never gib in a nodrop
	if ( self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) ) {
		GibEntity( self, killer );
		nogib = qfalse;
	}

	if(nogib) {
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH+1;
		}
		self->client->medicHealAmt = 0;
		self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, ANIM_ET_DEATH, qfalse, qtrue );
		G_AddEvent( self, EV_DEATH1 + 1, killer );
		self->die = body_die;
	}

	if ( g_announcer.integer &&
		!firstblood &&
		self &&
		self->client &&
		attacker &&
		attacker->client &&
		attacker->s.number != ENTITYNUM_NONE &&
		attacker->s.number != ENTITYNUM_WORLD &&
		attacker != self &&
		g_gamestate.integer == GS_PLAYING &&
		!OnSameTeam( self, attacker ) ) {
			gentity_t *te = G_TempEntity( level.intermission_origin, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/spree/firstblood.wav" );
			te->r.svFlags |= SVF_BROADCAST;
			AP( va( "chat \"^7%s ^7drew ^1FIRST BLOOD ^7from ^7%s^7!\" -1",
				attacker->client->pers.netname, self->client->pers.netname ) );

			firstblood = qtrue;
	}

	if ( g_gametype.integer >= GT_WOLF && meansOfDeath == MOD_SUICIDE ) {
		limbo( self, qtrue );
	}
}

qboolean IsHeadShotWeapon( int mod ) {
	// players are allowed headshots from these weapons
	if (    mod == MOD_LUGER ||
		mod == MOD_COLT ||
		mod == MOD_AKIMBO ||    //----(SA)	added
		mod == MOD_MP40 ||
		mod == MOD_THOMPSON ||
		mod == MOD_STEN ||
		mod == MOD_BAR ||
		mod == MOD_FG42 ||
		mod == MOD_FG42SCOPE ||
		mod == MOD_MAUSER ||
		mod == MOD_GARAND || // JPW NERVE this was left out
		mod == MOD_SNIPERRIFLE ||
		mod == MOD_SNOOPERSCOPE ||
		mod == MOD_SILENCER ||  //----(SA)	modified
		mod == MOD_SNIPERRIFLE ) {
			return qtrue;
	}

	return qfalse;
}

qboolean IsHeadShot( gentity_t *targ, vec3_t dir, vec3_t point, int mod ) {
	gentity_t   *head;
	trace_t tr;
	vec3_t start;
	gentity_t   *traceEnt;
	orientation_t or;           // DHM - Nerve

	qboolean head_shot_weapon = qfalse;

	// not a player or critter so bail
	if ( !( targ->client ) ) {
		return qfalse;
	}

	if ( targ->health <= 0 ) {
		return qfalse;
	}

	head_shot_weapon = IsHeadShotWeapon( mod );

	if ( head_shot_weapon ) {
		vec3_t end;
		head = G_Spawn();

		if ( trap_GetTag( targ->s.number, "tag_head", &or ) ) {
			G_SetOrigin( head, or.origin );
		} else {
			float height, dest;
			vec3_t v, angles, forward, up, right;

			G_SetOrigin( head, targ->r.currentOrigin );

			if ( targ->client->ps.pm_flags & PMF_DUCKED ) { // closer fake offset for 'head' box when crouching
				height = targ->client->ps.crouchViewHeight - 12;
			} else {
				height = targ->client->ps.viewheight;
			}

			// NERVE - SMF - this matches more closely with WolfMP models
			VectorCopy( targ->client->ps.viewangles, angles );
			if ( angles[PITCH] > 180 ) {
				dest = ( -360 + angles[PITCH] ) * 0.75;
			} else {
				dest = angles[PITCH] * 0.75;
			}
			angles[PITCH] = dest;

			AngleVectors( angles, forward, right, up );
			VectorScale( forward, 5, v );
			VectorMA( v, 18, up, v );

			VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
			head->r.currentOrigin[2] += height / 2;
			// -NERVE - SMF
		}

		VectorCopy( head->r.currentOrigin, head->s.origin );
		VectorCopy( targ->r.currentAngles, head->s.angles );
		VectorCopy( head->s.angles, head->s.apos.trBase );
		VectorCopy( head->s.angles, head->s.apos.trDelta );
		VectorSet( head->r.mins, -6, -6, -2 ); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
		VectorSet( head->r.maxs, 6, 6, 10 ); // changed this z from 0 to 6
		head->clipmask = CONTENTS_SOLID;
		head->r.contents = CONTENTS_SOLID;

		trap_LinkEntity( head );

		// trace another shot see if we hit the head
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );

		traceEnt = &g_entities[ tr.entityNum ];

		if ( g_debugBullets.integer >= 3 ) {   // show hit player head bb
			gentity_t *tent;
			vec3_t b1, b2;
			VectorCopy( head->r.currentOrigin, b1 );
			VectorCopy( head->r.currentOrigin, b2 );
			VectorAdd( b1, head->r.mins, b1 );
			VectorAdd( b2, head->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if ( tr.fraction != 1 ) {
				VectorMA( start, ( tr.fraction * 64 ), dir, end );
			}
			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}

		G_FreeEntity( head );

		if ( traceEnt == head ) {
			level.totalHeadshots++;         // NERVE - SMF
			return qtrue;
		} else {
			level.missedHeadshots++;    // NERVE - SMF
		}
	}

	return qfalse;
}

gentity_t* G_BuildHead( gentity_t *ent ) {
	gentity_t* head;
	orientation_t or;           // DHM - Nerve

	head = G_Spawn();

	if ( trap_GetTag( ent->s.number, "tag_head", &or ) ) {
		G_SetOrigin( head, or.origin );
	} else {
		float height, dest;
		vec3_t v, angles, forward, up, right;

		G_SetOrigin( head, ent->r.currentOrigin );

		if ( ent->client->ps.pm_flags & PMF_DUCKED ) { // closer fake offset for 'head' box when crouching
			height = ent->client->ps.crouchViewHeight - 12;
		} else {
			height = ent->client->ps.viewheight;
		}

		// NERVE - SMF - this matches more closely with WolfMP models
		VectorCopy( ent->client->ps.viewangles, angles );
		if ( angles[PITCH] > 180 ) {
			dest = ( -360 + angles[PITCH] ) * 0.75;
		} else {
			dest = angles[PITCH] * 0.75;
		}
		angles[PITCH] = dest;

		AngleVectors( angles, forward, right, up );
		VectorScale( forward, 5, v );
		VectorMA( v, 18, up, v );

		VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
		head->r.currentOrigin[2] += height / 2;
		// -NERVE - SMF
	}

	VectorCopy( head->r.currentOrigin, head->s.origin );
	VectorCopy( ent->r.currentAngles, head->s.angles );
	VectorCopy( head->s.angles, head->s.apos.trBase );
	VectorCopy( head->s.angles, head->s.apos.trDelta );
	VectorSet( head->r.mins, -6, -6, -2 ); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	VectorSet( head->r.maxs, 6, 6, 10 ); // changed this z from 0 to 6
	head->clipmask = CONTENTS_SOLID;
	head->r.contents = CONTENTS_SOLID;
	head->parent = ent;
	head->s.eType = ET_TEMPHEAD;

	trap_LinkEntity( head );

	return head;
}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
DAMAGE_NO_ARMOR			armor does not protect from this damage
DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
const char *_GetEntityName( gentity_t *_ent );
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
			  vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
				  gclient_t   *client;
				  int take;
				  int knockback;
				  qboolean headShot;
				  qboolean skipHitsounds;
				  qboolean sameTeam;

				  if ( !targ->takedamage ) {
					  return;
				  }

				  // the intermission has allready been qualified for, so don't
				  // allow any extra scoring
				  if ( level.intermissionQueued || (g_gamestate.integer != GS_PLAYING && g_warmupDamage.integer == 0 && targ->client) ) {
					  return;
				  }

				  if ( !inflictor ) {
					  inflictor = &g_entities[ENTITYNUM_WORLD];
				  }

				  if ( !attacker ) {
					  attacker = &g_entities[ENTITYNUM_WORLD];
				  }

				  if ( ( mod == MOD_FLAMETHROWER ) && ( targ->waterlevel >= 3 ) ) {
					  return;
				  }

				  // shootable doors / buttons don't actually have any health
				  if ( targ->s.eType == ET_MOVER && !( targ->isProp ) && !targ->scriptName ) {
					  if ( !( targ->aiName ) ) {
						  if ( targ->use && targ->moverState == MOVER_POS1 ) {
							  targ->use( targ, inflictor, attacker );
						  }
						  return;
					  } else {
						  switch ( mod ) {
						  case MOD_GRENADE:
						  case MOD_GRENADE_SPLASH:
						  case MOD_ROCKET:
						  case MOD_ROCKET_SPLASH:
							  break;
						  default:
							  return; // no damage from other weapons
						  }
					  }
				  } else if ( targ->s.eType == ET_EXPLOSIVE ) {
					  // 32 Explosive
					  // 64 Dynamite only
					  if ( ( targ->spawnflags & 32 ) || ( targ->spawnflags & 64 ) ) {
						  switch ( mod ) {
						  case MOD_GRENADE:
						  case MOD_GRENADE_SPLASH:
						  case MOD_ROCKET:
						  case MOD_ROCKET_SPLASH:
						  case MOD_AIRSTRIKE:
						  case MOD_GRENADE_PINEAPPLE:
						  case MOD_MORTAR:
						  case MOD_MORTAR_SPLASH:
						  case MOD_EXPLOSIVE:
							  if ( targ->spawnflags & 64 ) {
								  return;
							  }

							  break;

						  case MOD_DYNAMITE:
						  case MOD_DYNAMITE_SPLASH:
							  break;

						  default:
							  return;
						  }
					  }
				  }

				  client = targ->client;
				  if ( client && client->noclip ) {
					  return;
				  }

				  if ( !dir ) {
					  dflags |= DAMAGE_NO_KNOCKBACK;
				  } else {
					  VectorNormalize( dir );
				  }

				  knockback = damage;
				  if ( ( targ->flags & FL_NO_KNOCKBACK ) || ( dflags & DAMAGE_NO_KNOCKBACK ) ) {
					  knockback = 0;
				  } else if ( knockback > 200 ) {
					  knockback = 200;
				  }

				  sameTeam = OnSameTeam( targ, attacker );

				  // figure momentum add, even if the damage won't be taken
				  if ( knockback && targ->client && ( g_friendlyFire.integer || !sameTeam ) ) {
					  vec3_t kvel;
					  float mass;

					  mass = 200;

					  VectorScale( dir, g_knockback.value * (float)knockback / mass, kvel );
					  VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );

					  if ( targ == attacker && !(  mod != MOD_ROCKET &&
						  mod != MOD_ROCKET_SPLASH &&
						  mod != MOD_GRENADE &&
						  mod != MOD_GRENADE_SPLASH &&
						  mod != MOD_DYNAMITE ) ) {
							  targ->client->ps.velocity[2] *= 0.25;
					  }

					  // set the timer so that the other client can't cancel
					  // out the movement immediately
					  if ( !targ->client->ps.pm_time ) {
						  int t;

						  t = knockback * 2;
						  if ( t < 50 ) {
							  t = 50;
						  } else if ( t > 200 ) {
							  t = 200;
						  }
						  targ->client->ps.pm_time = t;
						  targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
					  }
				  }

				  // check for completely getting out of the damage
				  if ( !( dflags & DAMAGE_NO_PROTECTION ) ) {
					  if ( targ != attacker && sameTeam && !g_friendlyFire.integer ) {
						  return;
					  }

					  // check for godmode
					  if ( targ->flags & FL_GODMODE ) {
						  return;
					  }

					  // check for invulnerability // (SA) moved from below so DAMAGE_NO_PROTECTION will still work
					  if ( client && client->ps.powerups[PW_INVULNERABLE] ) { //----(SA)	added
						  return;
					  }
				  }

				  if ( damage < 1 ) {
					  damage = 1;
				  }
				  take = damage;

				  headShot = IsHeadShot( targ, dir, point, mod );
				  if ( headShot ) {
					  if ( take * 2 < 50 ) { // head shots, all weapons, do minimum 50 points damage
						  if ( ( !( targ->client->ps.eFlags & EF_HEADSHOT ) ) && ( g_helmetProtection.integer ) ) {
							  take = 20;
						  } else {
							  take = 50;
						  }
					  } else {
						  take *= 2; // sniper rifles can do full-kill (and knock into limbo)
					  }

					  if ( targ->client ) {
						  if ( !( targ->client->ps.eFlags & EF_HEADSHOT ) ) {  // only toss hat on first headshot
							  G_AddEvent( targ, EV_LOSE_HAT, DirToByte( dir ) );
						  }

						  targ->client->ps.eFlags |= EF_HEADSHOT;
					  }

					  //stats
					  if ( attacker->client && targ->client && !sameTeam ) {
						  attacker->client->pers.headshots++;
					  }

					  if ( g_instaGib.integer ) {
						  take = 999;
					  }
				  } else if ( g_instaGib.integer && attacker->client && targ->client ) {
					  switch ( mod )
					  {
					  case MOD_MAUSER:
					  case MOD_LUGER:
					  case MOD_COLT:
					  case MOD_KNIFE:
					  case MOD_KNIFE2:
					  case MOD_MP40:
					  case MOD_THOMPSON:
					  case MOD_STEN:
						  take = 0;
						  break;
					  default:
						  take = 999;
						  break;
					  }
				  }

				  // skip hitsounds for some mods
				  switch ( mod )
				  {
				  case MOD_FLAMETHROWER:
				  case MOD_POISON:
				  case MOD_DYNAMITE:
				  case MOD_DYNAMITE_SPLASH:
				  case MOD_GRENADE:
				  case MOD_AIRSTRIKE:
				  case MOD_ARTY:
					  skipHitsounds = qtrue;
					  break;
				  default:
					  skipHitsounds = qfalse;
					  break;
				  }

				  //hitsounds
				  if ( !skipHitsounds && attacker->client && targ != attacker && targ->client ) {
					  if ( !headShot ) {
						  if ( sameTeam ) {
							  attacker->client->ps.persistant[PERS_HITS] -= take;
						  } else {
							  attacker->client->ps.persistant[PERS_HITS] += take;
						  }
					  } else {
						  if ( sameTeam ) {
							  attacker->client->ps.persistant[PERS_HITS] -= take;
						  } else {
							  attacker->client->ps.persistant[PERS_BLEH_2] += take;
						  }
					  }
				  }

				  if ( g_debugDamage.integer ) {
					  G_Printf( "client:%i health:%i damage:%i\n", targ->s.number,
						  targ->health, take );
				  }

				  // add to the damage inflicted on a player this frame
				  // the total will be turned into screen blends and view angle kicks
				  // at the end of the frame
				  if ( client ) {
					  if ( attacker ) {
						  client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
					  } else {
						  client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
					  }

					  client->damage_blood += take;
					  client->damage_knockback += knockback;

					  if ( dir ) {
						  VectorCopy( dir, client->damage_from );
						  client->damage_fromWorld = qfalse;
					  } else {
						  VectorCopy( targ->r.currentOrigin, client->damage_from );
						  client->damage_fromWorld = qtrue;
					  }
				  }

				  if ( targ->client ) {
					  // set the last client who damaged the target
					  targ->client->lasthurt_client = attacker->s.number;
					  targ->client->lasthurt_mod = mod;
				  }

				  if ( ( attacker && attacker->client ) && ( targ && targ->client ) ) {
					  if ( targ->client->ps.stats[STAT_HEALTH] > 0 ) {
						  if ( !sameTeam ) {
							  attacker->client->pers.damage_given += take;
							  targ->client->pers.damage_received += take;
						  } else {
							  attacker->client->pers.team_damage += take;
						  }
					  }
				  }

				  // need to trigger the explodes filtered for health before the damage ...
				  if ( take && take > targ->health && targ->s.number >= MAX_CLIENTS && targ->health > 50 ) {
					  if ( targ->spawnflags & 16 ) {
						  if ( Q_stricmp( _GetEntityName( targ ), "" ) ) {
							  Bot_Util_SendTrigger( targ, NULL, va( "Explode_%s Exploded.", _GetEntityName( targ ) ), "exploded" );
						  } else {
							  Bot_Util_SendTrigger( targ, NULL, va( "Explode_%d Exploded", targ->s.number ), "exploded" );
						  }
					  }
				  }

				  // do the damage
				  if ( take ) {
					  targ->health = targ->health - take;

					  // Ridah, can't gib with bullet weapons (except VENOM)
					  if ( mod != MOD_VENOM && attacker == inflictor && targ->health <= GIB_HEALTH ) {
						  targ->health = GIB_HEALTH + 1;
					  }

					  // JPW NERVE overcome previous chunk of code for making grenades work again
					  if ( (targ->s.number < MAX_CLIENTS) && (take > 190) ) { // 190 is greater than 2x mauser headshot, so headshots don't gib
						  targ->health = GIB_HEALTH - 1;
					  }
					  // jpw
					  if ( targ->health <= 0 ) {
						  if ( client ) {
							  targ->flags |= FL_NO_KNOCKBACK;

							  if ( g_gametype.integer >= GT_WOLF ) {
								  if ( ( targ->health < FORCE_LIMBO_HEALTH ) && ( targ->health > GIB_HEALTH ) && ( !( targ->client->ps.pm_flags & PMF_LIMBO ) ) ) {
									  limbo( targ, qtrue );

									  if ( !attacker->client && targ->client ) {
										  if ( g_reportGibs.integer ) {
											  trap_SendServerCommand( -1, va( "print \"%s^7 was gibbed.\n\"", targ->client->pers.netname ) );
										  }
									  }
									  else if ( attacker->client && targ->client ) {
										  if ( !sameTeam ) {
											  if ( g_reportGibs.integer ) {
												  trap_SendServerCommand( -1, va( "print \"%s^7 gibbed %s^7.\n\"",
													  attacker->client->pers.netname, targ->client->pers.netname ) );
											  }

											  attacker->client->pers.gibs++;
										  } else {
											  if ( g_reportGibs.integer ) {
												  if ( attacker == targ ) {
													  trap_SendServerCommand( -1, va( "print \"%s^7 gibbed himself.\n\"", targ->client->pers.netname ) );
												  }
												  else {
													  trap_SendServerCommand( -1, va( "print \"%s^3 was gibbed by TEAMMATE^7 %s\n\"",
														  targ->client->pers.netname, attacker->client->pers.netname ) );
												  }
											  }

											  attacker->client->pers.teamGibs++;
										  }
									  }
								  }
							  }
						  }

						  if ( targ->health < -999 ) {
							  targ->health = -999;
						  }

						  targ->enemy = attacker;
						  if ( targ->die ) { // Ridah, mg42 doesn't have die func (FIXME)
							  targ->die( targ, inflictor, attacker, take, mod );
						  }

						  // if we freed ourselves in death function
						  if ( !targ->inuse ) {
							  return;
						  }

						  // RF, entity scripting
						  if ( targ->s.number >= MAX_CLIENTS && targ->health <= 0 ) { // might have revived itself in death function
							  G_Script_ScriptEvent( targ, "death", "" );
						  }
					  } else if ( targ->pain ) {
						  if ( dir ) {  // Ridah, had to add this to fix NULL dir crash
							  VectorCopy( dir, targ->rotate );
							  VectorCopy( point, targ->pos3 ); // this will pass loc of hit
						  } else {
							  VectorClear( targ->rotate );
							  VectorClear( targ->pos3 );
						  }

						  targ->pain( targ, attacker, take, point );

						  // RF, entity scripting
						  if ( targ->s.number >= MAX_CLIENTS ) {
							  G_Script_ScriptEvent( targ, "pain", va( "%d %d", targ->health, targ->health + take ) );
						  }
					  }

					  // Ridah, this needs to be done last, incase the health is altered in one of the event calls
					  if ( targ->client ) {
						  targ->client->ps.stats[STAT_HEALTH] = targ->health;
					  }

					  if ( targ->s.number < MAX_CLIENTS ) {
						  // notify omni-bot framework
						  Bot_Event_TakeDamage( targ - g_entities, attacker );
					  }
				  }
}

void G_RailBox( vec_t* origin, vec_t* mins, vec_t* maxs, vec_t* color, int index ) {
	vec3_t b1;
	vec3_t b2;
	gentity_t* temp;

	VectorCopy( origin, b1 );
	VectorCopy( origin, b2 );
	VectorAdd( b1, mins, b1 );
	VectorAdd( b2, maxs, b2 );

	temp = G_TempEntity( b1, EV_RAILTRAIL );

	VectorCopy( b2, temp->s.origin2 );
	VectorCopy( color, temp->s.angles );
	temp->s.dmgFlags = 1;

	temp->s.angles[0] = (int)( color[0] * 255 );
	temp->s.angles[1] = (int)( color[1] * 255 );
	temp->s.angles[2] = (int)( color[2] * 255 );

	temp->s.effect1Time = index + 1;
}

/*
============
CanDamage ET version
Martin - Indoor artillery
============
*/
qboolean CanDamage( gentity_t *targ, vec3_t origin ) {
	vec3_t dest;
	trace_t tr;
	vec3_t midpoint;
	vec3_t offsetmins = { -16.f, -16.f, -16.f };
	vec3_t offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if ( targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2] ) {
		VectorCopy( targ->r.currentOrigin, midpoint );

		if ( targ->s.eType == ET_MOVER ) {
			midpoint[2] += 32;
		}
	} else {
		VectorAdd( targ->r.absmin, targ->r.absmax, midpoint );
		VectorScale( midpoint, 0.5, midpoint );
	}

	trap_Trace( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1.0 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	if ( targ->client ) {
		VectorCopy( targ->client->ps.mins, offsetmins );
		VectorCopy( targ->client->ps.maxs, offsetmaxs );
	}

	// cs: test. send one trace with the bounds rather than 8 separate. seems fine so far
	// cs: bleh, it appears to break when tracing to an mg42 .. nades won't damage them.
	//VectorCopy( midpoint, dest );
	//trap_Trace( &tr, origin, offsetmins, offsetmaxs, dest, ENTITYNUM_NONE, MASK_SOLID );
	//if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
	//G_Printf("CanDamage: bounds trace success\n");
	//	return qtrue;
	//}
	//else {
	//	G_Printf("CanDamage: bounds trace fail\n");
	//}

	// this should probably check in the plane of projection,
	// rather than in world coordinate
	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	// =========================

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	return qfalse;
}
// end martin
/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *attacker, float damage, float radius,
						gentity_t *ignore, int mod ) {
							float points, dist;
							gentity_t   *ent;
							int entityList[MAX_GENTITIES];
							int numListedEntities;
							vec3_t mins, maxs;
							vec3_t v;
							vec3_t dir;
							int i, e;
							qboolean hitClient = qfalse;
							// JPW NERVE
							float boxradius;
							// jpw

							if ( radius < 1 ) {
								radius = 1;
							}

							boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement --
							// bounding box was checking against radius / sqrt(2) if collision is along box plane
							for ( i = 0 ; i < 3 ; i++ ) {
								mins[i] = origin[i] - boxradius; // JPW NERVE
								maxs[i] = origin[i] + boxradius; // JPW NERVE
							}

							numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

							for ( e = 0 ; e < numListedEntities ; e++ ) {
								ent = &g_entities[entityList[ e ]];

								if ( ent == ignore ) {
									continue;
								}
								if ( !ent->takedamage ) {
									if (mod != MOD_DYNAMITE_SPLASH && ent->s.weapon != WP_DYNAMITE) {
										continue;
									}
								}

								if ( !ent->r.bmodel ) {
									VectorSubtract( ent->r.currentOrigin,origin,v ); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
								} else {
									for ( i = 0 ; i < 3 ; i++ ) {
										if ( origin[i] < ent->r.absmin[i] ) {
											v[i] = ent->r.absmin[i] - origin[i];
										} else if ( origin[i] > ent->r.absmax[i] ) {
											v[i] = origin[i] - ent->r.absmax[i];
										} else {
											v[i] = 0;
										}
									}
								}

								dist = VectorLength( v );
								if ( dist >= radius ) {
									continue;
								}

								// explode any other dyno's
								if ( mod == MOD_DYNAMITE_SPLASH && ent->s.weapon == WP_DYNAMITE ) {
									// cs: only if it is a teammates dyno
									if ( ent->parent && ent->parent->client && attacker->client && ent->parent->client->sess.sessionTeam == attacker->client->sess.sessionTeam ) {
										// and not if they both went to spec ...
										if ( attacker->client->sess.sessionTeam != TEAM_SPECTATOR ) {
											ent->nextthink = level.time + 250;
										}
									}
								}

								// JPW NERVE -- different radiusdmg behavior for MP -- big explosions should do less damage (over less distance) through failed traces
								if ( CanDamage( ent, origin ) ) {
									if ( LogAccuracyHit( ent, attacker ) ) {
										hitClient = qtrue;
									}
									VectorSubtract( ent->r.currentOrigin, origin, dir );
									// push the center of mass higher than the origin so players
									// get knocked into the air more
									dir[2] += 24;
									points = damage * ( 1.0 - dist / radius );
									G_Damage( ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod );
								}
								// jpw
							}
							return hitClient;
}
// Begin indoor arty
/*
============
G_AdjustedDamageVec

for indoor arty stuff from sandmod ...
============
*/

void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t v ) {
	if ( !ent->r.bmodel ) {
		VectorSubtract( ent->r.currentOrigin,origin,v ); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
	} else {
		int i;
		for ( i = 0 ; i < 3 ; i++ ) {
			if ( origin[i] < ent->r.absmin[i] ) {
				v[i] = ent->r.absmin[i] - origin[i];
			} else if ( origin[i] > ent->r.absmax[i] ) {
				v[i] = origin[i] - ent->r.absmax[i];
			} else {
				v[i] = 0;
			}
		}
	}
}

/*
============
G_ET_RadiusDamage

for indoor arty stuff from sandmod ...
============
*/
qboolean G_ET_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod ) {
	float points, dist;
	gentity_t   *ent;
	int entityList[MAX_GENTITIES];
	int numListedEntities;
	vec3_t mins, maxs;
	vec3_t v;
	vec3_t dir;
	int i, e;
	qboolean hitClient = qfalse;
	float boxradius;
	int flags = DAMAGE_RADIUS;

	if ( radius < 1 ) {
		radius = 1;
	}

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement --
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < level.num_entities ; e++ ) {
		g_entities[e].dmginloop = qfalse;
	}

	for ( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if ( ent == ignore ) {
			continue;
		}
		if ( !ent->takedamage && ( !ent->dmgparent || !ent->dmgparent->takedamage ) ) {
			if (mod != MOD_DYNAMITE_SPLASH && ent->s.weapon != WP_DYNAMITE) {
				continue;
			}
		}

		G_AdjustedDamageVec( ent, origin, v );

		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		// explode any other dyno's
		if ( mod == MOD_DYNAMITE_SPLASH && ent->s.weapon == WP_DYNAMITE ) {
			// cs: only if it is a teammates dyno
			if ( ent->parent && ent->parent->client && attacker->client && ent->parent->client->sess.sessionTeam == attacker->client->sess.sessionTeam ) {
				// and not if they both went to spec ...
				if ( attacker->client->sess.sessionTeam != TEAM_SPECTATOR ) {
					ent->nextthink = level.time + 250;
				}
			}
		}

		if ( CanDamage( ent, origin ) ) {
			if ( ent->dmgparent ) {
				ent = ent->dmgparent;
			}

			if ( ent->dmginloop ) {
				continue;
			}

			if ( LogAccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}
			VectorSubtract( ent->r.currentOrigin, origin, dir );
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			points = damage * ( 1.0 - dist / radius );
			G_Damage( ent, inflictor, attacker, dir, origin, (int)points, flags, mod );
		}
	}
	return hitClient;
}
/*
================
BG_ET_EvaluateTrajectory

dutch: changed the function name,
so all other entities that doesn't have a problem with
the oriignal BG_EvaluateTrajectory can still use the old one

================
*/

void BG_ET_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splinePath ) {
	float deltaTime;
	float phase;
	vec3_t v;

	switch ( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
	case TR_GRAVITY_PAUSED: //----(SA)
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
		//----(SA)	removed
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;     // FIXME: local gravity...
		break;
		// Ridah
	case TR_GRAVITY_LOW:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * ( DEFAULT_GRAVITY * 0.3 ) * deltaTime * deltaTime;     // FIXME: local gravity...
		break;
		// done.
		//----(SA)
	case TR_GRAVITY_FLOAT:
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * ( DEFAULT_GRAVITY * 0.2 ) * deltaTime;
		break;
		//----(SA)	end
		// RF, acceleration
	case TR_ACCELERATE:     // trDelta is the ultimate speed
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		// phase is the acceleration constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001 );
		// trDelta at least gives us the acceleration direction
		VectorNormalize2( tr->trDelta, result );
		// get distance travelled at current time
		VectorMA( tr->trBase, phase * 0.5 * deltaTime * deltaTime, result, result );
		break;
	case TR_DECCELERATE:    // trDelta is the starting speed
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;    // milliseconds to seconds
		// phase is the breaking constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001 );
		// trDelta at least gives us the acceleration direction
		VectorNormalize2( tr->trDelta, result );
		// get distance travelled at current time (without breaking)
		VectorMA( tr->trBase, deltaTime, tr->trDelta, v );
		// subtract breaking force
		VectorMA( v, -phase * 0.5 * deltaTime * deltaTime, result, result );
		break;
	default:
		Com_Error( ERR_DROP, "BG_ET_EvaluateTrajectory: unknown trType: %i", tr->trTime );
		break;
	}
}
// END INDOOR ARTY