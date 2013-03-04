function admindel(clientNum, params)
    local header = "^3admindel: ^7"
	local adminnum = params[1]
	
    if adminnum ~= nil then
		if isNumeric(adminnum) then
			adminnum = tonumber(adminnum)
			local counter = 0
			local name = ""
			local level = 0
			
			mmodDB = sqlite3.open(dbname)
			for row in mmodDB:nrows('SELECT * FROM tblAdmins WHERE ind=' .. adminnum) do
				counter = counter + 1
				name = row.name
				level = row.level
			end
			if clientNum ~= rconclient then
				local clientguid = string.lower(tostring(et.Info_ValueForKey( et.trap_GetUserinfo(clientNum), "cl_guid" )))			
				if level > global_admin_table[clientguid]["level"] then
					userprint(clientNum, "chat", "^3admindel: ^7can not delete admin with higher level than yours")
					mmodDB:close()
					mmodDB = nil
					return
				end
			end
			
			if counter ~= 0 then
				mmodDB:exec('DELETE FROM tblAdmins WHERE ind=' .. adminnum)
				userprint(-1, "chat", header .. name .. " ^7is no longer an admin")
			else
				userprint(clientNum, "chat", "^3admindel: ^5" .. adminnum .. " ^7is an invalid admin number")			
			end
			mmodDB:close()
			mmodDB = nil	
			loadAdmins()
		else
			userprint(clientNum, "chat", "^3admindel: ^5" .. adminnum .. " ^7is not a number")
			return
		end
    else
        userprint(clientNum, "chat", commandhelp["admindel"])
    end
end

function admintest(clientNum, params)
    local header = "^3admintest: ^7"
    local victim = ReadyCommand(clientNum, "admintest", params)
    
    if victim == 99 then
        return
    end
    
    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
        local level = "0"
        
		if (global_admin_table[guid] ~= nil) then
	        level = tostring(global_admin_table[guid]["level"])
        end
		levelname = chop(global_level_table[level]["name"])
		userprint(-1, "chat", header .. name .. " ^7is level " .. level .. " (" .. levelname .. ")")
    else
        if clientNum ~= rconclient then
            local name = et.gentity_get(clientNum,"pers.netname")
		    local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid" ))
            local level = "0"
            
		    if (global_admin_table[guid] ~= nil) then
	            level = tostring(global_admin_table[guid]["level"])
            end
		    levelname = chop(global_level_table[level]["name"])
		    userprint(-1, "chat", header .. name .. " ^7is level " .. level .. " (" .. levelname .. ")")
		end
    end
    return
end

function announce(clientNum, msg)
	local header = "^3announce: ^7"
	
	if msg[1] == nil then
		userprint(clientNum, "chat", commandhelp["announce"])
		return
	end
	
	local msgString = ""
	for index, val in pairs(msg) do
		if index > 0 then
			msgString = msgString .. " " .. val
		end
	end

	userprint(-1, "cp", msgString)
	local tmpMsg = header .. "^7announcement was center printed!"
	userprint(-1, "chat", tmpMsg)
end

function ban(clientNum, params)
    local header = "^3ban: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "ban", params)
	local rstart = 2 -- reason start position
	local s,e,tnum,tmod
	local tban = 0
    
    if victim == 99 then
        return
    end
	
	if params[2] ~= nil then
		s,e,tnum,tmod = string.find(params[2], "(%d+)(%a)")
		if s ~= nil then
			tmod = string.lower(tmod)
			if tmod == "s" then
				tban = tnum + et.GetTimeStamp()
			elseif tmod == "m" then
				tban = (tnum * 60) + et.GetTimeStamp()
			elseif tmod == "h" then
				tban = (tnum * 3600) + et.GetTimeStamp()
			elseif tmod == "d" then
				tban = (tnum * 86400) + et.GetTimeStamp()
			elseif tmod == "w" then
				tban = (tnum * 604800) + et.GetTimeStamp()
			else
				userprint(clientNum, "chat", header .. tnum .. tmod .. " ^7is not a valid time modifier!")
				return
			end
			rstart = 3
		else
			rstart = 2
		end
	end
 
    if table.getn(params) >= rstart then
        for i = rstart, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end
    
    if (reason == "") then
	    reason = "banned by admin"
    end
		
    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
		local made = os.date("%c")
		local expires = tban
		local banner = ""
		
		if et.IsBot(victim) then
			userprint(clientNum, "chat", header .. "cannot ban bots")
			return 
		end
		if clientNum==rconclient then
			banner = rconclient
		else
			banner = et.gentity_get(clientNum,"pers.netname")
		end
		mmodDB = sqlite3.open(dbname)
		mmodDB:exec('insert into tblBans (guid,name,reason,expires,made,banner) values("'.. guid ..'", "'.. name ..'", "'.. reason ..'", '.. expires ..', "'.. made ..'", "'.. banner .. '")' )
		mmodDB:close()
		mmodDB = nil	
		bmsg = "\n\nYou have been banned: \n"
		bmsg = bmsg .. "Reason: " .. reason .. "\n"
		bmsg = bmsg .. "Expires: " .. expiremsg(expires)
        et.trap_DropClient(victim, bmsg, 0)
        reason = " ^3reason: ^7" .. reason
        local tmpMsg = header .. name .. " ^7was banned!" .. reason
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
   else
        userprint(clientNum, "chat", commandhelp["ban"])
    end
    return
