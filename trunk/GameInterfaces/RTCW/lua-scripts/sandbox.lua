--[[
	Name: sandbox.lua
	Purpose: general testing script
]]--

-- load constants
require('rtcw_constants')

-- ---------------------------------------------------------------------
-- Sandbox functions
-- ---------------------------------------------------------------------

-- centerprint to victim the killers health (if victim is not a bot)
local function killerHealth( victim, killer )
	if not et.IsBot(victim) and victim ~= killer and killer < MAX_CLIENTS then
		et.trap_SendServerCommand(victim, string.format('cp \"%s\"',string.format("%s ^2had %d health left",et.gentity_get(killer,"pers.netname"), et.gentity_get(killer,"health"))))
	end
end

------------------------------------------------------------------------
-- welcome message for clients that are not bots
local function serverGreeting( clientNum, firstTime, isBot, message )
	if isBot == 0 and firstTime == 1 then
		local userInfo = et.trap_GetUserinfo(clientNum)
		et.trap_SendServerCommand(clientNum, string.format('chat \" %s %s\"', message, et.Info_ValueForKey( userInfo,"name")))
	end
end

------------------------------------------------------------------------
-- disallow a connection the proper way by returning a string (reason)
local function canConnect( clientNum, isBot )
	-- uncomment this block for test
	--if isBot == 0 then
	--	return "banned mofo"
	--end

	return nil
end

------------------------------------------------------------------------
-- simple revenge announce
local revengeFlags = { lastKiller = -1, lastVictim = -1 }
local revengeArray = {}

local function initRevengeArray( clientNum )
	revengeArray[ clientNum ] = revengeFlags
end

local function updateRevenge( killer, victim )
	if killer == victim or killer > MAX_CLIENTS-1 then
		return
	end

	-- if its a revenge kill, then print it ...
	if revengeArray[ killer ].lastKiller == victim then
		et.trap_SendServerCommand(-1, string.format('print \"%s ^2took revenge from %s\n\"', et.gentity_get(killer,"pers.netname"), et.gentity_get(victim,"pers.netname")))
	end

	-- update the array
	revengeArray[ killer ].lastVictim = victim
	revengeArray[ victim ].lastKiller = killer
end

-- clear lastKiller when that client disconnects
local function clearLastKiller( clientNum )
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
		if revengeArray[ i ] and revengeArray[ i ].lastKiller == clientNum then
			revengeArray[ i ].lastKiller = -1
		end
	end
end

------------------------------------------------------------------------
-- simple addevent example.
local function addGeneralSound(clientNum, snd)
	if et.gentity_get(clientNum,"pers.connected") == CON_CONNECTED then
		et.G_AddEvent(clientNum, EV_GENERAL_SOUND, et.G_SoundIndex(snd) )
	end
end

------------------------------------------------------------------------
-- add a powerup
local function addPowerup(clientNum, powerup, ms)
	if et.gentity_get(clientNum,"pers.connected") == CON_CONNECTED then
		et.gentity_set(clientNum,"ps.powerups", powerup, ms)
	end
end


------------------------------------------------------------------------

-- ---------------------------------------------------------------------
-- All of the hooked functions start here (must be global)
-- ---------------------------------------------------------------------

function et_InitGame( levelTime, randomSeed, restart )
end

function et_ShutdownGame( restart )
end

function et_LogExit()
end

function et_RunFrame( levelTime )
end

function et_ClientConnect( clientNum, firstTime, isBot )
	local reason = canConnect(clientNum, isBot)
	if reason ~= nil then return reason end

	serverGreeting(clientNum, firstTime, isBot, "Welcome to the server")
	return nil
end

function et_ClientDisconnect( clientNum )
	--clear this slot from the revenge array
	revengeArray[ clientNum ] = nil

	-- and remove it from clients lastKiller flag
	clearLastKiller(clientNum)
end

function et_ClientBegin( clientNum )
	-- initialize the revenge array for this slot
	initRevengeArray(clientNum)
end

function et_ClientUserinfoChanged( clientNum )
end

function et_ClientSpawn( clientNum, revived )
end

function et_ClientCommand( clientNum, command )
end

function et_ConsoleCommand( command )
	local cmd = et.trap_Argv(0)
	if cmd == "medsnd" then
		addGeneralSound(et.trap_Argv(1), 'sound/multiplayer/allies/a-medic2.wav')
		return 1
	elseif cmd == "regen" then
		-- regen health for 10 seconds
		addPowerup(et.trap_Argv(1), PW_REGEN, et.GetLevelTime() + 10000)
		return 1
	end
end

function et_Print( text )
end

function et_Obituary( victim, killer, meansOfDeath )
	-- send killer health to the victim in centerprint
	killerHealth( victim, killer )

	-- update the revenge array
	updateRevenge( killer, victim )
end
