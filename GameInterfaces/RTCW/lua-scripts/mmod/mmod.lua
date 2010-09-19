--dofile(et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("gamename") .. '/mmod/commands.lua')
--dofile(et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("gamename") .. '/mmod/global.lua')

dofile(et.trap_Cvar_Get("fs_homepath") .. '/omnibot8/mmod/commands.lua')
dofile(et.trap_Cvar_Get("fs_homepath") .. '/omnibot8/mmod/global.lua')

-- think if you replace gamename with fs_game it would work

function et_InitGame(levelTime, randomSeed, restart)
	loadLevels()
	loadAdmins()
	loadBans()
	
	et.G_Print("^5" .. modname .. " has been initialized...\n")
	et.G_Print("^5" .. modname .. " created by (LuNaTiC)*Marcus...\n")
	et.G_Print("^5www.thelunaticfam.com...\n")
	return
end

function et_ClientConnect(clientNum, firstTime, isBot)  -- see if client is banned
    local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid"))

	if global_ban_table[clientguid] ~= nil then
		et.trap_DropClient(clientNum, "(BANNED) " .. global_ban_table[clientguid]["reason"], bantime)
	end
	return
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
	        userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid command")
	        return
	    end
		execcommand(command, clientNum, params)
		return
	end
	
    return
end

function et_ClientCommand(clientNum)
    local argv0 = et.trap_Argv(0) -- say, vsay, vsay_team
	local text = et.ConcatArgs(1)  -- chat concatargs automatically, typing command in console does not, needed for same input
	local s,e,command,temp
	local params = {}
    local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid"))
    local clientlvl = "0"
    local clientflags = global_level_table[clientlvl]["flags"]
	
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
	        userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid command")
	        return
	    end
		if global_admin_table[clientguid] ~= nil then
		    clientlvl = tostring(global_admin_table[clientguid]["level"])
		    clientflags = global_level_table[clientlvl]["flags"]
		end
        if string.find(clientflags, commandflags[command]) == nil and clientflags ~= "*" then
            userprint(clientNum, "chat", "^3" .. command .. ": ^7insufficient admin status")
            return
        end
		execcommand(command, clientNum, params)
	else -- just some text including commandprefix
		command = nil
		temp = nil
	end
    return
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

function execcommand(command, clientNum, params)
    if command == "test" then
        test()
    elseif command == "admintest" then
        admintest(clientNum, params)
    elseif command == "ban" then
        ban(clientNum, params)
    elseif command == "botplayers" then
        botplayers(clientNum, params)
    elseif command == "date" then
        date(clientNum)
    elseif command == "help" then
        help(clientNum, params)
    elseif command == "gib" then
        gib(clientNum, params)
    elseif command == "kick" then
        kick(clientNum, params)
    elseif command == "listplayers" then
        listplayers(clientNum)
    elseif command == "nextmap" then
        nextmap(clientNum)
    elseif command == "putteam" then
        putteam(clientNum, params)
    elseif command == "rename" then
        rename(clientNum, params)
    elseif command == "setlevel" then
        setlevel(clientNum, params)
    elseif command == "showbans" then
        showbans(clientNum)
    elseif command == "slap" then
        slap(clientNum, params)
    elseif command == "spec999" then
        spec999(clientNum)
    elseif command == "stats" then
        stats(clientNum, params)
    elseif command == "swap" then
        swap(clientNum)
    elseif command == "time" then
        time(clientNum)
    elseif command == "timelimit" then
        timelimit(clientNum, params)
    elseif command == "warn" then
        warn(clientNum, params)
    else
        userprint(clientNum, "chat", "^3error: ^5" .. command .. " ^7is not a valid command!")
    end

    return
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

function userprint(clientNum, area, text)
    if clientNum == rconclient then
        et.G_Print(et.Q_CleanStr(text) .. "\n")
    else
        et.G_Print(et.Q_CleanStr(text) .. "\n")  -- questionable if im gonna leave this line in
        -- et.trap_SendServerCommand(clientNum, area .. "\"" .. text .. " \"")
        et.trap_SendServerCommand(clientNum, string.format('%s \"%s\"',area,text))
    end