end

function botkickall()
	et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot kickall")
end

function botplayers(clientNum, params)
	local header = "^3botplayers: ^7"
	local bplayer = params[1]
	local maxclients = tonumber(et.trap_Cvar_Get("sv_maxclients"))

	if bplayer ~= nil then
		if isNumeric(bplayer) then
			bplayer = tonumber(bplayer)
			if (bplayer < 1) or  (bplayer > (maxclients - 1)) then 
				userprint(clientNum, "chat", header .. "must be between 1 and " .. tostring(maxclients - 1))
				return
			end
			et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot minbots -1")
			et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot maxbots " .. bplayer)	
			userprint(-1, "chat", header .. "maxbots changed to " .. bplayer)
		else
			userprint(clientNum, "chat", "^3botplayers: ^5" .. bplayer .. " ^7is not a number")
			return
		end
	else
		userprint(clientNum, "chat", commandhelp["botplayers"])
	end
end

function botteam(clientNum, params)
	local team = params[1]
	
	if team ~= nil and isNumeric(team) then
		team = tonumber(team)
		if team >= 0 and team < 3 then
			if team > 0 then
				et.trap_Cvar_Set("g_teamForceBalance", "0")
				et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot balanceteams 0")
				userprint(-1, "cp", "^3bots vs humans enabled. bots are set to the " .. TEAMS[team] .. " team.")
			else
				et.trap_Cvar_Set("g_teamForceBalance", "1")
				et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot balanceteams 1")
				userprint(-1, "cp", "^3bots vs humans disabled. bots will balance teams.")			
			end
			
			et.trap_Cvar_Set("g_botteam", team)
			return
		end
	end
	
	userprint(clientNum, "chat", commandhelp["botteam"])
end

function date(clientNum)
	local date = os.date("%A, %B %d, %Y")
	userprint(-1, "chat", "^3date: ^7The server date is " .. date)
    return
end

function difficulty(clientNum, params)
	local difficulty = params[1]
	
	if difficulty ~= nil and isNumeric(difficulty) then
		difficulty = tonumber(difficulty)
		if difficulty >= -1 and difficulty <= MAX_DIFFICULTY then
			et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot difficulty " .. difficulty)
			userprint(-1, "cp", "^3bot difficulty set to " .. difficulty)
			return
		end
	end
	
	userprint(clientNum, "chat", commandhelp["difficulty"])
end

function fling(clientNum, params)
	local header = "^3fling: ^7"
    local victim = ReadyCommand(clientNum, "fling", params)
    
    if victim == 99 then
        return
    end

	if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
		           
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team!")
	        return
        end
        if victim_health <=0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive!")
	        return
        end
		et.FlingClient(victim, 0)  -- 0=fling, 1=throw, 2=launch
        local tmpMsg = header .. name .. " ^7was flung!"
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["fling"])
    end
    return
end

function flinga(clientNum)
	local header = "^3flinga: ^7"
    
	et.FlingClientA(0)  -- 0=fling, 1=throw, 2=launch
	local tmpMsg = header .. "^7everyone was flung!"
	userprint(-1, "chat", tmpMsg)
	et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    return
end

function gib(clientNum, params)
    local header = "^3gib: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "gib", params)
    
    if victim == 99 then
        return
    end
 
    if table.getn(params) >= 2 then
        reason = " ^3reason: ^7"
        for i = 2, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end
    
    if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
        
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team")
	        return
        end
        if victim_health <= 0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive")
	        return
        end

		-- explanation -  et.G_Damage(target, inflictor, attacker, damage, dflags, mod), mod 0 = unknown
		et.G_Damage(victim, 80, 1022, 667, 8, 34)
		-- et.gentity_set(victim, "ps.persistant[PERS_SCORE]", (et.gentity_get(victim,"ps.persistant[PERS_SCORE]") + 1))
		local tmpMsg = header .. name .. " ^7was gibbed!" .. reason
		et.trap_SendServerCommand(victim, "cp \"" .. reason .. " \"\n")
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["gib"])
    end
    return
end

