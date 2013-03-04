commandprefix = "!"
rconclient = "console"
modname = "mmod"
dbname = et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/mmod.db'
RecordsDBName = et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/maprecords.db'

-- some constants
TEAMS = {
	"axis",
	"allies",
}

MAX_DIFFICULTY = 7

global_admin_table = {}
global_config_table = {}
global_level_table = {}
global_mute_table = {}

-- command flags are the same as etpub shrubbot flags
commandflags = {}
commandflags["admindel"] = "Q"
commandflags["admintest"] = "a"
commandflags["announce"] = "f"
commandflags["ban"] = "b"
commandflags["botkickall"] = "o"
commandflags["botplayers"] = "o"
commandflags["botteam"] = "o"
commandflags["date"] = "C"
commandflags["difficulty"] = "o"
commandflags["gib"] = "g"
commandflags["fling"] = "l"
commandflags["flinga"] = "L"
commandflags["help"] = "h"
commandflags["kick"] = "k"
commandflags["launch"] = "l"
commandflags["launcha"] = "L"
commandflags["listadmins"] = "q"
commandflags["listplayers"] = "i"
commandflags["lol"] = "x"
commandflags["map"] = "M"
commandflags["maprecords"] = "t"
commandflags["mute"] = "m"
commandflags["nextmap"] = "n"
commandflags["putteam"] = "p"
commandflags["rename"] = "N"
commandflags["reset"] = "r"
commandflags["restart"] = "r"
commandflags["serverrecords"] = "t"
commandflags["setlevel"] = "s"
commandflags["showbans"] = "B"
commandflags["shuffle"] = "S"
commandflags["slap"] = "A"
commandflags["spec999"] = "P"
commandflags["stats"] = "t"
commandflags["swap"] = "w"
commandflags["throw"] = "l"
commandflags["throwa"] = "L"
commandflags["time"] = "C"
commandflags["timelimit"] = "T"
commandflags["unban"] = "b"
commandflags["unmute"] = "m"
commandflags["warn"] = "R"

