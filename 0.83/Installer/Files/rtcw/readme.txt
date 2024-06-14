==Overview==
RTCW:Omnibot is a modification for https://en.wikipedia.org/wiki/Return_to_Castle_Wolfenstein. Some general modifications have been made to the 1.4 SDK. 

===Bot Features===
* 99 fully supported maps (complete waypoints / scripts)
* Complete Class and Weapon support
* Riding movers (trams, trains, etc)
* Targeting of breakables
* Customizable difficulty settings
** Aim skill
** Combat behaviors
* Elevator usage
* Switch usage
* Routing
* Class Management
* Role Management
* Complete Goal Support
** Artillery
** Dynamite
*** Plant
*** Defuse
** Steal
** Return
** Checkpoint
** Attack
** Defend
** Roam
** Panzerfaust camps 
** Snipe camps
** Flamethrower camps
** Venom camps
** Revive Teammate
* Scripted Goal Support
** Airstrike
** Call for supplies ( medic packs and ammo ) 
** Deliver supplies
** Dispense ammo
** Grenade target 
** Supply self ( for medics and lieutenants )

===General Mod Features===
* Hitsounds
* Colored Crosshairs
* Local time in hud
* Crosshair Pulse
* ET / OSP style Table Stats
* Shrub like /stats command
* Private Messages
* Fixed Physics
* Helmet Protection
* Drop Items
* Throwable Knives
* Instagib game mode
* ET / OSP colors
* Weapon Flash
* Blood Flash
* Objective Info Prints
* Warmup Damage
* Water Revive
* NQ / ETPub Killing Sprees
* ETPro Antiwarp
* End of round awards
* Banner Messages
* Goombas
* Shoving
* Corpse Dragging
* Tap out reports
* Revive reports
* Gib reports
* Right aligned overlays for fps, kills, spree, speedometer, and ping
* Extra clips
* OSP's goat sound for knife kills
* Reinforcement time offsets
* Poison needles
* Betting and Buying system
* Deathmatch support
* Kickable Airstrikes and Grenades
* Binoculars for all classes
* Unlockable weapons
* Flag locking
* Grenade Packs
* Smoke Grenades
* LUA support


==Installation==
The mod is installed in two locations; an omnibot folder for the game folder and an omni-bot folder which contains the bot dll as well as waypoints and scripts.

===Windows===
Unzip the archive to your Return to Castle Wolfenstein folder.

===Linux===
It is recommended that the zip be copied to the games root folder as a user with root priveledges (default is /usr/local/games/wolfenstein). Extract the zip to this location. Once extracted, the omnibot_path must be set in either the server config or wolfconfig_mp.cfg. It must be set to the exact path of the omni-bot folder. Alternatively, you can start the game and set the value in the console; /seta omnibot_path /usr/local/games/wolfenstein/omi-bot for example. If set with the console, a map_restart is required.

===LUA===
Copy the lua library from the with_lua folder to the RTCW root directory that contains the game executable ( wolfmp.exe / wolfmp ). Copy the qagame library from the with_lua folder to the omnibot mod folder (replacing the existing qagame). Note that for linux, the file extension is .so while in Windows it is .dll.

==Starting Omnibot==
There are two ways to start the mod. One way is to start Wolfmp.exe, then select options / mods / omnibot. The other way is to create a shortcut. To do this, copy your Wolfmp.exe to the desktop, then right click it. Select properties, then add +set fs_game omnibot to the target line:

 "C:\games\Return to Castle Wolfenstein\WolfMP.exe" +set fs_game omnibot


==Configuration==
===Difficulty Settings===
Aim and Combat movement difficulty can be set with bot commands.
* /bot difficulty - from 0 to 5, default 3. 0 is easiest, 5 is hardest
* /bot moveskill - from 0 to 4, default 3. 0 = off, 1 = easy, 2 = medium, 3 = hard, 4 = random per bot

===CVARS===
====Server Vars====
=====Extra Clips=====
Add extra clips to players when they spawn
default 0, x : x = number of extra clips
*g_engExtraClips - max 9
*g_ltExtraClips - max 9
*g_mauserExtraClips - max 3
*g_medExtraClips - max 9
*g_panzerExtraClips - max 3
*g_pistolExtraClips - max 3
*g_soldExtraClips - max 9
*g_venomExtraClips - max 3

=====g_allowGiveAmmo=====
Enables the usage of the /giveammo # command
default 0, 1 to enable

=====g_announcer=====
NQ / ETPub Killing Sprees
default 0, bit flagged cvar:
*1 - killing spree
*2 - death spree
*4 - multi-kill
g_announcer 7 enables all messages / sounds 

=====g_antiwarp=====
zinx ETPro antiwarp
default 0,  1 to enable

=====g_awards=====
Display end of round awards'
default 0,  1 to enable