function help(clientNum, params)
    local header = "^3help: ^7"
    local clientguid
    local clientlvl
    local clientflags
    
    if clientNum == rconclient then
        clientflags = "*"
    else
	    clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid" ))
        clientlvl = "0"
        clientflags = global_level_table[clientlvl]["flags"]
        
        if (global_admin_table[clientguid] ~= nil) then
            clientlvl = tostring(global_admin_table[clientguid]["level"])
        end
        clientflags = global_level_table[clientlvl]["flags"]
    end

    if table.getn(params) >= 1 then
        -- display help for a command
        local command = string.lower(params[1])
        
	    if commandhelp[command] == nil then
	        userprint(clientNum, "chat", "^3help: ^5" .. command .. " ^7is not a valid command")
	        return
	    end
        if string.find(clientflags, commandflags[command]) == nil and clientflags ~= "*" then
            userprint(clientNum, "chat", header .. "insufficient admin status")
            return
        end
	    
        userprint(clientNum, "print", header .. "help for '" .. command .. "' \n")
        userprint(clientNum, "print", commandhelp[command] .. "\n")
        userprint(clientNum, "chat", header .. "commands help have been printed in console")
    else
        -- list commands
        local counter = 0
        local cmdcount = 0
        local printline = ""
        local temptable = {}

        table.foreach(commandhelp, function (k) table.insert (temptable, k) end)
        table.sort(temptable)
        
	    for key, helpcmd in pairs(temptable) do
            if string.find(clientflags, commandflags[helpcmd]) or clientflags == "*" then
                counter = counter + 1
                cmdcount = cmdcount + 1
                printline = printline .. helpcmd .. string.rep(" ", (13 - string.len(helpcmd)))  .. " "
            end
            if counter >= 5 then
                userprint(clientNum, "print", header .. " ^7" .. printline .. "\n")
                counter = 0
                printline = ""
            end
        end
        if printline ~= "" then
            userprint(clientNum, "print", header .. " ^7" .. printline .. "\n")
        end
        userprint(clientNum, "chat", header .. " ^7" .. cmdcount .. " commands have been listed in console")
    end
end

function kick(clientNum, params)
    local header = "^3kick: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "kick", params)
	local kicktime = tonumber(global_config_table["kicktime"])

    
    if victim == 99 then
        return
    end
 
    if table.getn(params) >= 2 then
        for i = 2, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end
    
    if (reason == "") then
	    reason = "kicked by admin"
    end
				
    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
		local banner = ""
		local expires = (kicktime * 60) + et.GetTimeStamp()
		
		if et.IsBot(victim) then
			userprint(clientNum, "chat", header .. "cannot kick bots")
			return 
		end
		if clientNum==rconclient then
			banner = rconclient
		else
			banner = et.gentity_get(clientNum,"pers.netname")
		end
		mmodDB = sqlite3.open(dbname)
		mmodDB:exec('insert into tblBans (guid,name,reason,expires,made,banner) values("'.. guid ..'", "'.. name ..'", "'.. reason ..'", '.. expires ..', "'.. os.date("%c") ..'", "'.. banner .. '")' )
		mmodDB:close()
		mmodDB = nil	
        et.trap_DropClient(victim, reason, 0)
        reason = " ^3reason: ^7" .. reason
        local tmpMsg = header .. name .. " ^7was kicked!" .. reason
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["kick"])
    end
    return
end

function launch(clientNum, params)
	local header = "^3launch: ^7"
    local victim = ReadyCommand(clientNum, "launch", params)
    
    if victim == 99 then
        return
    end

	if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
		        
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team!")
	        return
        end
        if victim_health <=0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive!")
	        return
        end
		et.FlingClient(victim, 2)  -- 0=fling, 1=throw, 2=launch
        local tmpMsg = header .. name .. " ^7was launched!"
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["launch"])
    end
    return
end

function launcha(clientNum)
	local header = "^3launcha: ^7"
    
	et.FlingClientA(2)  -- 0=fling, 1=throw, 2=launch
	local tmpMsg = header .. "^7everyone was launched!"
	userprint(-1, "chat", tmpMsg)
	et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    return
end

function listadmins(clientNum, params)
    local header = "^3listadmins: ^7"
	local admins = 0
	local message = ""
	local levelname = ""
	local num = params[1]
	local maxind = 0
	local count = 0

    if num ~= nil then
		if isNumeric(num) then
			num = tonumber(num)
		else
			userprint(clientNum, "chat", "^3showbans: ^5" .. num .. " ^7is not a number")
			return
		end
    else
        num = 0
    end
	
	mmodDB = sqlite3.open(dbname)
	
	for row in mmodDB:nrows('SELECT count(*) as count FROM tblAdmins') do
		count = row.count
	end
	
	if count == 0 then
		mmodDB:close()
		mmodDB = nil
		userprint(clientNum, "chat", header .. "there are no admins")
		return
	end	
	
	for row in mmodDB:nrows('SELECT MAX(ind) as maxind FROM tblAdmins') do
		maxind = row.maxind
	end
	
	if num > maxind then
		mmodDB:close()
		mmodDB = nil
		userprint(clientNum, "chat", "^3listadmins: ^5" .. num .. " ^7is invalid. There are only " .. maxind .. " admins.")
		return
	end
	
	for row in mmodDB:nrows("SELECT * FROM tblAdmins WHERE ind >= " .. num .. " LIMIT 15") do
		levelname = chop(global_level_table[tostring(row.level)]["name"])
		levelname = crop_text(levelname, 20)
		message = ""
		admins = admins + 1
		if row.ind < 10 then
			message = "^7 " .. row.ind .. " "
		else
			message = "^7" .. row.ind .. " "
		end	
		if row.level < 10 then
			message = message .. "^7 " .. row.level .. " "
		else
			message = message .. "^7" .. row.level .. " "
		end
		message = message .."'" ..  string.rep(" ", (20 - string.len(levelname))) .. levelname .. "'"
		message = message .. " ^7" .. row.name
    	userprint(clientNum, "print", message .. " \n")
	end
	mmodDB:close()
	mmodDB = nil
	userprint(clientNum, "chat", header .. admins .. " admins have been listed in the console.")		
