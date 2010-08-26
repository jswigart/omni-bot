//
// Copyright (c) 2009 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef NAVMESHTESTERTOOL_H
#define NAVMESHTESTERTOOL_H

#include "Sample.h"
#include "DetourNavMesh.h"

class NavMeshTesterTool : public SampleTool
{
	Sample* m_sample;
	
	dtNavMesh* m_navMesh;

	dtQueryFilter m_filter;

	enum ToolMode
	{
		TOOLMODE_PATHFIND_ITER,
		TOOLMODE_PATHFIND_STRAIGHT,
		TOOLMODE_RAYCAST,
		TOOLMODE_DISTANCE_TO_WALL,
		TOOLMODE_FIND_POLYS_AROUND,
	};
	
	ToolMode m_toolMode;
	
	static const int MAX_POLYS = 256;
	static const int MAX_SMOOTH = 2048;
	
	dtPolyRef m_startRef;
	dtPolyRef m_endRef;
	dtPolyRef m_polys[MAX_POLYS];
	dtPolyRef m_parent[MAX_POLYS];
	int m_npolys;
	float m_straightPath[MAX_POLYS*3];
	unsigned char m_straightPathFlags[MAX_POLYS];
	dtPolyRef m_straightPathPolys[MAX_POLYS];
	int m_nstraightPath;
	float m_polyPickExt[3];
	float m_smoothPath[MAX_SMOOTH*3];
	int m_nsmoothPath;
	
	float m_spos[3];
	float m_epos[3];
	float m_hitPos[3];
	float m_hitNormal[3];
	float m_distanceToWall;
	bool m_sposSet;
	bool m_eposSet;
	
public:
	NavMeshTesterTool();
	~NavMeshTesterTool();

	virtual int type() { return TOOL_NAVMESH_TESTER; }
	virtual void init(Sample* sample);
	virtual void reset();
	virtual void handleMenu();
	virtual void handleClick(const float* p, bool shift);
	virtual void handleRender();
	virtual void handleRenderOverlay(double* proj, double* model, int* view);

	void recalc();
	void drawAgent(const float* pos, float r, float h, float c, const unsigned int col);
};

#endif // NAVMESHTESTERTOOL_H