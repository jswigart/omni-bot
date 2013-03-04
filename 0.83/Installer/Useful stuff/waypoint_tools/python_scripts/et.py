########################################################################
# Name: et.py
# Purpose: perform some common tasks for the waypoint package
# Requirements: latest version of Python installed
# Usage: python et.py in console
#
# Output: various reports in the results folder
########################################################################

import os, re

########################################################################
# config options
########################################################################
DoWaypointList = True		# build a wiki formatted wp list
DoDebugEnabled = True		# replace Debug = 1 with Debug = 0
DoFindDuplicates = True		# search all nav folders for dupes
DoReplaceDeprecated = True	# replace deprecated utility functions
DoFindZeroFacings = True	# find all camp type goals with no facing
DoToDoList = True		# find TODOs in map scripts
DoNoRouteList = True		# list all maps that don't have routes initialized
DoOldScriptGoalList = True	# list maps with unsupported script goal setups

########################################################################
# populate a sorted list of files in nav
########################################################################
Path = "../../../Files/et/nav/"
NavList=os.listdir(os.path.normpath(Path))
NavList.sort()

########################################################################
# populate lists for incomplete maps
########################################################################
NoScriptPath = ('../../../Files/et/incomplete_navs/no_script/')
NoScriptList = os.listdir(os.path.normpath(NoScriptPath))
WithScriptPath = ('../../../Files/et/incomplete_navs/with_script/')
WithScriptList = os.listdir(os.path.normpath(WithScriptPath))
PriorityPath = ('../../../Files/et/incomplete_navs/priority_maps/')
PriorityList = os.listdir(os.path.normpath(PriorityPath))

########################################################################
# Main
########################################################################
def Main():
	if DoWaypointList==True:
		BuildWaypointList()

	if DoDebugEnabled==True:
		FindDebugEnabled()

	if DoFindDuplicates==True:
		FindDuplicates()

	if DoReplaceDeprecated==True:
		FindDeprecated()

	if DoFindZeroFacings==True:
		FindZeroFacings()

	if DoToDoList==True:
		ListToDo()

	if DoNoRouteList==True:
		NoRouteList()

	if DoOldScriptGoalList==True:
		OldScriptGoalList()

########################################################################
# build a wiki formatted waypoint list
########################################################################
def BuildWaypointList():
	f = open(os.path.normpath('results/et_wplist.txt'), 'w')
	f.write('=== 0.82 released waypoints ===\n\n')

	for fname in NavList:
		ext = os.path.splitext(fname)
		if ext[1]==".way":
			f.write('{0}{1}\n'.format('*',ext[0]))

	f.close()

########################################################################
# find duplicate waypoints
########################################################################
def FindDuplicates():
	f = open(os.path.normpath('results/et_dupelist.txt'), 'w')

	# iterate no_script
	for fname in NoScriptList:
		if fname == '.svn':
			continue

		if fname in WithScriptList:
			f.write('{0} exists in no_script and with_script\n'.format(fname))

		if fname in PriorityList:
			f.write('{0} exists in no_script and priority_maps\n'.format(fname))

		if fname in NavList:
			f.write('{0} exists in no_script and nav\n'.format(fname))

	# iterate with_script
	for fname in WithScriptList:
		if fname == '.svn':
			continue

		if fname in PriorityList:
			f.write('{0} exists in with_script and priority_maps\n'.format(fname))

		if fname in NavList:
			f.write('{0} exists in with_script and nav\n'.format(fname))

	# iterate priority_maps
	for fname in PriorityList:
		if fname == '.svn':
			continue

		if fname in NavList:
			f.write('{0} exists in priority_maps and nav\n'.format(fname))

	f.write('### end dup list')
	f.close()