end

function listplayers(clientNum)
    local header = "^3listplayers: ^7"
	local players = 0
	local message = ""
	local slot
	local guid = ""
	local level = ""
	local levelname = ""
	local pmute = ""

	for slot=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do
		message = ""
		local name = et.Info_ValueForKey( et.trap_GetUserinfo( slot ), "name" )
		if (name ~= "") then -- slot isnt empty
			players = players + 1
            local team = et.gentity_get(slot,"sess.sessionTeam")
            if team == 1 then	
                team = "^1R "
            elseif team == 2 then
        	    team = "^4B "
            elseif team == 3 then
        	    team = "^3S "
            else
        	    team = "^3C " -- connecting, not sure its possible though
            end
			guid = string.lower(tostring(et.Info_ValueForKey( et.trap_GetUserinfo(slot), "cl_guid" )))
			if (global_mute_table[guid] ~= nil) then
				pmute = "^1M "
			else
				pmute = "  "
			end
			level = "0"
			if (global_admin_table[guid] ~= nil) then
		        level = tostring(global_admin_table[guid]["level"])
	        end
			levelname = chop(global_level_table[level]["name"])
			levelname = crop_text(levelname, 20)
			if et.IsBot(slot) then
			    guid = " ( OmniBot ) "
			else
			    guid = " (*" .. string.sub(guid, 25) .. ") "
			end
			if slot < 10 then
		        message = "^7 " .. slot .. " "
		    else
		        message = "^7" .. slot .. " "
		    end
		    message = message .. team
		    message = message .. "^7" .. level .. " "
		    message = message .."'" ..  string.rep(" ", (20 - string.len(levelname))) .. levelname .. "'"
		    message = message .. guid
		    message = message .. pmute
		    message = message .. "^7" .. name
    	    userprint(clientNum, "print", message .. " \n")
		end
	end
	userprint(clientNum, "chat", header .. players .. " players have been listed in the console.")	
    return
end

function lol(clientNum, params)
	local header = "^3lol: ^7"
    local victim = ReadyCommand(clientNum, "lol", params)
    
    if victim == 99 then
        return
    end

	if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
		        
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team!")
	        return
        end
        if victim_health <=0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive!")
	        return
        end
		et.lol(victim)
        local tmpMsg = header .. name
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["lol"])
    end
    return
end

function map(clientNum, params)
	local header = "^3map: ^7"

	if params[1] ~= nil then
		et.SetMap(params[1])
		local tmpMsg = header .. "^7map changed to: ^3" .. params[1]
		userprint(-1, "chat", tmpMsg)
	else
		userprint(clientNum, "chat", commandhelp["map"])
	end
end

function maprecords(clientNum)
	local header = "^3maprecords: ^7"
	local count = 0

	OpenDB()
	
	for row in RecordsDB:nrows('SELECT count(*) as count FROM Records') do
		count = row.count
	end
	
	if count == 0 then
		et.trap_SendServerCommand(clientNum,string.format('chat \"^3maprecords: ^7No Records for this server yet\"'))
		CloseDB()
		return
	end	
	
	mapId = getMapId(et.GetMapName())
	tblheader = "^3----------------------------------------------------------\n^3Record          Value      Date            Player\n^3----------------------------------------------------------\n"

	if clientNum == rconclient then
		et.G_Print(string.format('%s',tblheader))
	else
		et.trap_SendServerCommand(clientNum, string.format('print \"%s\"',tblheader))
	end
	for row in RecordsDB:nrows('SELECT * FROM Records WHERE mapId=' .. mapId) do
		if clientNum == rconclient then
			et.G_Print(string.format('^3%-15s ^5%-10d ^7%-15s %-36s\n',recordTypes[row.recordTypeId],row.value,row.recordDate,getPlayerName(row.playerId)))
		else
			et.trap_SendServerCommand(clientNum,string.format('print \"^3%-15s ^5%-10d ^7%-15s %-36s\n\"',recordTypes[row.recordTypeId],row.value,row.recordDate,getPlayerName(row.playerId)))
		end
	end
	CloseDB()
	userprint(clientNum, "chat", header .. "map records have been printed in console")
    return
end

function mute(clientNum, params)
    local header = "^3mute: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "mute", params)
    
    if victim == 99 then
        return
    end
 
    if table.getn(params) >= 2 then
        for i = 2, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end
    
    if (reason == "") then
	    reason = "muted by admin"
    end
				
    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
		
		if et.IsBot(victim) then
			userprint(clientNum, "chat", header .. "cannot mute bots")
			return 
		end
		if global_mute_table[guid] ~= nil then
			userprint(clientNum, "chat", header .. name .. "^7 is already muted")
			return 
		end
		mmodDB = sqlite3.open(dbname)
		mmodDB:exec('insert into tblMutes (guid,name) values("'.. guid ..'", "'.. name ..'")')
		mmodDB:close()
		mmodDB = nil	
        reason = " ^3reason: ^7" .. reason
        local tmpMsg = header .. name .. " ^7was muted!" .. reason
		et.trap_SendServerCommand(victim, "cp \"" .. reason .. " \"\n")
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
		loadMutes()
    else
        userprint(clientNum, "chat", commandhelp["mute"])
    end
    return
