commandprefix = "!"
rconclient = "console"
modname = "mmod"
dbname = et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/mmod.db'
RecordsDBName = et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/maprecords.db'

global_admin_table = {}
global_config_table = {}
global_level_table = {}
global_mute_table = {}

-- command flags are the same as etpub shrubbot flags
commandflags = {}
commandflags["admindel"] = "Q"
commandflags["admintest"] = "a"
commandflags["ban"] = "b"
commandflags["botplayers"] = "o"
commandflags["date"] = "C"
commandflags["gib"] = "g"
commandflags["help"] = "h"
commandflags["kick"] = "k"
commandflags["listadmins"] = "q"
commandflags["listplayers"] = "i"
commandflags["maprecords"] = "t"
commandflags["mute"] = "m"
commandflags["nextmap"] = "n"
commandflags["putteam"] = "p"
commandflags["rename"] = "N"
commandflags["serverrecords"] = "t"
commandflags["setlevel"] = "s"
commandflags["showbans"] = "B"
commandflags["slap"] = "A"
commandflags["spec999"] = "P"
commandflags["stats"] = "t"
commandflags["swap"] = "w"
commandflags["time"] = "C"
commandflags["timelimit"] = "T"
commandflags["unban"] = "b"
commandflags["unmute"] = "m"
commandflags["warn"] = "R"

commandhelp = {}
commandhelp["admindel"] = "^3usage: ^7!admindel [admin number] \n^3function: ^7removes the admin that !listadmins lists as admin number"
commandhelp["admintest"] = "^3usage: ^7!admintest or !admintest [name|slot] \n^3function: ^7prints out player's current mmod admin level in global chat. When the NAME or SLOT are added and the user has the right mmod permission, the level of another player will be shown"
commandhelp["ban"] = "^3usage: ^7!ban [name|slot] [reason] \n^3function: ^7bans the player and gives them a reason"
commandhelp["botplayers"] = "^3usage: ^7!botplayers [number of bots] \n^3function: ^7changes value of omnibots maxbots"
commandhelp["date"] = "^3usage: ^7!date \n^3function: ^7prints out local date of the server"
commandhelp["gib"] = "^3usage: ^7!gib [name|slot] [reason] \n^3function: ^7kills and gibs the player"
commandhelp["help"] = "^3usage: ^7!help or !help [command] \n^3function: ^7list commands or show help for a command"
commandhelp["kick"] = "^3usage: ^7!kick [name|slot] [reason] \n^3function: ^7kicks the player and gives them a reason"
commandhelp["listadmins"] = "^3usage: ^7!listadmins or !listadmins [skip] \n^3function: ^7display a list of admins"
commandhelp["listplayers"] = "^3usage: ^7!listplayers \n^3function: ^7display a list of players"
commandhelp["maprecords"] = "^3usage: ^7!maprecords \n^3function: ^7display a list of records for current map"
commandhelp["mute"] = "^3usage: ^7!mute [name|slot] [reason] \n^3function: ^7mute the player and gives them a reason"
commandhelp["nextmap"] = "^3usage: ^7!nextmap \n^3function: ^7loads next map in rotation"
commandhelp["putteam"] = "^3usage: ^7!putteam [name|slot] [b|r|s] \n^3function: ^7move player to another team"
commandhelp["rename"] = "^3usage: ^7!rename [name|slot] [new name] \n^3function: ^7renames a player to a new name"
commandhelp["serverrecords"] = "^3usage: ^7!serverrecords \n^3function: ^7display a list of records for all maps on server"
commandhelp["setlevel"] = "^3usage: ^7!setlevel [name|slot] [level] \n^3function: ^7sets player with admin level"
commandhelp["showbans"] = "^3usage: ^7!showbans or !showbans [skip]\n^3function: ^7show a list of banned players"
commandhelp["slap"] = "^3usage: ^7!slap [name|slot] [reason] \n^3function: ^7slaps player and gives them hp damage"
commandhelp["spec999"] = "^3usage: ^7!spec999 \n^3function: ^7moves all players with ping of 999 to the spectators team"
commandhelp["stats"] = "^3usage: ^7!stats or !stats [name|slot]\n^3function: ^7player stats"
commandhelp["swap"] = "^3usage: ^7!swap \n^3function: ^7swaps the players to the opposite team"
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