########################################################################
# search for map scripts that have debug enabled and disable it
# note: make sure all gm files are closed before running this one
########################################################################
def FindDebugEnabled():
	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') == -1:
					origFile = Path+fname
					inp = open(origFile, 'r')

					try:
						s = inp.read()
					except Exception:
						print('FindDebugEnabled: skipping {0}, check encoding for utf-8'.format(fname))
						continue

					if ( s.find('Debug = 1') > 0 ):
						tempFile = Path+fname+'~~~'
						outp = open(tempFile, 'w')
						outtext = re.sub('Debug = 1', 'Debug = 0', s)
						outp.write(outtext)
						outp.close()
						os.remove(origFile)
						os.rename(tempFile,origFile)

					inp.close()

########################################################################
# search for map scripts that have deprecated utility functions and
# replace them.
# note: make sure all gm files are closed before running this one
########################################################################
_ReplacedDeprecated = False

def ReplaceDeprecated(fname, stream, oldFunc, newFunc):
	global _ReplacedDeprecated
	tempFile = Path+fname+'~~~'
	outp = open(tempFile, 'w')
	outtext = re.sub(oldFunc, newFunc, stream)
	outp.write(outtext)
	outp.close()

	_ReplacedDeprecated = True

	# read from new file to catch other deprecated funcs
	inp = open(tempFile, 'r')
	s = inp.read()
	inp.close()
	return s

def FindDeprecated():
	global _ReplacedDeprecated
	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') == -1:
					origFile = Path+fname
					tempFile = Path+fname+'~~~'
					inp = open(origFile, 'r')

					try:
						s = inp.read()
					except Exception:
						print('ReplaceDeprecated: skipping {0}, check encoding for utf-8'.format(fname))
						inp.close()
						continue

					if ( s.find('ETUtil.DisableGoal') > 0 ):
						s = ReplaceDeprecated(fname,s,'ETUtil.DisableGoal','Util.DisableGoal')

					if ( s.find('ETUtil.EnableGoal') > 0 ):
						s = ReplaceDeprecated(fname,s,'ETUtil.EnableGoal','Util.EnableGoal')

					if ( s.find('ETUtil.RandomSpawn') > 0 ):
						s = ReplaceDeprecated(fname,s,'ETUtil.RandomSpawn','Util.RandomSpawn')

					if ( s.find('ETUtil.ChangeSpawn') > 0 ):
						s = ReplaceDeprecated(fname,s,'ETUtil.ChangeSpawn','Util.ChangeSpawn')

					inp.close()

					if ( _ReplacedDeprecated == True ):
						os.remove(origFile)
						os.rename(tempFile,origFile)

					_ReplacedDeprecated = False


########################################################################
# find camp goals with zero facings
########################################################################
def FindZeroFacings():
	out = open(os.path.normpath('results/et_zerofacings.txt'), 'w')

	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') != -1:
					f = open(Path+fname, 'r')
					lines = f.readlines()
					findingGoal = True
					findingAimVectors = False
					findingAimVectorsEnd = False
					aimVecLine = 0
					currentGoal = ''
					lineNum = 0
					for line in lines:
						lineNum += 1
						if findingGoal==True:
							g = line.find('ATTACK')
							if g == -1:
								g = line.find('DEFEND')
								if g == -1:
									g = line.find('CAMP')
									if g == -1:
										g = line.find('SNIPE')
							i = line.find('=')
							if g > -1 and i > g:
								currentGoal = line.split(' ')
								currentGoal = currentGoal[0].strip('\t')
								findingGoal = False
								findingAimVectors = True
						if findingAimVectors==True:
							if line.find('AimVectors') > -1:
								aimVecLine = lineNum
								findingAimVectors = False
								findingAimVectorsEnd = True
						if findingAimVectorsEnd==True:
							if line.find('},') > -1:
								if lineNum - aimVecLine < 3:
									out.write('{0} : {1} has no aim vectors\n'.format(fname, currentGoal))
								findingAimVectorsEnd = False
								findingGoal = True

					f.close()

	out.write('### end zero facing list')
	out.close()