end

function nextmap(clientNum)
    local tmpMsg = "^3nextmap: ^7nextmap has been loaded!"
    userprint(-1, "chat", tmpMsg)
    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
	et.trap_SendConsoleCommand(et.EXEC_APPEND, "vstr nextmap")
    return
end

function putteam(clientNum, params)
    local header = "^3putteam: ^7"
    local victim = ReadyCommand(clientNum, "putteam", params)
    
    if victim == 99 then
        return
    end
    
    if victim ~= -1 then
	    local newteam = ""
	    local fullteam = ""
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
		        
        if table.getn(params) < 2 then
            userprint(clientNum, "chat", header .. "usage: !putteam [name|slot] [b|r|s]")
            return
        end
		
	    if ( team == 1 ) then
		    team = "r"
	    elseif ( team == 2 ) then
		    team = "b"
	    elseif ( team == 3 ) then
		    team = "s"
		end

		params[2] = string.lower(params[2])
	    
	    if ( params[2] == "r" or params[2] == "axis" ) then
		    newteam = "r"
		    fullteam = "^1axis"
	    elseif ( params[2] == "b" or params[2] == "allies" ) then
		    newteam = "b"
		    fullteam = "^4allies"
	    elseif ( params[2] == "s" or params[2] == "spec" ) then
		    newteam = "s"
		    fullteam = "^3spectators"
	    else
            userprint(clientNum, "chat", header .. "unknown team ^5" ..params[2])
		    return
	    end
	    if newteam == team then
            userprint(clientNum, "chat", header .. name .. " ^7is already on team " .. fullteam)
	        return
	    end
		et.SetTeam(victim, newteam)
        local tmpMsg = header .. name .. " ^7joined the " .. fullteam
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["putteam"])
    end
end

function rename(clientNum, params)
    local header = "^3rename: ^7"
    local newname = ""
    local victim = ReadyCommand(clientNum, "rename", params)
    
    if victim == 99 then
        return
    end
				
    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
        local userinfo = et.trap_GetUserinfo(victim)
         
        if table.getn(params) < 2 then
            userprint(clientNum, "chat", header .. "usage: !rename [name|slot] [new name]")
            return
        end
        
        for i = 2, table.getn(params) do
            newname = newname .. tostring(params[i]) .. " "
        end
        
        newname = chop(newname)                
		userinfo = et.Info_SetValueForKey(userinfo, "name", newname)
	    et.trap_SetUserinfo(victim, userinfo)
        et.ClientUserinfoChanged(victim)
        local tmpMsg = header .. name .. " ^7was renamed to " .. newname
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["rename"])
    end
    return
end

function reset(clientNum)
    local tmpMsg = "^3reset: ^7match was resetted!"
    userprint(-1, "chat", tmpMsg)
    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
	et.trap_SendConsoleCommand(et.EXEC_APPEND, "reset_match")
    return
end

function restart(clientNum)
    local tmpMsg = "^3restart: ^7map was restarted!"
    userprint(-1, "chat", tmpMsg)
    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
	et.trap_SendConsoleCommand(et.EXEC_APPEND, "map_restart")
    return
end

function serverrecords(clientNum)
	local header = "^3serverrecords: ^7"
	local count = 0

	OpenDB()
	
	for row in RecordsDB:nrows('SELECT count(*) as count FROM Records') do
		count = row.count
	end
	
	if count == 0 then
		et.trap_SendServerCommand(clientNum,string.format('chat \"^3serverrecords: ^7No Records for this server yet\"'))
		CloseDB()
		return
	end	
	
	tblheader = "^3----------------------------------------------------------\n^3Record          Value      Map             Player\n^3----------------------------------------------------------\n"

	if clientNum == rconclient then
		et.G_Print(string.format('%s',tblheader))
	else
		et.trap_SendServerCommand(clientNum, string.format('print \"%s\"',tblheader))
	end

	for row in RecordsDB:nrows('SELECT r.mapId, r.playerId, r.recordTypeId, r.value FROM (SELECT mapId, playerId, recordTypeId, MAX(value) AS highest FROM Records GROUP BY recordTypeId) AS x INNER JOIN Records AS r ON r.recordTypeId = x.recordTypeId AND r.Value = x.highest') do
		if row.value then
			if clientNum == rconclient then
				et.G_Print(string.format('^3%-15s ^5%-10d ^2%-15s %-36s\n',recordTypes[row.recordTypeId],row.value,getMapName(row.mapId),getPlayerName(row.playerId)))
			else
				et.trap_SendServerCommand(clientNum,string.format('print \"^3%-15s ^5%-10d ^2%-15s %-36s\n\"',recordTypes[row.recordTypeId],row.value,getMapName(row.mapId),getPlayerName(row.playerId)))
			end
		end
	end

	CloseDB()
	userprint(clientNum, "chat", header .. "server records have been printed in console")
    return
end

