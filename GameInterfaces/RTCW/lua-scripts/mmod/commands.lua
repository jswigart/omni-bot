function test()
	local i = 2
	et.G_Print("test \n")
	et.G_Print("---" .. string.find(string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(i), "cl_guid" )),"omnibot") .. "---\n")
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
		
		if string.find(guid,"omnibot") then
			userprint(clientNum, "chat", header .. "cannot ban bots")
			return 
		end
		global_ban_table[guid] = {}
		global_ban_table[guid]["name"] = name
		global_ban_table[guid]["reason"] = reason
		global_ban_table[guid]["made"] = made
		global_ban_table[guid]["expires"] = expires
		if clientNum==rconclient then
			banner = rconclient
		else
			banner = et.gentity_get(clientNum,"pers.netname")
		end
		global_ban_table[guid]["banner"] = banner
		if expires == 0 then
			bmsg = "(Perm Ban) "
		else
			bmsg = "(Temp Ban) "
		end
        et.trap_DropClient(victim, bmsg .. reason, 0)
		writeBans()
        reason = " ^3reason: ^7" .. reason
        local tmpMsg = header .. name .. " ^7was banned!" .. reason
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["ban"])
    end
    return

end

function botplayers(clientNum, params)
    local header = "^3botplayers: ^7"
	local bplayer = params[1]
	local maxclients = tonumber(et.trap_Cvar_Get("sv_maxclients"))
    
    if bplayer ~= nil then
		if isNumeric(bplayer) then
			bplayer = tonumber(bplayer)
            if (bplayer < 0) or  (bplayer > (maxclients - 1)) then 
				userprint(clientNum, "chat", header .. "must be between 0 and " .. tostring(maxclients - 1))
                return
            end
			et.trap_SendConsoleCommand(et.EXEC_INSERT, "bot maxbots " .. bplayer)		
		else
			userprint(clientNum, "chat", "^3botplayers: ^5" .. bplayer .. " ^7is not a number")
			return
		end
    else
        userprint(clientNum, "chat", commandhelp["botplayers"])
    end
end

function date(clientNum)
	local date = os.date("%A, %B %d, %Y")
	userprint(-1, "chat", "^3date: ^7The server date is " .. date)
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
        userprint(clientNum, "chat", header .. "commands help has been printed in console")
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
                printline = printline .. helpcmd .. string.rep(" ", (12 - string.len(helpcmd)))  .. " "
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

        et.gentity_set(victim, "health", -600)
        local tmpMsg = header .. name .. " ^7was gibbed!" .. reason
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
        local sound = 'sound/multiplayer/hurt_barbwire.wav'
        soundindex = et.G_SoundIndex(sound)
        et.G_Sound(victim,  soundindex)
    else
        userprint(clientNum, "chat", commandhelp["gib"])
    end
    return
end

function kick(clientNum, params)
    local header = "^3kick: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "kick", params)
    
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
        -- et.trap_SendConsoleCommand( et.EXEC_INSERT, "clientkick " .. victim)
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

function listplayers(clientNum)
    local header = "^3listplayers: ^7"
	local players = 0
	local message = ""
	local slot
	local guid = ""
	local level = ""
	local levelname = ""

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
			local mute = 0 -- tonumber(et.gentity_get(slot, "sess.muted"))
			if (mute == 1) then
				mute = "^1M "
			else
				mute = "  "
			end
			guid = string.lower(tostring(et.Info_ValueForKey( et.trap_GetUserinfo(slot), "cl_guid" )))
			level = "0"
			if (global_admin_table[guid] ~= nil) then
		        level = tostring(global_admin_table[guid]["level"])
	        end
			levelname = chop(global_level_table[level]["name"])
			levelname = crop_text(levelname, 20)
			if string.find(string.lower(guid),"omnibot") then
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
		    message = message .. mute
		    message = message .. "^7" .. name
    	    userprint(clientNum, "print", message .. " \n")
		end
	end
	userprint(clientNum, "chat", header .. players .. " players have been listed in the console.")	
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
	    local newteam
	    local fullteam = ""
        local name = et.gentity_get(victim,"pers.netname")
        local team = et.gentity_get(victim,"sess.sessionTeam")  -- 1=axis ,2=allies ,3=spec
        
        if table.getn(params) < 2 then
            userprint(clientNum, "chat", header .. "usage: !putteam [name|slot] [b|r|s]")
            return
        end
	    
	    if ( params[2] == "r" or params[2] == "axis" ) then
		    newteam = 1
		    fullteam = "^1axis"
	    elseif ( params[2] == "b" or params[2] == "allies" ) then
		    newteam = 2
		    fullteam = "^4allies"
	    elseif ( params[2] == "s" or params[2] == "spec" ) then
		    newteam = 3
		    fullteam = "^3spectators"
	    else
            userprint(clientNum, "chat", header .. "unknown team ^5" ..params[2])
		    return
	    end
	    if newteam == team then
            userprint(clientNum, "chat", header .. name .. " ^7is already on team " .. fullteam)
	        return
	    end
        et.gentity_set(victim, "health", -600)
        et.gentity_set(victim, "sess.sessionTeam", newteam)
        et.ClientUserinfoChanged(victim)
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