end

function loadAdmins()
	local guid = ""
	local level = ""
	local name = ""
	local fd,len = et.trap_FS_FOpenFile('mmod/admins.cfg', et.FS_READ)

	if len <= 0 then
		et.G_Print("WARNING: No Admins Defined! \n")
		et.G_LogPrint("WARNING: No Admins Defined! \n")
	else
		local filestr = et.trap_FS_Read(fd, len)
		global_admin_table = {} -- reset
		
	    for name,guid,level in string.gfind(filestr, "%s*name%s*=%s*([^%\n]*)%s*guid%s*=%s*(%w+)%s*level%s*=%s*(-?%d+)%s*") do
		    global_admin_table[guid] = {}
		    global_admin_table[guid]["level"] = tonumber(level)
		    global_admin_table[guid]["name"] = name
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
		    global_level_table[level]["name"] = name
		    global_level_table[level]["flags"] = flags
		end
	end
	et.trap_FS_FCloseFile(fd) 
end

function writeAdmins()
	local fd,len = et.trap_FS_FOpenFile('mmod/admins.cfg', et.FS_WRITE)
	local guid
	local admins_string = ""

	for guid in pairs(global_admin_table) do
		admins_string = admins_string .. "name     = " .. global_admin_table[guid]["name"]     .. "\n"
		admins_string = admins_string .. "guid     = " .. guid				       .. "\n"
		admins_string = admins_string .. "level    = " .. global_admin_table[guid]["level"]    .. "\n" 
		admins_string = admins_string .. "\n" 

		et.trap_FS_Write(admins_string, string.len(admins_string) ,fd)
		admins_string = ""
	end
	et.trap_FS_FCloseFile(fd)
	loadAdmins()
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

function isNumeric(a)
    return type(tonumber(a)) == "number"
end

function loadBans()
	local guid = ""
	local reason = ""
	local name = ""
	local fd,len = et.trap_FS_FOpenFile('mmod/bans.cfg', et.FS_READ)

	if len <= 0 then
		et.G_Print("No Bans Defined! \n")
		et.G_LogPrint("No Bans Defined! \n")
	else
		local filestr = et.trap_FS_Read(fd, len)
		global_ban_table = {} -- reset
		
	    for name,guid,reason,made,expires,banner in string.gfind(filestr, "%s*name%s*=%s*([^%\n]*)%s*guid%s*=%s*(%w+)%s*reason%s*=%s*([^%\n]*)%s*made%s*=%s*([^%\n]*)%s*expires%s*=%s*(-?%d+)%s*banner%s*=%s*([^%\n]*)%s*") do
		    global_ban_table[guid] = {}
		    global_ban_table[guid]["reason"] = reason
		    global_ban_table[guid]["name"] = name
		    global_ban_table[guid]["made"] = made
		    global_ban_table[guid]["expires"] = expires
		    global_ban_table[guid]["banner"] = banner
		end
	end
	et.trap_FS_FCloseFile(fd) 
end

function writeBans()
	local fd,len = et.trap_FS_FOpenFile('mmod/bans.cfg', et.FS_WRITE)
	local guid
	local bans_string = ""

	for guid in pairs(global_ban_table) do
		bans_string = bans_string .. "name     = " .. global_ban_table[guid]["name"]     .. "\n"
		bans_string = bans_string .. "guid     = " .. guid				       .. "\n"
		bans_string = bans_string .. "reason   = " .. global_ban_table[guid]["reason"]    .. "\n" 
		bans_string = bans_string .. "made     = " .. global_ban_table[guid]["made"]    .. "\n" 
		bans_string = bans_string .. "expires  = " .. global_ban_table[guid]["expires"]    .. "\n" 
		bans_string = bans_string .. "banner   = " .. global_ban_table[guid]["banner"]    .. "\n" 
		bans_string = bans_string .. "\n" 

		et.trap_FS_Write(bans_string, string.len(bans_string) ,fd)
		bans_string = ""
	end
	et.trap_FS_FCloseFile(fd)
	loadBans()
end

function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end