function setlevel(clientNum, params) 
    local header = "^3setlevel: ^7"
    local victim = ReadyCommand(clientNum, "setlevel", params)
    
    if victim == 99 then
        return
    end
	
    if victim ~= -1 then
        local level = tostring(params[2])
		local name = et.Info_ValueForKey(et.trap_GetUserinfo(victim), "name" )
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
        
        loadAdmins()
        if not isNumeric(tonumber(level)) then
            userprint(clientNum, "chat", header .. "^5" .. level .. " ^7is not a number")
            return
        end

        if global_level_table[level] == nil then
            userprint(clientNum, "chat", header .. "admin level ^5" .. level .. " ^7is invalid")
            return
        end
        if clientNum ~= rconclient then
		    local clientguid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(clientNum), "cl_guid" ))
		    local clientlvl = global_admin_table[clientguid]["level"]
            if tonumber(level) > clientlvl then
                userprint(clientNum, "chat", header .. "can not setlevel higher then yours")
				return
            end
        end
        if global_admin_table[guid] == nil then -- new admin
            if level == "0" then 
                userprint(clientNum, "chat", header .. "can not set new admin to level 0")
				return
            end
			mmodDB = sqlite3.open(dbname)
			mmodDB:exec('insert into tblAdmins (guid,level,name) values("' .. guid .. '", ' .. tonumber(level) .. ', "' .. name .. '")' )
			mmodDB:close()
			mmodDB = nil	
        else 
            if level == "0" then 
				mmodDB = sqlite3.open(dbname)
				mmodDB:exec('delete from tblAdmins where guid = "' .. guid .. '"' )
				mmodDB:close()
            else
				mmodDB = sqlite3.open(dbname)
				mmodDB:exec('update tblAdmins set level=' .. level .. ' where guid = "' .. guid .. '"' )
				mmodDB:close()
            end
        end
		loadAdmins()
        local tmpMsg = header .. name .. " ^7was set to level " .. level
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["setlevel"])
    end
    return
end

function showbans(clientNum, params)
    local header = "^3showbans: ^7"
	local num = params[1]
	local counter = 0
	local maxind = 0
	local count = 0
	
    if num ~= nil then
		if isNumeric(num) then
			num = tonumber(num)
		else
			userprint(clientNum, "chat", "^3showbans: ^5" .. num .. " ^7is not a number")
			return
		end
    else
        num = 0
    end

	mmodDB = sqlite3.open(dbname)
	
	mmodDB:exec('DELETE FROM tblBans WHERE (expires - ' .. et.GetTimeStamp() .. ') <= 0 and expires <> 0') --delete expired bans
	
	for row in mmodDB:nrows('SELECT count(*) as count FROM tblBans') do
		count = row.count
	end
	
	if count == 0 then
		mmodDB:close()
		mmodDB = nil
		userprint(clientNum, "chat", header .. "there are no bans")
		return
	end	
	
	for row in mmodDB:nrows('SELECT MAX(ind) as maxind FROM tblBans') do
		maxind = row.maxind
	end
	
	if num > maxind then
		mmodDB:close()
		mmodDB = nil
		userprint(clientNum, "chat", "^3showbans: ^5" .. num .. " ^7is invalid. There are only " .. maxind .. " bans.")
		return
	end
	
	for row in mmodDB:nrows("SELECT * FROM tblBans WHERE ind >= " .. num .. " LIMIT 15") do
		local ind = row.ind
		local guid = row.guid
		local name = row.name
		local reason = row.reason
		local expires = row.expires
		local made = crop_text(row.made, 8)
		local banner = row.banner
		
		if ind < 10 then
			message = "^5 " .. tostring(ind) .. " "
		else
			message = "^5" .. tostring(ind) .. " "
		end
		emsg = expiremsg(expires)
		message = message .. " ^7" .. name .. string.rep(" ", (20 - string.len(et.Q_CleanStr(name))))
		message = message .. " ^5" .. made
		message = message .. " ^7" .. banner .. string.rep(" ", (20 - string.len(et.Q_CleanStr(banner))))
		message = message .. " ^5" .. emsg .. string.rep(" ", (20 - string.len(et.Q_CleanStr(emsg))))
		message = message .. "\n" .. string.rep(" ", 5) .. "^3Reason: " .. "^7" .. reason
		userprint(clientNum, "print", message .. " \n")
	end
	userprint(clientNum, "chat", header .. "^7bans have been listed in console")
	mmodDB:close()
	mmodDB = nil
end

function shuffle(clientNum)
	local header = "^3shuffle: ^7"
	local slottable = {}
	local teamtable = {}
	local tableindex = 1
	local newteam = ""
    
	for i=0,tonumber(et.trap_Cvar_Get("sv_maxclients"))-1,1 do
		tempname = string.lower(et.Q_CleanStr(et.gentity_get(i,"pers.netname")))
		if (tempname ~= "") then
			tempteam = et.gentity_get(i,"sess.sessionTeam")
			if (tempteam ~= 3) then
				slottable[tableindex] = i
				teamtable[tableindex] = tempteam
				tableindex = tableindex + 1
			end
		end
	end

	--shuffle array
	local n = table.getn(slottable)
	while n >= 2 do
		-- n is now the last pertinent index
		local k = math.random(n) -- 1 <= k <= n
		-- Quick swap
		teamtable[n], teamtable[k] = teamtable[k], teamtable[n]
		n = n - 1
	end
	
	for n = 1,table.getn(slottable),1 do
		if teamtable[n] == 1 then
			newteam = "r"
		else
			newteam = "b"
		end
		et.SetTeam(slottable[n], newteam)
	end

	local tmpMsg = header .. "^7teams were shuffled!"
	userprint(-1, "chat", tmpMsg)
	et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    return