function setlevel(clientNum, params)  -- invalid lvl check not working 
    local header = "^3setlevel: ^7"
    local victim = ReadyCommand(clientNum, "setlevel", params)
    
    if victim == 99 then
        return
    end

    if victim ~= -1 then
        local level = tostring(params[2])
		local name = et.Q_CleanStr(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "name" ))
		local guid = string.lower(et.Info_ValueForKey(et.trap_GetUserinfo(victim), "cl_guid" ))
		local colorname = et.gentity_get(victim,"pers.netname")
        
        loadAdmins()
        if global_level_table[level] == nil then
            userprint(clientNum, "chat", header .. "admin level " .. level .. " is invalid")
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
            global_admin_table[guid] = {}
            global_admin_table[guid]["name"] = name
            global_admin_table[guid]["level"] = level
        else 
            if level == "0" then 
                global_admin_table[guid] = nil
            else
                global_admin_table[guid]["level"] = level
            end
        end
        writeAdmins()
        local tmpMsg = header .. colorname .. " ^7was set to level " .. level
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
    else
        userprint(clientNum, "chat", commandhelp["setlevel"])
    end
    return
end

function showbans(clientNum)
    local header = "^3showbans: ^7"
	local counter = 0
	
	for guid in pairs (global_ban_table) do 
		local name = global_ban_table[guid]["name"]
		local made = global_ban_table[guid]["made"]
		local banner = global_ban_table[guid]["banner"]
		local expires = global_ban_table[guid]["expires"]
		local reason = global_ban_table[guid]["reason"]
		local message = ""
		
		counter = counter + 1
		if counter < 10 then
			message = "^5 " .. tostring(counter) .. " "
		else
			message = "^5" .. tostring(counter) .. " "
		end
		message = message .. " ^7" .. name .. string.rep(" ", (20 - string.len(et.Q_CleanStr(name))))
		message = message .. " ^5" .. made
		message = message .. " ^7" .. banner .. string.rep(" ", (20 - string.len(et.Q_CleanStr(banner))))
		message = message .. " ^5" .. expires .. string.rep(" ", (4 - string.len(et.Q_CleanStr(expires))))
		message = message .. " ^7" .. reason
		userprint(clientNum, "print", message .. " \n")
	end
end

function slap(clientNum, params)
    local header = "^3slap: ^7"
    local reason = ""
    local victim = ReadyCommand(clientNum, "slap", params)
    
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
                et.gentity_set(i, "health", -600)
                et.gentity_set(i, "sess.sessionTeam", 3)
                et.ClientUserinfoChanged(i)	
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
	userprint(clientNum, "print", header .. "Accuracy: " .. acc .. " (" .. hits .. "/" .. shots .. ")\n" )
	userprint(clientNum, "print", header .. "Headshots: " .. et.gentity_get(i,"pers.headshots") .. "\n" )
	userprint(clientNum, "print", header .. "Goombas: " .. et.gentity_get(i,"pers.goomba") .. "\n" )
	userprint(clientNum, "print", header .. "Poisons: " .. et.gentity_get(i,"pers.poison") .. "\n" )
	userprint(clientNum, "print", header .. "Revives: " .. et.gentity_get(i,"pers.revives") .. "\n" )
	userprint(clientNum, "print", header .. "Med Packs: " .. et.gentity_get(i,"pers.medPacks") .. "\n" )
	userprint(clientNum, "print", header .. "Ammo Packs: " .. et.gentity_get(i,"pers.ammoPacks") .. "\n" )
	userprint(clientNum, "chat", header .. "player stats has been printed in console")
end

function swap(clientNum)
    local header = "^3swap: ^7"	
	
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		local team = tonumber(et.gentity_get(i,"sess.sessionTeam"))
		if team ~= 3 and team ~= 0 then
			if team == 1 then
			    et.gentity_set(i, "health", -600)
                et.gentity_set(i, "sess.sessionTeam", 2)
                et.ClientUserinfoChanged(i)	
            else
                et.gentity_set(i, "health", -600)
                et.gentity_set(i, "sess.sessionTeam", 1)
                et.ClientUserinfoChanged(i)	
			end
		end
	end
    userprint(-1, "chat", header .. " teams have been swapped")
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
			et.trap_SendConsoleCommand( et.EXEC_INSERT, "timelimit " .. tlimit)		
		else
			userprint(clientNum, "chat", "^3timelimit: ^5" .. tlimit .. " ^7is not a number")
			return
		end
    else
        userprint(clientNum, "chat", commandhelp["timelimit"])
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
	    userprint(-1, "chat", tmpMsg)
	    et.G_LogPrint(et.Q_CleanStr("clientNum " .. clientNum .. ": command " .. tmpMsg) .. "\n")
        local sound = 'sound/multiplayer/hurt_barbwire.wav'
        soundindex = et.G_SoundIndex(sound)
        et.G_Sound(victim,  soundindex)
    else
        userprint(clientNum, "chat", commandhelp["warn"])
    end
    return
end