commandhelp = {}
commandhelp["admindel"] = "^3usage: ^7!admindel [admin number] \n^3function: ^7removes the admin that !listadmins lists as admin number"
commandhelp["admintest"] = "^3usage: ^7!admintest or !admintest [name|slot] \n^3function: ^7prints out player's current mmod admin level in global chat. When the NAME or SLOT are added and the user has the right mmod permission, the level of another player will be shown"
commandhelp["announce"] = "^3usage: ^7!announce [msg] \n^3function: ^7announces message in centerprint"
commandhelp["ban"] = "^3usage: ^7!ban [name|slot] [reason] \n^3function: ^7bans the player and gives them a reason"
commandhelp["botplayers"] = "^3usage: ^7!botplayers [number of bots] \n^3function: ^7changes value of omnibots maxbots"
commandhelp["botteam"] = "^3usage: ^7!botteam [0|1|2] \n^3function: ^7enables or disables bots vs humans 0 disables, 1 = bots on axis, 2 = bots on allies"
commandhelp["date"] = "^3usage: ^7!date \n^3function: ^7prints out local date of the server"
commandhelp["difficulty"] = "^3usage: ^7!difficulty [-1|" .. MAX_DIFFICULTY .. "] \n^3function: ^7changes the bot difficulty"
commandhelp["fling"] = "^3usage: ^7!fling [name|slot] \n^3function: ^7send the player flying."
commandhelp["flinga"] = "^3usage: ^7!flinga \n^3function: ^7sends everyone flying."
commandhelp["gib"] = "^3usage: ^7!gib [name|slot] [reason] \n^3function: ^7kills and gibs the player"
commandhelp["help"] = "^3usage: ^7!help or !help [command] \n^3function: ^7list commands or show help for a command"
commandhelp["kick"] = "^3usage: ^7!kick [name|slot] [reason] \n^3function: ^7kicks the player and gives them a reason"
commandhelp["launch"] = "^3usage: ^7!launch [name|slot] \n^3function: ^7send the player flying."
commandhelp["launcha"] = "^3usage: ^7!launcha \n^3function: ^7sends everyone flying."
commandhelp["listadmins"] = "^3usage: ^7!listadmins or !listadmins [skip] \n^3function: ^7display a list of admins"
commandhelp["listplayers"] = "^3usage: ^7!listplayers \n^3function: ^7display a list of players"
commandhelp["lol"] = "^3usage: ^7!lol [name|slot] \n^3function: ^7make player drop grenades."
commandhelp["map"] = "^3usage: ^7!map [map name] \n^3function: ^7change map to [map name]"
commandhelp["maprecords"] = "^3usage: ^7!maprecords \n^3function: ^7display a list of records for current map"
commandhelp["mute"] = "^3usage: ^7!mute [name|slot] [reason] \n^3function: ^7mute the player and gives them a reason"
commandhelp["nextmap"] = "^3usage: ^7!nextmap \n^3function: ^7loads next map in rotation"
commandhelp["putteam"] = "^3usage: ^7!putteam [name|slot] [b|r|s] \n^3function: ^7move player to another team"
commandhelp["rename"] = "^3usage: ^7!rename [name|slot] [new name] \n^3function: ^7renames a player to a new name"
commandhelp["reset"] = "^3usage: ^7!reset \n^3function: ^7reset the match"
commandhelp["restart"] = "^3usage: ^7!restart \n^3function: ^7restart the map"
commandhelp["serverrecords"] = "^3usage: ^7!serverrecords \n^3function: ^7display a list of records for all maps on server"
commandhelp["setlevel"] = "^3usage: ^7!setlevel [name|slot] [level] \n^3function: ^7sets player with admin level"
commandhelp["showbans"] = "^3usage: ^7!showbans or !showbans [skip]\n^3function: ^7show a list of banned players"
commandhelp["shuffle"] = "^3usage: ^7!shuffle\n^3function: ^7shuffles teams"
commandhelp["slap"] = "^3usage: ^7!slap [name|slot] [reason] \n^3function: ^7slaps player and gives them hp damage"
commandhelp["spec999"] = "^3usage: ^7!spec999 \n^3function: ^7moves all players with ping of 999 to the spectators team"
commandhelp["stats"] = "^3usage: ^7!stats or !stats [name|slot]\n^3function: ^7player stats"
commandhelp["swap"] = "^3usage: ^7!swap \n^3function: ^7swaps the players to the opposite team"
commandhelp["throw"] = "^3usage: ^7!throw [name|slot] \n^3function: ^7send the player flying."
commandhelp["throwa"] = "^3usage: ^7!throwa \n^3function: ^7sends everyone flying."
commandhelp["time"] = "^3usage: ^7!time \n^3function: ^7prints out local time of the server"
commandhelp["timelimit"] = "^3usage: ^7!timelimit [minutes] \n^3function: ^7change timelimit of current map"
commandhelp["unban"] = "^3usage: ^7!unban [ban number] \n^3function: ^7removes the ban that !showbans lists as ban number"
commandhelp["unmute"] = "^3usage: ^7!unmute [name|slot] \n^3function: ^7unmute the player"
commandhelp["warn"] = "^3usage: ^7!warn [name|slot] [reason] \n^3function: ^7gives a player a warning"

------ map records stuff
recordTypes = {
	"Kills",
	"Spree",
	"Damage",
	"HeadShots",
	"Poisons",
	"Goombas",
	"Revives",
	"MedPacks",
	"AmmoPacks"
}

recordTypeMap = {
	"pers.kills",
	"pers.killSpreeMax",
	"pers.damage_given",
	"pers.headshots",
	"pers.poison",
	"pers.goomba",
	"pers.revives",
	"pers.medPacks",
	"pers.ammoPacks"
}

-- note: length must match the arrays above
currentMapRecords = { 0,0,0,0,0,0,0,0,0 }