end

function slap(clientNum, params)
    local header = "^3slap: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "slap", params)
	local slapdamage = tonumber(global_config_table["slapdamage"])
    
    if victim == 99 then
        return
    end
 
    if table.getn(params) >= 2 then
        reason = " ^3reason: ^7"
        for i = 2, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end
    
    if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
            
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team!")
	        return
        end
        if victim_health <=0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive!")
	        return
        end

        if victim_health <= (slapdamage + 1) then -- dont kill the guy
	        et.gentity_set(victim, "health", 1)
        else
	        et.gentity_set(victim, "health", victim_health - slapdamage)
        end
        local tmpMsg = header .. name .. " ^7was slapped!" .. reason
		et.trap_SendServerCommand(victim, "cp \"" .. reason .. " \"\n")
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
        local sound = 'sound/multiplayer/hurt_barbwire.wav'
        soundindex = et.G_SoundIndex(sound)
        et.G_Sound(victim,  soundindex)
    else
        userprint(clientNum, "chat", commandhelp["slap"])
    end
    return
end

function spec999(clientNum)
    local header = "^3spec999: ^7"	
	local matches = 0
	
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
	    local name = et.gentity_get(i,"pers.netname")
		local team = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		local ping = tonumber(et.gentity_get(i,"ps.ping"))
		if team ~= 3 and team ~= 0 then
			if ping >= 999 then
				matches = matches + 1
				et.SetTeam(i, "s")
                userprint(-1, "chat", header .. name .. " ^7moved to spectator")			
			end
		end
	end
	if matches > 1 then
	    userprint(-1, "chat", header .. " moved " .. matches .. " players to spectator")
	else
	    userprint(-1, "chat", header .. " moved " .. matches .. " player to spectator")
	end
    return
end

function stats(clientNum, params)
    local header = "^3stats: ^7"	
	local victim = ReadyCommand(clientNum, "stats", params)
	local playerName = ""
	local i
	
	if victim == 99 then
        return
    end

    if victim ~= -1 then
		i = victim
	else
		i = clientNum
	end
	
	playerName = et.gentity_get(i,"pers.netname")
	userprint(clientNum, "print", header .. "Player Stats for " .. playerName .. "\n" )
	userprint(clientNum, "print", header .. "-----------------------------------\n" )
	userprint(clientNum, "print", header .. "Kills: " .. et.gentity_get(i,"pers.kills") .. "\n" )
	userprint(clientNum, "print", header .. "Deaths: " .. et.gentity_get(i,"pers.deaths") .. "\n" )  -- was ps.persistant[PERS_KILLED]
	userprint(clientNum, "print", header .. "Suicides: " .. et.gentity_get(i,"pers.suicides") .. "\n" )
	userprint(clientNum, "print", header .. "Team Kills: " .. et.gentity_get(i,"pers.teamKills") .. "\n" )
	userprint(clientNum, "print", header .. "Kill Spree Max: " .. et.gentity_get(i,"pers.killSpreeMax") .. "\n" )
	local deathspree = et.gentity_get(i,"pers.deathSpreeMax")
	if deathspree < 0 then
		deathspree = deathspree * -1
	end
	userprint(clientNum, "print", header .. "Death Spree Max: " .. deathspree .. "\n" )
	userprint(clientNum, "print", header .. "Damage Given: " .. et.gentity_get(i,"pers.damage_given") .. "\n" )
	userprint(clientNum, "print", header .. "Damage Received: " .. et.gentity_get(i,"pers.damage_received") .. "\n" )
	userprint(clientNum, "print", header .. "Team Damage: " .. et.gentity_get(i,"pers.team_damage") .. "\n" )
	local hits = et.gentity_get(i,"pers.acc_hits")
	local shots = et.gentity_get(i,"pers.acc_shots")
	local acc = 0
	if (shots ~= 0) and (hits ~= 0) then
		acc = (hits / shots) * 100
		acc = round(acc, 2)
	end
	userprint(clientNum, "print", header .. "Accuracy: " .. acc .. " (" .. hits .. " hits / " .. shots .. " shots)\n" )
	userprint(clientNum, "print", header .. "Headshots: " .. et.gentity_get(i,"pers.headshots") .. "\n" )
	userprint(clientNum, "print", header .. "Goombas: " .. et.gentity_get(i,"pers.goomba") .. "\n" )
	userprint(clientNum, "print", header .. "Poisons: " .. et.gentity_get(i,"pers.poison") .. "\n" )
	userprint(clientNum, "print", header .. "Revives: " .. et.gentity_get(i,"pers.revives") .. "\n" )
	userprint(clientNum, "print", header .. "Med Packs: " .. et.gentity_get(i,"pers.medPacks") .. "\n" )
	userprint(clientNum, "print", header .. "Ammo Packs: " .. et.gentity_get(i,"pers.ammoPacks") .. "\n" )
	userprint(clientNum, "chat", header .. "player stats have been printed in console")