########################################################################
# list todos in map scripts (nav directory only)
########################################################################
def ListToDo():
	out = open(os.path.normpath('results/et_todolist.txt'), 'w')
	out.write('List of map scripts in nav with TODOs\n\n')

	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') == -1:
					f = open(Path+fname, 'r')

					try:
						lines = f.readlines()
					except Exception:
						print('ListToDo: skipping {0}, check encoding for utf-8'.format(fname))
						continue

					todolist = []
					lineNum = 0
					for line in lines:
						lineNum += 1
						g = line.find('TODO')
						if g == -1:
							g = line.find('todo')
						if g != -1:
							todolist.append('TODO in line {0}\n'.format(lineNum))
					if len(todolist) > 0:
						out.write('TODOs in {0}:\n'.format(fname))
						for todo in todolist:
							out.write(todo)
						out.write('\n')
					f.close()
	out.close()

########################################################################
# list maps that don't call Util.Routes (except deathmatch, snipe and panzer maps)
########################################################################
def NoRouteList():
	out = open(os.path.normpath('results/et_noroutelist.txt'), 'w')
	out.write('List of map scripts in nav without routes initialized\n\n')

	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') == -1:
					f = open(Path+fname, 'r')

					try:
						contents = f.read()
					except Exception:
						print('NoRouteList: skipping {0}, check encoding for utf-8'.format(fname))
						continue

					if contents.find('DeathMatch') < 0 and contents.find('SnipeMap') < 0 and contents.find('PanzerMap') < 0 and contents.find('Util.Routes') < 0:
						out.write('{0} has no routes initialized\n'.format(ext[0]))
					f.close()
	out.close()

########################################################################
# find maps with old script goals set up
########################################################################
def SearchForOldScriptGoal(contents):
	str_list = []

	# doh, MountVehicle auto activated based on availability of mover goal ...
	oops = re.findall('\s*SetAvailableMapGoals\(.*"MOVER_.*', contents)
	if len(oops) > 0:
		str_list.append('sets availability for mover goal - goal_mountvehicle\n')

	if contents.find("Target =") > 0:
		str_list.append('has a Target table - goal_grenadetarget\n')

	if contents.find("MountVehicle =") > 0:
		str_list.append('has a MountVehicle table - goal_mountvehicle\n')

	if contents.find("RideVehicle =") > 0:
		str_list.append('has a RideVehicle table - goal_ridevehicle\n')

	if contents.find("RideTram =") > 0:
		str_list.append('has a RideTram table - goal_ridetram\n')

	if contents.find("EscortVehicle =") > 0:
		str_list.append('has an EscortVehicle table - goal_escortvehicle\n')

	#if contents.find("Switches =") > 0:
	#	str_list.append('has a Switches table - check for paththrough\n')

	return str_list

def CheckForOldScriptGoal(path, fname, out):
	if os.path.isfile(path):
		ext = os.path.splitext(path)

		if ext[1]==".gm":
			if ext[0].find('_goals') == -1:
				f = open(path, 'r')

				try:
					contents = f.read()
				except Exception:
					print('OldScriptGoalList: skipping {0}, check encoding for utf-8'.format(fname))
					f.close()
					return

				res =  SearchForOldScriptGoal(contents)
				if ( len(res) > 0 ):
					out.write('---- {0} ----\n'.format(fname))
					for item in res:
						out.write(item)

					out.write('\n')

				f.close()

def OldScriptGoalList():
	out = open(os.path.normpath('results/et_oldscriptgoallist.txt'), 'w')
	out.write('List of map scripts with old script goals\n\n')

	out.write('---------- With Script ----------\n\n')
	for fname in WithScriptList:
		CheckForOldScriptGoal(WithScriptPath + fname,fname,out)

	out.write('\n\n---------- Priority Maps ----------\n\n')
	for fname in PriorityList:
		CheckForOldScriptGoal(PriorityPath + fname,fname,out)

	out.write('\n\n---------- Nav Folder ---------\n\n')
	for fname in NavList:
		CheckForOldScriptGoal(Path + fname,fname,out)

	out.close()

########################################################################
# run it
########################################################################
Main()


