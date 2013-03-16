--dofile(et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/config.lua')
dofile(et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/commands.lua')
dofile(et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/mmod/global.lua')

function et_InitGame(levelTime, randomSeed, restart)
	local file = io.open(dbname)
	if file then
		-- true
		io.close(file)
	else
		-- false
		CreateDB()
	end

	et.RegisterModname(modname)
	et.G_Print("^5" .. modname .. " has been initialized...\n")
	et.G_Print("^5" .. modname .. " created by (LuNaTiC)*Marcus...\n")
	et.G_Print("^5" .. modname .. " url: www.thelunaticfam.com...\n")
	loadAdmins()
	loadConfig()
	loadLevels()
	loadMutes()
	return
end

function et_ClientConnect(clientNum, firstTime, isBot)  
	local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid"))
	local counter = 0
	local reason = ""
	local expires = ""

	mmodDB = sqlite3.open(dbname)
	mmodDB:exec('DELETE FROM tblBans WHERE (expires - ' .. et.GetTimeStamp() .. ') <= 0 and expires <> 0') -- delete expired bans

	for row in mmodDB:nrows('SELECT * FROM tblBans WHERE guid="' .. clientguid .. '"') do -- see if client is banned
		counter = counter + 1
		reason = row.reason
		expires = row.expires
	end

	mmodDB:close()
	mmodDB = nil	

	if counter ~= 0 then
		bmsg = "Banned - "
		bmsg = bmsg .. "Expires: " .. expiremsg(expires) .. " - "
		bmsg = bmsg .. "Reason: " .. reason
		return bmsg
	end
	return
end

function et_Obituary(victimnum, killernum, meansofdeath)
	local showkillerhp = tonumber(global_config_table["showkillerhp"])

	if showkillerhp == 1 then
		if not et.IsBot(victimnum) and killernum ~= 1022 then
			local victimteam = tonumber(et.gentity_get(victimnum, "sess.sessionTeam"))
			local killerteam = tonumber(et.gentity_get(killernum, "sess.sessionTeam"))

			if victimteam ~= killerteam then
				local killername = string.gsub(et.gentity_get(killernum, "pers.netname"), "%^$", "^^ ")
				local killerhp = et.gentity_get(killernum, "health")

				et.trap_SendServerCommand(victimnum, string.format("cp \"^7%s ^7had ^o%d ^7HP left.\" 2", killername, killerhp))
			end
		end
	end
end

function et_ConsoleCommand()  -- rcon
	local s,e,command = string.find(et.trap_Argv(0), commandprefix .. "(%w+)")
	local clientNum = rconclient
	local i = 1

	if (command ~= nil) then
		local params = {}

		-- if et.trap_Argv(i) is empty, it returns "" (and not nil!)
		while string.lower(et.trap_Argv(i)) ~= "" do
			params[i] =  string.lower(et.trap_Argv(i))
			i=i+1
		end

		command = string.lower(command)
		if commandflags[command] == nil then
			userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid " .. modname .. " command")
			return 1
		end
		execcommand(command, clientNum, params)
		return 1
	end

	return 0
end

function et_ClientCommand(clientNum)
	local argv0 = string.lower(et.trap_Argv(0)) -- say, say_team, vsay, vsay_team
	local text = et.ConcatArgs(1)  -- chat concatargs automatically, typing command in console does not, needed for same input
	local s,e,command,temp
	local params = {}
	local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid"))
	local clientlvl = "0"
	local clientflags = global_level_table[clientlvl]["flags"]

	if global_mute_table[clientguid] ~= nil and (argv0 == "say" or argv0 == "say_team" or argv0 == "vsay" or argv0 == "vsay_team") then -- is player muted
		userprint(clientNum, "cp", "^1You are muted")
		return 1
	end

	if (string.find(text, commandprefix) == 1) then -- we got a command!
		s,e,command,temp = string.find(text, commandprefix .. "(%w+)%s+(.*)")
		if (temp ~= nil) then
			-- commands with parameters such as !slap player
			params = ParseString(temp)
		else
			-- commands without parameters such as !nextmap
			s,e,command = string.find(text, commandprefix .. "([%S]*)") 
			command = command or "nil"
		end	
		command = string.lower(command)
		if commandflags[command] == nil then
			userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid " .. modname .. " command")
			return 1
		end
		if global_admin_table[clientguid] ~= nil then
			clientlvl = tostring(global_admin_table[clientguid]["level"])
			clientflags = global_level_table[clientlvl]["flags"]
		end
		if string.find(clientflags, commandflags[command]) == nil and clientflags ~= "*" then
			userprint(clientNum, "chat", "^3" .. command .. ": ^7insufficient admin status")
			return 1
		end
		-- dont print any commands to console
		if commandflags[command] ~= nil then
			-- do nothing
		elseif argv0 == "say" then
			-- SAY_ALL		0
			-- SAY_TEAM		1
			-- SAY_TELL		2
			-- SAY_LIMBO		3
			et.G_Say(clientNum, 0, text)
		elseif argv0 == "say_team" then
			et.G_Say(clientNum, 1, text)			
		end
		execcommand(command, clientNum, params)
		return 1
	else -- just some text including commandprefix
		command = nil
		temp = nil
	end
	return 0
end

function chop(text)
	-- chop(text) -- chops spaces from the end of the string

	local flag = true
	while flag do
		local s,e,c = string.find(string.sub(text,-1),"(%s)")
		if c then
			text = string.sub(text,1,-2)
		else
			flag = false
		end
	end
	return (text)
end

function CreateDB()
	mmodDB = sqlite3.open(dbname)

	mmodDB:exec[[
		CREATE TABLE tblBans (
		ind		INTEGER PRIMARY KEY,
		guid		VARCHAR(50),
		name		VARCHAR(50),
		reason		VARCHAR(50),
		expires		INTEGER,
		made		VARCHAR(50),
		banner		VARCHAR(50)
		);
	]]

	mmodDB:exec[[
		CREATE TABLE tblAdmins (
		ind		INTEGER PRIMARY KEY,
		guid		VARCHAR(50),
		level		INTEGER,
		name		VARCHAR(50)
		);
	]]

	mmodDB:exec[[
		CREATE TABLE tblMutes (
		ind		INTEGER PRIMARY KEY,
		guid		VARCHAR(50),
		name		VARCHAR(50)
		);
	]]

	mmodDB:close()
	mmodDB = nil
end

function crop_text(text,len)
	-- crop_text(text,len)
	--	recives: string and a the desired string's length
	--	returns: it crops the text from the end, until the string fits the length 
	--	note: this function ignores color-codes - so that color-codes does not effect the strings length (color codes are omitted fromm the string length)

	while string.len(et.Q_CleanStr(text)) > len do
		text = string.sub(text,1,string.len(text)-1)
	end
	return (text)
end

function execcommand(command, clientNum, params)
	if command == "admindel" then
		admindel(clientNum, params)
	elseif command == "admintest" then
		admintest(clientNum, params)
	elseif command == "announce" then
		announce(clientNum, params)
	elseif command == "ban" then
		ban(clientNum, params)
	elseif command == "botkickall" then
		botkickall()	
	elseif command == "botplayers" then
		botplayers(clientNum, params)
	elseif command == "botteam" then
		botteam(clientNum, params)
	elseif command == "date" then
		date(clientNum)
	elseif command == "difficulty" then
		difficulty(clientNum, params)
	elseif command == "help" then
		help(clientNum, params)
	elseif command == "fling" then
		fling(clientNum, params)
	elseif command == "flinga" then
		flinga(clientNum)
	elseif command == "gib" then
		gib(clientNum, params)
	elseif command == "kick" then
		kick(clientNum, params)
	elseif command == "launch" then
		launch(clientNum, params)
	elseif command == "launcha" then
		launcha(clientNum)
	elseif command == "listadmins" then
		listadmins(clientNum, params)
	elseif command == "listplayers" then
		listplayers(clientNum)
	elseif command == "lol" then
		lol(clientNum, params)
	elseif command == "map" then
		map(clientNum, params)
	elseif command == "maprecords" then
		maprecords(clientNum)
	elseif command == "mute" then
		mute(clientNum, params)
	elseif command == "nextmap" then
		nextmap(clientNum)
	elseif command == "putteam" then
		putteam(clientNum, params)
	elseif command == "rename" then
		rename(clientNum, params)
	elseif command == "reset" then
		reset(clientNum)
	elseif command == "restart" then
		restart(clientNum)
	elseif command == "serverrecords" then
		serverrecords(clientNum)
	elseif command == "setlevel" then
		setlevel(clientNum, params)
	elseif command == "showbans" then
		showbans(clientNum, params)
	elseif command == "shuffle" then
		shuffle(clientNum)
	elseif command == "slap" then
		slap(clientNum, params)
	elseif command == "spec999" then
		spec999(clientNum)
	elseif command == "stats" then
		stats(clientNum, params)
	elseif command == "swap" then
		swap(clientNum)
	elseif command == "throw" then
		throw(clientNum, params)
	elseif command == "throwa" then
		throwa(clientNum)
	elseif command == "time" then
		time(clientNum)
	elseif command == "timelimit" then
		timelimit(clientNum, params)
	elseif command == "unban" then
		unban(clientNum, params)
	elseif command == "unmute" then
		unmute(clientNum, params)
	elseif command == "warn" then
		warn(clientNum, params)
	else
		userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid command!")
	end

	return
end

function expiremsg(expires)
	local expiremsg = ""

	if expires == 0 then
		expiremsg = "Permanent"
		return expiremsg
	end
	expires = expires - et.GetTimeStamp()
	if expires >= 1 and expires < 60 then
		expiremsg = tostring(expires) .. " seconds"
	elseif expires >= 60 and expires < 3600 then
		expiremsg = tostring(round((expires / 60), 0)) .. " minutes"
	elseif expires >= 3600 and expires < 86400 then
		expiremsg = tostring(round((expires / 3600), 0)) .. " hours"
	elseif expires >= 86400 then
		expiremsg = tostring(round((expires / 86400), 0)) .. " days"
	else
		expiremsg = "Invalid"
		if expires < 0 then
			expiremsg = "Expired"
		end
	end

	return expiremsg
end

function isNumeric(a)
	return type(tonumber(a)) == "number"
end

function loadAdmins()
	global_admin_table = {} -- reset

	mmodDB = sqlite3.open(dbname)

	for row in mmodDB:nrows("SELECT * FROM tblAdmins") do
		global_admin_table[row.guid] = {}
		global_admin_table[row.guid]["level"] = tonumber(row.level)
		global_admin_table[row.guid]["name"] = row.name
	end

	mmodDB:close()
	mmodDB = nil
end

function loadConfig()
	local fd,len = et.trap_FS_FOpenFile('mmod/config.cfg', et.FS_READ)

	if len <= 0 then
		et.G_Print("WARNING: No Config Defined! \n")
		et.G_LogPrint("WARNING: No Config Defined! \n")
	else
		local filestr = et.trap_FS_Read(fd, len)

		for name,value in string.gfind(filestr, "([^%\n]*)%s*=%s*(-?%d+)") do
			global_config_table[chop(tostring(name))] = tonumber(value)
		end
	end
	et.trap_FS_FCloseFile(fd) 
end

function loadLevels()
	local name
	local flags 
	local level
	local fd,len = et.trap_FS_FOpenFile('mmod/levels.cfg', et.FS_READ)

	if len <= 0 then
		et.G_Print("WARNING: No Admins Defined! \n")
		et.G_LogPrint("WARNING: No Admins Defined! \n")
	else
		local filestr = et.trap_FS_Read(fd, len)
		global_level_table = {} -- reset

		for level,name,flags in string.gfind(filestr, "%s*level%s*=%s*(-?%d+)%s*name%s*=%s*([^%\n]*)%s*flags%s*=%s*([^%\n]*)%s*") do
			global_level_table[level] = {}
			global_level_table[level]["name"] = chop(name)
			global_level_table[level]["flags"] = chop(flags)
		end
	end
	et.trap_FS_FCloseFile(fd) 
end

function loadMutes()
	global_mute_table = {} -- reset

	mmodDB = sqlite3.open(dbname)

	for row in mmodDB:nrows("SELECT * FROM tblMutes") do
		global_mute_table[row.guid] = {}
		global_mute_table[row.guid]["name"] = row.name
	end

	mmodDB:close()
	mmodDB = nil
end

function NameToSlot(name) 
	--  returns a table containing all players found, and not nil if found more then 1
	local i = 0
	local slot = nil
	local j = 0
	local name = string.lower(et.Q_CleanStr( name ))
	local arr = {}
	local temp = ""
	for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do 
		if (et.gentity_get(i,"pers.netname") ~= nil) then
			temp = string.lower(et.Q_CleanStr( et.gentity_get(i,"pers.netname") ))
			if (temp ~= "") then
				s,e=string.find(temp, name, 1, true)
				if s and e then 
					j = j + 1
					arr[j] =  i -- j = how many we found so far, i = slot number
				end
			end 
		end 
	end 
	return(arr)  
end

function ParseString(inputString)
	inputString = inputString or ""
	local i = 1
	local t = {}
	for w in string.gfind(inputString, "([%S]+)%s*") do
		t[i]=w
		i=i+1
	end
	return t
end

function ReadyCommand(clientNum, command, params)
	local filename
	local i
	local test
	local victim
	local temp = {}
	local checknum
	local maxclients = tonumber(et.trap_Cvar_Get("sv_maxclients"))

	if table.getn(params) >= 1 then
		-- there are parameters
		checknum = tonumber(params[1])
		if checknum then
			if (checknum >= 0) and (checknum < maxclients) then 
				temp[1] = checknum
			else
				userprint(clientNum, "chat", "^3" .. command .. ": ^7Please enter a slot number between 0 and " .. tostring(maxclients - 1))
				return 99
			end
		else
			if string.len(params[1]) < 3 then
				userprint(clientNum, "chat", "^3" .. command .. ": ^7Player name requires 3 or more characters")
				return 99
			end
			temp = NameToSlot(params[1])
		end
		if table.getn(temp) < 1 then -- no matchs
			userprint(clientNum, "chat", "^3" .. command .. ": ^5" .. params[1] .. " ^7does not match a player!")
			return 99
		elseif table.getn(temp) > 1 then  -- more than one match
			userprint(clientNum, "chat", "^3" .. command .. ": ^5" .. params[1] .. " ^7matchs more than one player:")
			for i=1,table.getn(temp),1 do -- list all matching clients
				local slot = temp[i]
				if slot < 10 then
					slot = " " .. slot
				end
				userprint(clientNum, "chat", "^7" .. slot .. " - " ..  et.gentity_get(temp[i],"pers.netname"))
			end
			return 99
		else -- one match
			victim = temp[1]
			local victimguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid"))
			local victimlevel = 0

			if global_admin_table[victimguid] ~= nil then
				victimlevel = global_admin_table[victimguid]["level"]
			end
			if clientNum ~= rconclient then
				local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid"))
				local clientlevel = 0

				if global_admin_table[clientguid] ~= nil then
					clientlevel = global_admin_table[clientguid]["level"]
				end
				if victimlevel > clientlevel then 
					userprint(clientNum, "chat", "^3" .. command .. ": ^7you can not target higher level")
					return 99
				end            
			end
			if et.gentity_get(victim,"pers.netname") == "" then
				userprint(clientNum, "chat", "^3" .. command .. ": ^7No players on slot ^5" .. params[1])
				return 99
			end
		end
	else
		-- there are no parameters
		victim = -1
	end

	return victim
end 

function round(num, idp)
	local mult = 10^(idp or 0)
	return math.floor(num * mult + 0.5) / mult
end

function userprint(clientNum, area, text)
	if clientNum == rconclient then
		et.G_Print(et.Q_CleanStr(text) .. "\n")
	else
		-- et.G_Print(et.Q_CleanStr(text) .. "\n")  -- questionable if im gonna leave this line in
		-- et.trap_SendServerCommand(clientNum, area .. "\"" .. text .. " \"")
		-- cs: lets give centprints a high priority
		if area == "cp" then
			et.trap_SendServerCommand(clientNum, string.format('%s \"%s\" 5',area,text))
		else
			et.trap_SendServerCommand(clientNum, string.format('%s \"%s\"',area,text))
		end
	end
end

------ map records stuff
function OpenDB()
	local dbExists = io.open(RecordsDBName)
	if not dbExists then
		CreateRecordsDB()
	else
		io.close(dbExists)
	end

	RecordsDB = sqlite3.open(RecordsDBName)
end

function CloseDB()
	RecordsDB:close()
end

function updateCurrentRecords(ind, value)
	currentMapRecords[ ind ] = value
end

function CreateRecordsDB()
	RecordsDB = sqlite3.open(RecordsDBName)
	RecordsDB:exec[[
		CREATE TABLE Maps (
		ind		INTEGER PRIMARY KEY,
		name		VARCHAR(50)
		);

		CREATE TABLE RecordTypes (
		ind		INTEGER PRIMARY KEY,
		name		VARCHAR(50)
		);

		CREATE TABLE Players (
		ind		INTEGER PRIMARY KEY,
		name		VARCHAR(50)
		);

		CREATE TABLE Records (
		ind		INTEGER PRIMARY KEY,
		recordTypeId	INTEGER,
		playerId	INTEGER,
		mapId		INTEGER,
		value		INTEGER,
		recordDate	VARCHAR(50)
		);
	]]

	for ind, typeName in ipairs(recordTypes) do
		RecordsDB:exec('INSERT INTO RecordTypes(name) VALUES("' .. typeName .. '")')
	end
	RecordsDB:close()
end

function getMapId(mapName)
	for row in RecordsDB:nrows('SELECT * FROM Maps WHERE name="' .. mapName .. '"') do
		return row.ind
	end

	-- insert then recursively call to get the id if it doesn't exist
	RecordsDB:exec('INSERT INTO Maps(name) VALUES("' .. mapName .. '")')
	local ind = getMapId(mapName)
	return ind
end

function getMapName(mapId)
	for row in RecordsDB:nrows('SELECT * FROM Maps WHERE ind="' .. mapId .. '"') do
		return row.name
	end

	return "unknown"
end

function getPlayerId(playerName)
	for row in RecordsDB:nrows('SELECT * FROM Players WHERE name="' .. playerName .. '"') do
		return row.ind
	end

	-- insert then recursively call to get the id if it doesn't exist
	RecordsDB:exec('INSERT INTO Players(name) VALUES("' .. playerName .. '")')
	local ind = getPlayerId(playerName)
	return ind
end

function getPlayerName(playerId)
	for row in RecordsDB:nrows('SELECT * FROM Players WHERE ind="' .. playerId .. '"') do
		return row.name
	end

	return "unknown"
end

function getCurrentMapRecords(mapId)
	local ind = 1
	for row in RecordsDB:nrows('SELECT * FROM Records WHERE mapId=' .. mapId) do
		currentMapRecords[ ind ] = row.value
		ind = ind + 1
		if ind > 9 then
			break
		end
	end
end

function insertNewMapRecord( mapId, playerName, recordType, value )
	playerId = getPlayerId(playerName)

	-- update the record if it exists
	for row in RecordsDB:nrows('SELECT * FROM Records WHERE mapId=' .. mapId .. ' AND recordTypeId=' .. recordType) do
		RecordsDB:exec('UPDATE Records SET playerId=' .. playerId .. ', value=' .. value .. ', recordDate="' .. os.date("%x") .. '" WHERE mapId=' .. mapId .. ' AND recordTypeId=' .. recordType)
		return
	end

	-- new record
	RecordsDB:exec('INSERT INTO Records(recordTypeId,playerId,mapId,value,recordDate) VALUES(' .. recordType ..
		', '.. playerId ..
		', '.. mapId ..
		', '.. value ..
		', "'.. os.date("%x") .. '")' )
end

function checkForMapRecords()
	mapId = getMapId(et.GetMapName())
	getCurrentMapRecords(mapId)

	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		playerName = et.gentity_get(i,"pers.netname")
		if playerName ~= "" and et.gentity_get(i,"pers.connected") == 2 then
			for ind, val in ipairs(currentMapRecords) do
				testVal = et.gentity_get(i,recordTypeMap[ind])
				if testVal  > val then
					updateCurrentRecords(ind, testVal)
					insertNewMapRecord(mapId, playerName, ind, testVal)
				end
			end
		end
	end
end

function et_LogExit()
	-- open up the database
	OpenDB()

	checkForMapRecords()

	-- close the database
	CloseDB()
end