end

function swap(clientNum)
    local header = "^3swap: ^7"	
	
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local team = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		if team ~= 3 and team ~= 0 then
			if team == 1 then
				et.SetTeam(i, "b")
            else
				et.SetTeam(i, "r")
			end
		end
	end
    userprint(-1, "chat", header .. " teams have been swapped")
    return
end

function throw(clientNum, params)
	local header = "^3throw: ^7"
    local victim = ReadyCommand(clientNum, "throw", params)
    
    if victim == 99 then
        return
    end

	if victim ~= -1 then
        local victim_health = et.gentity_get(victim, "health")
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
		
        if team == 3 then -- spec
	        userprint(clientNum, "chat", header .. name .. " ^7must be on a team!")
	        return
        end
        if victim_health <=0 then 
	        userprint(clientNum, "chat", header .. name .. " ^7must be alive!")
	        return
        end
		et.FlingClient(victim, 1)  -- 0=fling, 1=throw, 2=launch
        local tmpMsg = header .. name .. " ^7was thrown!"
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["throw"])
    end
    return
end

function throwa(clientNum)
	local header = "^3throwa: ^7"
    
	et.FlingClientA(1)  -- 0=fling, 1=throw, 2=launch
	local tmpMsg = header .. "^7everyone was thrown!"
	userprint(-1, "chat", tmpMsg)
	et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    return
end


function time(clientNum)
	local time = os.date("%I:%M:%S%p")
	userprint(-1, "chat", "^3time: ^7The server time is " .. time)
    return
end

function timelimit(clientNum, params)
    local header = "^3timelimit: ^7"
	local tlimit = params[1]
    
    if tlimit ~= nil then
		if isNumeric(tlimit) then
			tlimit = tonumber(tlimit)
			if tlimit < 1 then
				userprint(clientNum, "chat", header .. "time limit must be greater than 1")
				return
			end
			et.trap_SendConsoleCommand( et.EXEC_INSERT, "timelimit " .. tlimit)	
			userprint(-1, "chat", header .. "time limit was changed to " .. tlimit)			
		else
			userprint(clientNum, "chat", "^3timelimit: ^5" .. tlimit .. " ^7is not a number")
			return
		end
    else
        userprint(clientNum, "chat", commandhelp["timelimit"])
    end
end

function unban(clientNum, params)
    local header = "^3unban: ^7"
	local bannum = params[1]
	
    if bannum ~= nil then
		if isNumeric(bannum) then
			bannum = tonumber(bannum)
			local counter = 0
			local name = ""
			
			mmodDB = sqlite3.open(dbname)
			for row in mmodDB:nrows('SELECT * FROM tblBans WHERE ind=' .. bannum) do
				counter = counter + 1
				name = row.name
			end

			if counter ~= 0 then
				mmodDB:exec('DELETE FROM tblBans WHERE ind=' .. bannum)
				userprint(-1, "chat", header .. name .. " ^7was unbanned")			
			else
				userprint(clientNum, "chat", "^3unban: ^5" .. bannum .. " ^7is an invalid ban number")			
			end
			mmodDB:close()
			mmodDB = nil	
		else
			userprint(clientNum, "chat", "^3unban: ^5" .. bannum .. " ^7is not a number")
			return
		end
    else
        userprint(clientNum, "chat", commandhelp["unban"])
    end
end

function unmute(clientNum, params)
    local header = "^3unmute: ^7"
    local victim = ReadyCommand(clientNum, "slap", params)
    
    if victim == 99 then
        return
    end
	
    if victim ~= -1 then
		local name = et.Info_ValueForKey(et.trap_GetUserinfo(victim), "name" )
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
		
		if global_mute_table[guid] == nil then
			userprint(clientNum, "chat", header .. name .. " ^7is not muted")
			return
		end
		
		mmodDB = sqlite3.open(dbname)
		mmodDB:exec('DELETE FROM tblMutes WHERE guid="' .. guid .. '"')
		userprint(-1, "chat", header .. name .. " ^7was unmuted")
		mmodDB:close()
		mmodDB = nil	
		loadMutes()
    else
        userprint(clientNum, "chat", commandhelp["unmute"])
    end
end

function warn(clientNum, params)
    local header = "^3warn: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "warn", params)
    
    if victim == 99 then
        return
    end
 
    if table.getn(params) >= 2 then
        reason = " ^3reason: ^7"
        for i = 2, table.getn(params) do
            reason = reason .. tostring(params[i]) .. " "
        end
    end

    if victim ~= -1 then
        local name = et.gentity_get(victim,"pers.netname")
        local tmpMsg = header .. name .. " ^7was warned!" .. reason
		et.trap_SendServerCommand(victim, "cp \"" .. reason .. " \"\n")
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
        local sound = 'sound/referee.wav'
        soundindex = et.G_SoundIndex(sound)
        et.G_Sound(victim,  soundindex)
    else
        userprint(clientNum, "chat", commandhelp["warn"])
    end
    return
end