=====g_betting=====
Allow players to bet against each other
default 0, bit flagged cvar:
*1 - enable betting
*2 - enable buying

=====g_botTeam=====
Limit bots to a specific team
default 0,  1 = axis, 2 = allies

=====g_binocs=====
Supply classes other than lieutenants with binoculars
default 0,  bit flagged cvar:
*1 - soldiers
*2 - engineers
*4 - medics

=====g_botGib=====
Allow bots to target injured players
default 1, 0 to disable bot gibbing

=====g_deathmatch=====
Enable team deathmatch mode
default 0, 1 to enable. requires map_restart

=====g_debugPlayerHitboxes=====
Cheat Protected, renders client hitboxes
default 0,  1 to enable

=====g_dragCorpse=====
Enable dragging of corpse using +activate
default 0,  1 to enable

=====g_dropItems=====
When enabled, Medics and Lt's will drop packs when gibbed
default 0, 1 to enable 

=====g_fixedPhysics=====
Simulate fps movement advantages for 125 or 333 fps
default 0, 1 = 125 fps, 2 = 333 fps

=====g_flagLock=====
Checkpoints cannot be re-captured
default 0, 1 = disable recapturing of checkpoints

=====g_goomba=====
Enable Shrub like goomba kills
default 0,  1 to enable

=====g_goombaDmg=====
Optional override of default goomba damage
default 0, #: g_goombaDmg 999 for instagib at all fall distances

=====g_helmetProtection=====
If a player has a helmet, headshot damage is reduced to 20. Ammo replaces a missing helmet.
default 0, 1 to enable

=====g_httpdownhost=====
The http server for windows clients to download maps from
default www.s4ndmod.com/downloads

=====g_instaGib=====
An alternate game mode where all players use an unscoped mauser. Headshots, thrown knives, and some explosions instantly gib
default 0, 1 to enable

=====g_kickItems=====
Allow kicking of Airstrike cannisters and / or grenades
default 0,  bit flagged cvar:
*1 - grenades
*2 - airstrike cannisters

=====g_knifeDamage=====
The amount of damage a thrown knife deals
default 35

=====g_LTNades=====
The amount of grenades a Lieutenant spawns with
default 1

=====g_LTInfoMsg=====
Display the amount of ammo a teammate currently has
default 0, 1 to enable.

=====g_mapconfigs=====
The directory in which to load map specific configs
default ""
example: g_mapconfigs "mapconfigs" will search for omnibot/mapconfigs
note: a default.cfg in the map config directory is loaded in cases where a map specific config does not exist

=====g_maxKnives=====
The amount of throwable knives a player can have
default 5

=====g_MedNades=====
The amount of grenades a Medic spawns with
default 1

=====g_msg1 - 10=====
Banner message strings
messages for the banner as strings. g_msg5 = "This is my message" for example

=====g_msgPos=====
Banner message position
default 0,  0 = console, 1 = chat, 2 = center print

=====g_msgs=====
Enable Banner Messages
default 0,  1 to enable

=====g_multikilltime=====
Time in milliseconds for multikill registration
default 1000

=====g_mutespecs=====
Disable global voice chat for spectators
default 0,  1 to mute spectators

=====g_nadepacks=====
Allow lieutenants to supply grenade packs
default 0,  1 to enable

=====g_poison=====
Medics can poison enemy players
default 0,  1 to enable

=====g_poisonDmg=====
The amount of damage each tick of poison deals
default 10

=====g_privateMessages=====
Allow player to player private communication
default 0, 1 to enable

=====g_reportGibs=====
Enable gib reporting
default 0,  1 to enable

=====g_reportRevives=====
Enable revive reporting
default 0,  1 to enable

=====g_reportTaps=====
Enable Tap out reporting
default 0,  1 to enable

=====g_shortcuts=====
Enables usage of shortcuts in chat messages
default 0, 1 to enable

=====g_shove=====
Enable Shrub like shoving with +activate'
default 0,  1+ to enable where 1+ is the distance to shove.
example: seta g_shove 60

=====g_shoveNoZ=====
Disable shoving along Z axis (prevents wall boosting)
default 1,  0 to disable

=====g_smokeGrenades=====
Allow lieutenants to throw smoke grenades
default 0,  1 to enable

=====g_throwableKnives=====
The amount of throwable knives a player starts with
default 0, any number greater than 0 enables usage. -1 grants infinite throwable knives

=====g_unlockWeapons=====
Unlock all weapons for all classes
default 0, 1 to unlock weapons

=====g_warmupDamage=====
Allow damage to be given in warmup
default 0, 1 to enable

=====g_waterRevive=====
Allow players to be revived under water
default 0, 1 to enable

