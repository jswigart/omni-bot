--[[
	Name: stats.lua
	Purpose: store player stats in an sqlite3 database and provide some query functions to be used with commands ( like !maprecords )
]]--

StatsDB = nil
StatsDBName = "stats.db"

function OpenDatabase()
	StatsDB = sqlite3.open(StatsDBName)
end

function CloseDatabase()
	StatsDB:close()
	StatsDB = nil
end

function CreateStatsDB()
	if StatsDB == nil then
		OpenDatabase()
	end

	StatsDB:exec[[
	  CREATE TABLE roundStats (
		id			INTEGER PRIMARY KEY,
		roundId			INTEGER,
		playerId		INTEGER,
		playerKills		INTEGER,
		playerDeaths		INTEGER,
		playerSuicides		INTEGER,
		playerTeamKills		INTEGER,
		playerKillSpree		INTEGER,
		playerDeathSpree	INTEGER,
		playerDamageGiven	INTEGER,
		playerDamageReceived	INTEGER,
		playerTeamDamage	INTEGER,
		playerHits		INTEGER,
		playerShots		INTEGER,
		playerHeadshots		INTEGER,
		playerGoombas		INTEGER,
		playerPoisons		INTEGER,
		playerRevives		INTEGER,
		playerMedPacks		INTEGER,
		playerAmmoPacks		INTEGER
	  );
	]]
end

function AddRecord( i )
	if StatsDB == nil then
		et.G_Print("AddPlayerStats: StatsDB is nil")
		return
	end

	assert (StatsDB:exec('insert into roundStats (roundId,playerId,playerKills,playerDeaths,playerSuicides,playerTeamKills,playerKillSpree,playerDeathSpree,playerDamageGiven,playerDamageReceived,playerTeamDamage,playerHits,playerShots,playerHeadshots,playerGoombas,playerPoisons,playerRevives,playerMedPacks,playerAmmoPacks) values('.. i ..
							', '.. i ..
							', '.. et.gentity_get(i,'pers.kills') ..
							', '.. et.gentity_get(i,'ps.persistant[PERS_KILLED]') ..
							', '.. et.gentity_get(i,'pers.suicides') ..
							', '.. et.gentity_get(i,'pers.teamKills') ..
							', '.. et.gentity_get(i,'pers.killSpreeMax') ..
							', '.. et.gentity_get(i,'pers.deathSpreeMax') ..
							', '.. et.gentity_get(i,'pers.damage_given') ..
							', '.. et.gentity_get(i,'pers.damage_received') ..
							', '.. et.gentity_get(i,'pers.team_damage') ..
							', '.. et.gentity_get(i,'pers.acc_hits') ..
							', '.. et.gentity_get(i,'pers.acc_shots') ..
							', '.. et.gentity_get(i,'pers.headshots') ..
							', '.. et.gentity_get(i,'pers.goomba') ..
							', '.. et.gentity_get(i,'pers.poison') ..
							', '.. et.gentity_get(i,'pers.revives') ..
							', '.. et.gentity_get(i,'pers.medPacks') ..
							', '.. et.gentity_get(i,'pers.ammoPacks') .. ')' )
		)
end

function et_LogExit()
	-- open up the database
	CreateStatsDB()

	-- add the records
	for i=0, tonumber(et.trap_Cvar_Get("sv_maxclients"))-1, 1 do
                playerName = et.gentity_get(i,"pers.netname")
                if playerName ~= "" and et.gentity_get(i,"pers.connected") == 2 then
			AddRecord(i)
		end
	end

	-- close the database
	CloseDatabase()
end