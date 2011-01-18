--[[
	Name: maprecords.lua
	Purpose: store map records in sqlite db
]]--

RecordsDB = nil
RecordsDBName = et.trap_Cvar_Get("fs_homepath") .. '/' .. et.trap_Cvar_Get("fs_game") .. '/maprecords.db'

local function OpenDB()
	local dbExists = io.open(RecordsDBName)
	if not dbExists then
		CreateRecordsDB()
	else
		io.close(dbExists)
	end

	RecordsDB = sqlite3.open(RecordsDBName)
end

local function CloseDB()
	RecordsDB:close()
end

local recordTypes = {
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

local recordTypeMap = {
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
local currentMapRecords = { 0,0,0,0,0,0,0,0,0 }

--here for scope
local function updateCurrentRecords(ind, value)
	currentMapRecords[ ind ] = value
end

function CreateRecordsDB()
	RecordsDB = sqlite3.open(RecordsDBName)
	RecordsDB:exec[[
	  CREATE TABLE Maps (
		ind			INTEGER PRIMARY KEY,
		name		VARCHAR(50)
	  );

	  CREATE TABLE RecordTypes (
		ind			INTEGER PRIMARY KEY,
		name		VARCHAR(50)
	  );

	  CREATE TABLE Players (
		ind			INTEGER PRIMARY KEY,
		name		VARCHAR(50)
	  );

	  CREATE TABLE Records (
		ind					INTEGER PRIMARY KEY,
		recordTypeId		INTEGER,
		playerId			INTEGER,
		mapId				INTEGER,
		value				INTEGER,
		recordDate			VARCHAR(50)
	  );
	]]

	for ind, typeName in ipairs(recordTypes) do
		RecordsDB:exec('INSERT INTO RecordTypes(name) VALUES("' .. typeName .. '")')
	end
	RecordsDB:close()
end

local function getMapId(mapName)
	for row in RecordsDB:nrows('SELECT * FROM Maps WHERE name="' .. mapName .. '"') do
		return row.ind
	end

	-- insert then recursively call to get the id if it doesn't exist
	RecordsDB:exec('INSERT INTO Maps(name) VALUES("' .. mapName .. '")')
	local ind = getMapId(mapName)
	return ind
end

local function getMapName(mapId)
	for row in RecordsDB:nrows('SELECT * FROM Maps WHERE ind="' .. mapId .. '"') do
		return row.name
	end

	return "unknown"
end

local function getPlayerId(playerName)
	for row in RecordsDB:nrows('SELECT * FROM Players WHERE name="' .. playerName .. '"') do
		return row.ind
	end

	-- insert then recursively call to get the id if it doesn't exist
	RecordsDB:exec('INSERT INTO Players(name) VALUES("' .. playerName .. '")')
	local ind = getPlayerId(playerName)
	return ind
end

local function getPlayerName(playerId)
	for row in RecordsDB:nrows('SELECT * FROM Players WHERE ind="' .. playerId .. '"') do
		return row.name
	end

	return "unknown"
end

local function getCurrentMapRecords(mapId)
	for row in RecordsDB:nrows('SELECT * FROM Records WHERE mapId=' .. mapId) do
		currentMapRecords[ row.ind ] = row.value
	end
end

local function insertNewMapRecord( mapId, playerName, recordType, value )
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

local function checkForMapRecords()
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

local function listMapRecords(clientNum)
	OpenDB()
	mapId = getMapId(et.GetMapName())
	x = 0
	header = "^3----------------------------------------------------------\n^3Record          Value      Date            Player\n^3----------------------------------------------------------\n"

	et.trap_SendServerCommand(clientNum, string.format('print \"%s\"',header))
	for row in RecordsDB:nrows('SELECT * FROM Records WHERE mapId=' .. mapId) do
		et.trap_SendServerCommand(clientNum,string.format('print \"^3%-15s ^5%-10d ^7%-15s %-36s\n\"',recordTypes[row.recordTypeId],row.value,row.recordDate,getPlayerName(row.playerId)))
		x=x+1
	end
	CloseDB()
	if x < 1 then
		et.trap_SendServerCommand(clientNum,string.format('print \"^1No Records for this map yet\n\"'))
	end
end

local function listServerRecords(clientNum)
	OpenDB()
	x = 0
	header = "^3----------------------------------------------------------\n^3Record          Value      Map             Player\n^3----------------------------------------------------------\n"

	et.trap_SendServerCommand(clientNum, string.format('print \"%s\"',header))

	for row in RecordsDB:nrows('SELECT r.mapId, r.playerId, r.recordTypeId, r.value FROM (SELECT mapId, playerId, recordTypeId, MAX(value) AS highest FROM Records GROUP BY recordTypeId) AS x INNER JOIN Records AS r ON r.recordTypeId = x.recordTypeId AND r.Value = x.highest') do
		if row.value then
			et.trap_SendServerCommand(clientNum,string.format('print \"^3%-15s ^5%-10d ^2%-15s %-36s\n\"',recordTypes[row.recordTypeId],row.value,getMapName(row.mapId),getPlayerName(row.playerId)))
			x=x+1
		end
	end

	CloseDB()
	if x < 1 then
		et.trap_SendServerCommand(clientNum,string.format('print \"^1No Records for this server yet\n\"'))
	end
end

function et_LogExit()
	-- open up the database
	OpenDB()

	checkForMapRecords()

	-- close the database
	CloseDB()
end

function et_ClientCommand(clientNum)
	local cmd = et.trap_Argv(0)
	if cmd == "maprecords" then
		listMapRecords(clientNum)
		return 1
	elseif cmd == "serverrecords" then
		listServerRecords(clientNum)
		return 1
	end
end