====Client Vars====
=====cg_announcer=====
Enable the OSP style announcer sounds
default 0, 1 to enable
* prepare to fight
* fight
* axis win
* allies win

=====cg_bloodDamageBlend=====
Disable the blood splotches when taking damage
default 0, 1 to remove the blood splotches

=====cg_bloodFlash=====
Disable the full screen flash when taking damage
default 0, 1 to remove the flashing

=====cg_crosshairColor=====
Adjust color of crosshair as string or hex
Default white

Strings for colors:
* white
* red
* green
* blue
* yellow
* magenta
* cyan
* orange
* mdred
* mdgreen
* dkgreen
* mdcyan
* mdyellow
* mdorange
* mdblue
* ltgrey
* mdgrey
* dkgrey
* black

note: this can also be changed via the in game menu

=====cg_crosshairPulse=====
Toggle the pulsing of crosshair
default 1, 0 = disabled, 1 = enabled

=====cg_drawCrosshairNames=====
Toggle the different modes for display of crosshair names
default 1, 0 = disabled, 1 = white names, 2 = colored names

=====cg_drawKillCount=====
Enable drawing of kills or current spree in right aligned overlay - '''version 2 only'''
default 0,  1 = kills, 2 = current spree

=====cg_drawLocalTime=====
Toggle the drawing of local time in the hud
default 0, 0 = disabled, 1 = enabled

=====cg_drawPing=====
Enable drawing of ping in right aligned overlay - '''version 2 only'''
default 0,  1 to enable

=====cg_drawSpeedometer=====
Enable drawing of speedometer in right aligned overlay - '''version 2 only'''
default 0,  1 = mph, 2 = kph, 3 = ups

=====cg_drawStopwatchSprite=====
Enable drawing of the stopwatch sprite - '''version 2 only'''
default 1,  0 to disable

=====cg_drawWeaponIconFlash=====
Enable flashing of weapon icon on certain events ( firing, etc )
default 0, 1 to enable

=====cg_goat=====
Enable OSP goat sound for knife kill - '''version 2 only'''
default 0,  1 to enable

=====cg_hitsounds=====
Enable playing of sounds when shots hit a player
default 0, 1 to enable

=====cg_muzzleFlash=====
Disable the flash when firing a weapon
default 1, 0 to disable the flashing

=====cg_ospFonts=====
Enable usage of OSP fonts in hud. Requires vid_restart
default 0, 1 to enable

=====cg_printObjectiveInfo=====
Enable info prints when objectives are completed
default 0, 1 to enable

====Client Commands====
=====bet=====
Bet against an opponent that you will kill them before they kill you
 /bet <partial name> <num credits>

=====buy=====
Buy some items with your earned credits. Players earn credits by killing, completing objectives, and betting other players
 /buy <ammo | health> 
example: buy health

=====mycredits=====
Displays in console how many credits you have accumulated
 /mycredits

=====injure=====
An alternative to /kill that does not send the player to limbo. Useful in friendly fire off games to benefit from a quick revive
 /injure

=====pm=====
Send a private message to one or more clients
 /pm <full or partial name> <message>

=====stats=====
Print current round stats to console
 /stats

=====resettimer=====
 /resettimer - restarts the spawn timer countdown. use this command when you notice an enemy has just respawned 

=====timerset=====
 /timerset <seconds> - the spawn time in seconds for the enemy team

using this command will render enemy spawn time in cyan next to your own spawn time. note that you must use the command /resettimer in order for it to be accurate.


==Credits==
* crapshoot - integrated omni-bot support, added mod features, waypointing, and scripting
* DrEvil - creator of the omni-bot framework
* palota - updated omni-bot for RTCW from version 0.82 to version 0.85
* Jaskot / GeekFestStarter - omni-bot team member
* AG3NT - provided some mod code, beta testing, waypoints / scripts
* 420Blunt / FireItUp - beta testing, waypoints / scripts
* S4NDM4NN - provided some mod code and bug fixes
* Guerna - dedicated server beta testing
* TeamWhore / Old Lady Giblet - ubersoldat mod developer, provided some mod code
* tjw - private messaging code from etpub mod
* Denny - Fritzbot team member who graciously gave permission to convert his waypoints to omni-bot
* The BindleStiff - Fritzbot team member who graciously gave permission to convert his waypoints to omni-bot
* OSP - Announcer sounds, goat sound, fonts and some mod code
* (LuNaTiC)Marcus - beta testing, feature requests
* NoQuarter - Killing spree code, debug player hitbox code
* Chruker - some bug fix code from project bug fix
* etpub - LUA api code, fling code, spawn timer code
* splashdamage - antilag code in the et sdk
* |ToaK|JD - feature requests, bug reports, beta testing, hosting of several omnibot rtcw servers
* n00b-l00ps - mp_depot waypoint improvements
* madaaron - feature requests
