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
DoNoRouteList = True	# list all maps that don't have routes initialized

########################################################################
# populate a sorted list of files in nav
########################################################################
Path = "../../../Files/et/nav/"
NavList=os.listdir(os.path.normpath(Path))
NavList.sort()

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
		ReplaceDeprecated()

	if DoFindZeroFacings==True:
		FindZeroFacings()

	if DoToDoList==True:
		ListToDo()

	if DoNoRouteList==True:
		NoRouteList()

########################################################################
# build a wiki formatted waypoint list
########################################################################
def BuildWaypointList():
	f = open(os.path.normpath('results/wplist.txt'), 'w')
	f.write('=== 0.81 released waypoints ===\n\n')

	for fname in NavList:
		ext = os.path.splitext(fname)
		if ext[1]==".way":
			f.write('{0}{1}\n'.format('*',ext[0]))

	f.close()

########################################################################
# find duplicate waypoints
########################################################################
def FindDuplicates():
	NoScriptList = os.listdir('../../../Files/et/incomplete_navs/no_script/')
	WithScriptList = os.listdir('../../../Files/et/incomplete_navs/with_script/')
	PriorityList = os.listdir('../../../Files/et/incomplete_navs/priority_maps/')

	f = open(os.path.normpath('results/dupelist.txt'), 'w')

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
					tempFile = Path+fname+'~~~'
					inp = open(origFile)
					outp = open(tempFile, 'w')

					try:
						s = inp.read()
					except Exception:
						print('FindDebugEnabled: skipping {0}, check encoding for utf-8'.format(fname))
						continue

					outtext = re.sub('Debug = 1', 'Debug = 0', s)
					outp.write(outtext)
					outp.close()
					inp.close()
					os.remove(origFile)
					os.rename(tempFile,origFile)

########################################################################
# search for map scripts that have deprecated utility functions and
# replace them.
# note: make sure all gm files are closed before running this one
########################################################################
def ReplaceDeprecated():
	for fname in NavList:
		if os.path.isfile(Path+fname):
			ext = os.path.splitext(fname)
			if ext[1]==".gm":
				if ext[0].find('_goals') == -1:
					origFile = Path+fname
					tempFile = Path+fname+'~~~'
					inp = open(origFile)
					outp = open(tempFile, 'w')

					try:
						s = inp.read()
					except Exception:
						print('ReplaceDeprecated: skipping {0}, check encoding for utf-8'.format(fname))
						continue

					outtext = re.sub('ETUtil.DisableGoal', 'Util.DisableGoal', s)
					outtext = re.sub('ETUtil.EnableGoal', 'Util.EnableGoal', s)
					outtext = re.sub('ETUtil.RandomSpawn', 'Util.RandomSpawn', s)
					outtext = re.sub('ETUtil.ChangeSpawn', 'Util.ChangeSpawn', s)
					outp.write(outtext)
					outp.close()
					inp.close()
					os.remove(origFile)
					os.rename(tempFile,origFile)

########################################################################
# find camp goals with zero facings
########################################################################
def FindZeroFacings():
	out = open(os.path.normpath('results/zerofacings.txt'), 'w')

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
	out = open(os.path.normpath('results/todolist.txt'), 'w')
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
	out = open(os.path.normpath('results/noroutelist.txt'), 'w')
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
# run it
########################################################################
Main()

