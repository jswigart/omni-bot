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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "InputGeom.h"
#include "Sample.h"
#include "Sample_SoloMeshTiled.h"
#include "Recast.h"
#include "RecastTimer.h"
#include "RecastDebugDraw.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourDebugDraw.h"
#include "NavMeshTesterTool.h"
#include "OffMeshConnectionTool.h"
#include "ConvexVolumeTool.h"

#ifdef WIN32
#	define snprintf _snprintf
#endif


class TileHighlightTool : public SampleTool
{
	Sample_SoloMeshTiled* m_sample;
	float m_hitPos[3];
	bool m_hitPosSet;
	float m_agentRadius;
	
public:
	
	TileHighlightTool() :
		m_sample(0),
		m_hitPosSet(false),
		m_agentRadius(0)
	{
	}
	
	virtual ~TileHighlightTool()
	{
		if (m_sample)
			m_sample->setHighlightedTile(0);
	}
	
	virtual int type() { return TOOL_TILE_HIGHLIGHT; }
	
	virtual void init(Sample* sample)
	{
		m_sample = (Sample_SoloMeshTiled*)sample; 
	}
	
	virtual void reset() {}
	
	virtual void handleMenu()
	{
		imguiValue("Click LMB to highlight a tile.");
	}
	
	virtual void handleClick(const float* p, bool shift)
	{
		m_hitPosSet = true;
		vcopy(m_hitPos,p);
		if (m_sample)
			m_sample->setHighlightedTile(m_hitPos);
	}
	
	virtual void handleRender()
	{
		if (m_hitPosSet)
		{
			const float s = m_sample->getAgentRadius();
			glColor4ub(0,0,0,128);
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex3f(m_hitPos[0]-s,m_hitPos[1]+0.1f,m_hitPos[2]);
			glVertex3f(m_hitPos[0]+s,m_hitPos[1]+0.1f,m_hitPos[2]);
			glVertex3f(m_hitPos[0],m_hitPos[1]-s+0.1f,m_hitPos[2]);
			glVertex3f(m_hitPos[0],m_hitPos[1]+s+0.1f,m_hitPos[2]);
			glVertex3f(m_hitPos[0],m_hitPos[1]+0.1f,m_hitPos[2]-s);
			glVertex3f(m_hitPos[0],m_hitPos[1]+0.1f,m_hitPos[2]+s);
			glEnd();
			glLineWidth(1.0f);
		}
	}
	
	virtual void handleRenderOverlay(double* proj, double* model, int* view)
	{
	}
};


Sample_SoloMeshTiled::Sample_SoloMeshTiled() :
	m_measurePerTileTimings(false),
	m_keepInterResults(false),
	m_tileSize(64),
	m_pmesh(0),
	m_dmesh(0),
	m_tileSet(0),
	m_statPolysPerTileSamples(0),
	m_statTimePerTileSamples(0),
	m_highLightedTileX(-1),
	m_highLightedTileY(-1),
	m_drawMode(DRAWMODE_NAVMESH)
{
	setTool(new NavMeshTesterTool);
}

Sample_SoloMeshTiled::~Sample_SoloMeshTiled()
{
	cleanup();
}

void Sample_SoloMeshTiled::cleanup()
{
	delete m_tileSet;
	m_tileSet = 0;
	delete m_pmesh;
	m_pmesh = 0;
	delete m_dmesh;
	m_dmesh = 0;
	delete m_navMesh;
	m_navMesh = 0;
	m_statTimePerTileSamples = 0;
	m_statPolysPerTileSamples = 0;
}

void Sample_SoloMeshTiled::handleSettings()
{
	Sample::handleCommonSettings();
	
	imguiLabel("Tiling");
	imguiSlider("TileSize", &m_tileSize, 16.0f, 1024.0f, 16.0f);

	if (m_geom)
	{
		const float* bmin = m_geom->getMeshBoundsMin();
		const float* bmax = m_geom->getMeshBoundsMax();
		char text[64];
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
		const int ts = (int)m_tileSize;
		const int tw = (gw + ts-1) / ts;
		const int th = (gh + ts-1) / ts;
		snprintf(text, 64, "Tiles  %d x %d", tw, th);
		imguiValue(text);
	}
	
	imguiSeparator();
	if (imguiCheck("Keep Itermediate Results", m_keepInterResults))
		m_keepInterResults = !m_keepInterResults;
	if (imguiCheck("Measure Per Tile Timings", m_measurePerTileTimings))
		m_measurePerTileTimings = !m_measurePerTileTimings;
	
	imguiSeparator();
}

void Sample_SoloMeshTiled::handleTools()
{
	int type = !m_tool ? TOOL_NONE : m_tool->type();
	
	if (imguiCheck("Test Navmesh", type == TOOL_NAVMESH_TESTER))
	{
		setTool(new NavMeshTesterTool);
	}
	if (imguiCheck("Create Off-Mesh Links", type == TOOL_OFFMESH_CONNECTION))
	{
		setTool(new OffMeshConnectionTool);
	}
	if (imguiCheck("Create Convex Volumes", type == TOOL_CONVEX_VOLUME))
	{
		setTool(new ConvexVolumeTool);
	}
	if (imguiCheck("Highlight Tile", type == TOOL_TILE_HIGHLIGHT))
	{
		setTool(new TileHighlightTool);
	}
	
	imguiSeparator();
	
	if (m_tool)
		m_tool->handleMenu();
}

void Sample_SoloMeshTiled::handleDebugMode()
{
	// Check which modes are valid.
	bool valid[MAX_DRAWMODE];
	for (int i = 0; i < MAX_DRAWMODE; ++i)
		valid[i] = false;

	bool hasChf = false;
	bool hasSolid = false;
	bool hasCset = false;
	bool hasPmesh = false;
	bool hasDmesh = false;
	if (m_tileSet)
	{
		for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
		{
			if (m_tileSet->tiles[i].solid) hasSolid = true;
			if (m_tileSet->tiles[i].chf) hasChf = true;
			if (m_tileSet->tiles[i].cset) hasCset = true;
			if (m_tileSet->tiles[i].pmesh) hasPmesh = true;
			if (m_tileSet->tiles[i].dmesh) hasDmesh = true;
		}
	}
	if (m_pmesh) hasPmesh = true;
	if (m_dmesh) hasDmesh = true;
	
	if (m_geom)
	{
		valid[DRAWMODE_NAVMESH] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_TRANS] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_BVTREE] = m_navMesh != 0;
		valid[DRAWMODE_NAVMESH_INVIS] = m_navMesh != 0;
		valid[DRAWMODE_MESH] = true;
		valid[DRAWMODE_VOXELS] = hasSolid;
		valid[DRAWMODE_VOXELS_WALKABLE] = hasSolid;
		valid[DRAWMODE_COMPACT] = hasChf;
		valid[DRAWMODE_COMPACT_DISTANCE] = hasChf;
		valid[DRAWMODE_COMPACT_REGIONS] = hasChf;
		valid[DRAWMODE_REGION_CONNECTIONS] = hasCset;
		valid[DRAWMODE_RAW_CONTOURS] = hasCset;
		valid[DRAWMODE_BOTH_CONTOURS] = hasCset;
		valid[DRAWMODE_CONTOURS] = hasCset;
		valid[DRAWMODE_POLYMESH] = hasPmesh;
		valid[DRAWMODE_POLYMESH_DETAIL] = hasDmesh;
	}
	
	int unavail = 0;
	for (int i = 0; i < MAX_DRAWMODE; ++i)
		if (!valid[i]) unavail++;
	
	if (unavail == MAX_DRAWMODE)
		return;
	
	imguiLabel("Draw");
	if (imguiCheck("Input Mesh", m_drawMode == DRAWMODE_MESH, valid[DRAWMODE_MESH]))
		m_drawMode = DRAWMODE_MESH;
	if (imguiCheck("Navmesh", m_drawMode == DRAWMODE_NAVMESH, valid[DRAWMODE_NAVMESH]))
		m_drawMode = DRAWMODE_NAVMESH;
	if (imguiCheck("Navmesh Invis", m_drawMode == DRAWMODE_NAVMESH_INVIS, valid[DRAWMODE_NAVMESH_INVIS]))
		m_drawMode = DRAWMODE_NAVMESH_INVIS;
	if (imguiCheck("Navmesh Trans", m_drawMode == DRAWMODE_NAVMESH_TRANS, valid[DRAWMODE_NAVMESH_TRANS]))
		m_drawMode = DRAWMODE_NAVMESH_TRANS;
	if (imguiCheck("Navmesh BVTree", m_drawMode == DRAWMODE_NAVMESH_BVTREE, valid[DRAWMODE_NAVMESH_BVTREE]))
		m_drawMode = DRAWMODE_NAVMESH_BVTREE;
	if (imguiCheck("Voxels", m_drawMode == DRAWMODE_VOXELS, valid[DRAWMODE_VOXELS]))
		m_drawMode = DRAWMODE_VOXELS;
	if (imguiCheck("Walkable Voxels", m_drawMode == DRAWMODE_VOXELS_WALKABLE, valid[DRAWMODE_VOXELS_WALKABLE]))
		m_drawMode = DRAWMODE_VOXELS_WALKABLE;
	if (imguiCheck("Compact", m_drawMode == DRAWMODE_COMPACT, valid[DRAWMODE_COMPACT]))
		m_drawMode = DRAWMODE_COMPACT;
	if (imguiCheck("Compact Distance", m_drawMode == DRAWMODE_COMPACT_DISTANCE, valid[DRAWMODE_COMPACT_DISTANCE]))
		m_drawMode = DRAWMODE_COMPACT_DISTANCE;
	if (imguiCheck("Compact Regions", m_drawMode == DRAWMODE_COMPACT_REGIONS, valid[DRAWMODE_COMPACT_REGIONS]))
		m_drawMode = DRAWMODE_COMPACT_REGIONS;
	if (imguiCheck("Region Connections", m_drawMode == DRAWMODE_REGION_CONNECTIONS, valid[DRAWMODE_REGION_CONNECTIONS]))
		m_drawMode = DRAWMODE_REGION_CONNECTIONS;
	if (imguiCheck("Raw Contours", m_drawMode == DRAWMODE_RAW_CONTOURS, valid[DRAWMODE_RAW_CONTOURS]))
		m_drawMode = DRAWMODE_RAW_CONTOURS;
	if (imguiCheck("Both Contours", m_drawMode == DRAWMODE_BOTH_CONTOURS, valid[DRAWMODE_BOTH_CONTOURS]))
		m_drawMode = DRAWMODE_BOTH_CONTOURS;
	if (imguiCheck("Contours", m_drawMode == DRAWMODE_CONTOURS, valid[DRAWMODE_CONTOURS]))
		m_drawMode = DRAWMODE_CONTOURS;
	if (imguiCheck("Poly Mesh", m_drawMode == DRAWMODE_POLYMESH, valid[DRAWMODE_POLYMESH]))
		m_drawMode = DRAWMODE_POLYMESH;
	if (imguiCheck("Poly Mesh Detail", m_drawMode == DRAWMODE_POLYMESH_DETAIL, valid[DRAWMODE_POLYMESH_DETAIL]))
		m_drawMode = DRAWMODE_POLYMESH_DETAIL;
	
	if (unavail)
	{
		imguiValue("Tick 'Keep Itermediate Results'");
		imguiValue("to see more debug mode options.");
	}
}

void Sample_SoloMeshTiled::handleRender()
{
	if (!m_geom || !m_geom->getMesh())
		return;
	
	DebugDrawGL dd;
	
	glEnable(GL_FOG);
	glDepthMask(GL_TRUE);
	
	if (m_drawMode == DRAWMODE_MESH)
	{
		// Draw mesh
		duDebugDrawTriMeshSlope(&dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
								m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(),
								m_agentMaxSlope);
		m_geom->drawOffMeshConnections(&dd);
	}
	else if (m_drawMode != DRAWMODE_NAVMESH_TRANS)
	{
		// Draw mesh
		duDebugDrawTriMesh(&dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
						   m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(), 0);
		m_geom->drawOffMeshConnections(&dd);
	}
	
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	
	// Draw bounds
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	duDebugDrawBoxWire(&dd, bmin[0],bmin[1],bmin[2], bmax[0],bmax[1],bmax[2], duRGBA(255,255,255,128), 1.0f);
	
	// Tiling grid.
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int tw = (gw + (int)m_tileSize-1) / (int)m_tileSize;
	const int th = (gh + (int)m_tileSize-1) / (int)m_tileSize;
	const float s = m_tileSize*m_cellSize;
	duDebugDrawGridXZ(&dd, bmin[0],bmin[1],bmin[2], tw,th, s, duRGBA(0,0,0,64), 1.0f);
	
	if (m_navMesh &&
		(m_drawMode == DRAWMODE_NAVMESH ||
		 m_drawMode == DRAWMODE_NAVMESH_TRANS ||
		 m_drawMode == DRAWMODE_NAVMESH_BVTREE ||
		 m_drawMode == DRAWMODE_NAVMESH_INVIS))
	{
		if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
			duDebugDrawNavMesh(&dd, m_navMesh, m_navMeshDrawFlags);
		if (m_drawMode == DRAWMODE_NAVMESH_BVTREE)
			duDebugDrawNavMeshBVTree(&dd, m_navMesh);
	}
	
	glDepthMask(GL_TRUE);
	
	if (m_tileSet)
	{
		if (m_drawMode == DRAWMODE_COMPACT)
		{
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].chf && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawCompactHeightfieldSolid(&dd, *m_tileSet->tiles[i].chf);
			}
		}
		
		if (m_drawMode == DRAWMODE_COMPACT_DISTANCE)
		{
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].chf && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawCompactHeightfieldDistance(&dd, *m_tileSet->tiles[i].chf);
			}
		}
		if (m_drawMode == DRAWMODE_COMPACT_REGIONS)
		{
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].chf && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawCompactHeightfieldRegions(&dd, *m_tileSet->tiles[i].chf);
			}
		}
			
		if (m_drawMode == DRAWMODE_VOXELS)
		{
			glEnable(GL_FOG);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].solid && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawHeightfieldSolid(&dd, *m_tileSet->tiles[i].solid);
			}
			glDisable(GL_FOG);
		}
		if (m_drawMode == DRAWMODE_VOXELS_WALKABLE)
		{
			glEnable(GL_FOG);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].solid && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawHeightfieldWalkable(&dd, *m_tileSet->tiles[i].solid);
			}
			glDisable(GL_FOG);
		}
		if (m_drawMode == DRAWMODE_RAW_CONTOURS)
		{
			glDepthMask(GL_FALSE);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].cset && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawRawContours(&dd, *m_tileSet->tiles[i].cset);
			}
			glDepthMask(GL_TRUE);
		}
		if (m_drawMode == DRAWMODE_BOTH_CONTOURS)
		{
			glDepthMask(GL_FALSE);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].cset && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
				{
					duDebugDrawRawContours(&dd, *m_tileSet->tiles[i].cset, 0.5f);
					duDebugDrawContours(&dd, *m_tileSet->tiles[i].cset);
				}
			}
			glDepthMask(GL_TRUE);
		}
		if (m_drawMode == DRAWMODE_CONTOURS)
		{
			glDepthMask(GL_FALSE);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].cset && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawContours(&dd, *m_tileSet->tiles[i].cset);
			}
			glDepthMask(GL_TRUE);
		}
		if (m_drawMode == DRAWMODE_REGION_CONNECTIONS)
		{
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].chf && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawCompactHeightfieldRegions(&dd, *m_tileSet->tiles[i].chf);
			}
			
			glDepthMask(GL_FALSE);
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].cset && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
					duDebugDrawRegionConnections(&dd, *m_tileSet->tiles[i].cset);
			}
			glDepthMask(GL_TRUE);
		}
		if (/*m_pmesh &&*/ m_drawMode == DRAWMODE_POLYMESH)
		{
			glDepthMask(GL_FALSE);
			if (m_pmesh)
			{
				duDebugDrawPolyMesh(&dd, *m_pmesh);
			}
			else
			{
				for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
				{
					if (m_tileSet->tiles[i].pmesh && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
						duDebugDrawPolyMesh(&dd, *m_tileSet->tiles[i].pmesh);
				}
			}
			
			glDepthMask(GL_TRUE);
		}
		if (/*m_dmesh &&*/ m_drawMode == DRAWMODE_POLYMESH_DETAIL)
		{
			glDepthMask(GL_FALSE);
			if (m_dmesh)
			{
				duDebugDrawPolyMeshDetail(&dd, *m_dmesh);
			}
			else
			{
				for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
				{
					if (m_tileSet->tiles[i].dmesh && canDrawTile(m_tileSet->tiles[i].x,m_tileSet->tiles[i].y))
						duDebugDrawPolyMeshDetail(&dd, *m_tileSet->tiles[i].dmesh);
				}
			}
			glDepthMask(GL_TRUE);
		}
	}
		
	m_geom->drawConvexVolumes(&dd);

	if (m_tool)
		m_tool->handleRender();

	glDepthMask(GL_TRUE);
}

static float nicenum(float x, int round)
{
	float expv = floorf(log10f(x));
	float f = x / powf(10.0f, expv);
	float nf;
	if (round)
	{
		if (f < 1.5f) nf = 1.0f;
		else if (f < 3.0f) nf = 2.0f;
		else if (f < 7.0f) nf = 5.0f;
		else nf = 10.0f;
	}
	else
	{
		if (f <= 1.0f) nf = 1.0f;
		else if (f <= 2.0f) nf = 2.0f;
		else if (f <= 5.0f) nf = 5.0f;
		else nf = 10.0f;
	}
	return nf*powf(10.0f, expv);
}

static void drawLabels(int x, int y, int w, int h,
					   int nticks, float vmin, float vmax, const char* unit)
{
	char str[8], temp[32];
	
	float range = nicenum(vmax-vmin, 0);
	float d = nicenum(range/(float)(nticks-1), 1);
	float graphmin = floorf(vmin/d)*d;
	float graphmax = ceilf(vmax/d)*d;
	int nfrac = (int)-floorf(log10f(d));
	if (nfrac < 0) nfrac = 0;
	snprintf(str, 6, "%%.%df %%s", nfrac);

	for (float v = graphmin; v < graphmax+d/2; v += d)
	{
		float lx = x + (v-vmin)/(vmax-vmin)*w;
		if (lx < 0 || lx > w) continue;
		snprintf(temp, 20, str, v, unit);
		imguiDrawText((int)lx+2, (int)y+2, IMGUI_ALIGN_LEFT, temp, imguiRGBA(255,255,255));
		glColor4ub(0,0,0,64);
		glBegin(GL_LINES);
		glVertex2f(lx,(float)y);
		glVertex2f(lx,(float)(y+h));
		glEnd();
	}
}

static void drawGraph(const char* name, int x, int y, int w, int h, float sd,
					  const int* samples, int n, int nsamples, const char* unit)
{
	char text[64];
	int first, last, maxval;
	first = 0;
	last = n-1;
	while (first < n && samples[first] == 0)
		first++;
	while (last >= 0 && samples[last] == 0)
		last--;
	if (first == last)
		return;
	maxval = 1;
	for (int i = first; i <= last; ++i)
	{
		if (samples[i] > maxval)
			maxval = samples[i];
	}
	const float sx = (float)w / (float)(last-first);
	const float sy = (float)h / (float)maxval;
	
	glBegin(GL_QUADS);
	glColor4ub(32,32,32,64);
	glVertex2i(x,y);
	glVertex2i(x+w,y);
	glVertex2i(x+w,y+h);
	glVertex2i(x,y+h);
	glEnd();
	
	glColor4ub(255,255,255,64);
	glBegin(GL_LINES);
	for (int i = 0; i <= 4; ++i)
	{
		int yy = y+i*h/4;
		glVertex2i(x,yy);
		glVertex2i(x+w,yy);
	}
	glEnd();

	glColor4ub(0,196,255,255);
	glBegin(GL_LINE_STRIP);
	for (int i = first; i <= last; ++i)
	{
		float fx = x + (i-first)*sx;
		float fy = y + samples[i]*sy;
		glVertex2f(fx,fy);
	}
	glEnd();

	snprintf(text,64,"%d", maxval);
	imguiDrawText((int)x+w-2, (int)y+h-20, IMGUI_ALIGN_RIGHT, text, imguiRGBA(0,0,0));
	imguiDrawText((int)x+2, (int)y+h-20, IMGUI_ALIGN_LEFT, name, imguiRGBA(255,255,255));
	
	drawLabels(x, y, w, h, 10, first*sd, last*sd, unit);
}

void Sample_SoloMeshTiled::handleRenderOverlay(double* proj, double* model, int* view)
{
	if (m_measurePerTileTimings)
	{
		if (m_statTimePerTileSamples)
			drawGraph("Build Time/Tile", 10, 10, 500, 100, 1.0f, m_statTimePerTile, MAX_STAT_BUCKETS, m_statTimePerTileSamples, "ms");

		if (m_statPolysPerTileSamples)
			drawGraph("Polygons/Tile", 10, 120, 500, 100, 1.0f, m_statPolysPerTile, MAX_STAT_BUCKETS, m_statPolysPerTileSamples, "");

		int validTiles = 0;
		if (m_tileSet)
		{
			for (int i = 0; i < m_tileSet->width*m_tileSet->height; ++i)
			{
				if (m_tileSet->tiles[i].buildTime > 0)
					validTiles++;
			}
		}
		char text[64];
		snprintf(text,64,"Tiles %d\n", validTiles);
		imguiDrawText(10, 240, IMGUI_ALIGN_LEFT, text, imguiRGBA(255,255,255));
	}

	if (m_tool)
		m_tool->handleRenderOverlay(proj, model, view);
}

void Sample_SoloMeshTiled::handleMeshChanged(class InputGeom* geom)
{
	Sample::handleMeshChanged(geom);

	delete m_navMesh;
	m_navMesh = 0;
	
	m_statTimePerTileSamples = 0;
	m_statPolysPerTileSamples = 0;

	if (m_tool)
	{
		m_tool->reset();
		m_tool->init(this);
	}
}

bool Sample_SoloMeshTiled::handleBuild()
{
	if (!m_geom || !m_geom->getMesh() || !m_geom->getChunkyMesh())
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
		return false;
	}

	if (m_measurePerTileTimings)
	{
		memset(m_statPolysPerTile, 0, sizeof(m_statPolysPerTile));
		memset(m_statTimePerTile, 0, sizeof(m_statTimePerTile));
		m_statPolysPerTileSamples = 0;
		m_statTimePerTileSamples = 0;
	}
	
	cleanup();
	
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	const float* verts = m_geom->getMesh()->getVerts();
	const int nverts = m_geom->getMesh()->getVertCount();
	const int ntris = m_geom->getMesh()->getTriCount();
	const rcChunkyTriMesh* chunkyMesh = m_geom->getChunkyMesh();
	
	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionSize = (int)rcSqr(m_regionMinSize);
	m_cfg.mergeRegionSize = (int)rcSqr(m_regionMergeSize);
	m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
	m_cfg.tileSize = (int)m_tileSize;
	m_cfg.borderSize = m_cfg.walkableRadius + 4; // Reserve enough padding.
	m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;
		
	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	vcopy(m_cfg.bmin, bmin);
	vcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);
	
	// Reset build times gathering.
	memset(&m_buildTimes, 0, sizeof(m_buildTimes));
	rcSetBuildTimes(&m_buildTimes);
	
	// Start the build process.	
	rcTimeVal totStartTime = rcGetPerformanceTimer();

	// Calculate the number of tiles in the output and initialize tiles.
	m_tileSet = new TileSet;
	if (!m_tileSet)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Out of memory 'tileSet'.");
		return false;
	}
	vcopy(m_tileSet->bmin, m_cfg.bmin);
	vcopy(m_tileSet->bmax, m_cfg.bmax);
	m_tileSet->cs = m_cfg.cs;
	m_tileSet->ch = m_cfg.ch;
	m_tileSet->width = (m_cfg.width + m_cfg.tileSize-1) / m_cfg.tileSize;
	m_tileSet->height = (m_cfg.height + m_cfg.tileSize-1) / m_cfg.tileSize;
	m_tileSet->tiles = new Tile[m_tileSet->height * m_tileSet->width];
	if (!m_tileSet->tiles)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Out of memory 'tileSet->tiles' (%d).", m_tileSet->height * m_tileSet->width);
		return false;
	}
	
	if (rcGetLog())
	{
		rcGetLog()->log(RC_LOG_PROGRESS, "Building navigation:");
		rcGetLog()->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
		rcGetLog()->log(RC_LOG_PROGRESS, " - %d x %d tiles", m_tileSet->width, m_tileSet->height);
		rcGetLog()->log(RC_LOG_PROGRESS, " - %.1f verts, %.1f tris", nverts/1000.0f, ntris/1000.0f);
	}
		
	// Initialize per tile config.
	rcConfig tileCfg;
	memcpy(&tileCfg, &m_cfg, sizeof(rcConfig));
	tileCfg.width = m_cfg.tileSize + m_cfg.borderSize*2;
	tileCfg.height = m_cfg.tileSize + m_cfg.borderSize*2;
		
	// Allocate array that can hold triangle flags for all geom chunks.
	unsigned char* triangleFlags = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!triangleFlags)
	{
		if (rcGetLog())
		{
			rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Out of memory 'triangleFlags' (%d).",
							chunkyMesh->maxTrisPerChunk);
		}
		return false;
	}
		
	rcHeightfield* solid = 0;
	rcCompactHeightfield* chf = 0;
	rcContourSet* cset = 0;
		
	for (int y = 0; y < m_tileSet->height; ++y)
	{
		for (int x = 0; x < m_tileSet->width; ++x)
		{
			rcTimeVal startTime = rcGetPerformanceTimer();
			
			Tile& tile = m_tileSet->tiles[x + y*m_tileSet->width];
			tile.x = x;
			tile.y = y;
			
			// Calculate the per tile bounding box.
			tileCfg.bmin[0] = m_cfg.bmin[0] + (x*m_cfg.tileSize - m_cfg.borderSize)*m_cfg.cs;
			tileCfg.bmin[2] = m_cfg.bmin[2] + (y*m_cfg.tileSize - m_cfg.borderSize)*m_cfg.cs;
			tileCfg.bmax[0] = m_cfg.bmin[0] + ((x+1)*m_cfg.tileSize + m_cfg.borderSize)*m_cfg.cs;
			tileCfg.bmax[2] = m_cfg.bmin[2] + ((y+1)*m_cfg.tileSize + m_cfg.borderSize)*m_cfg.cs;
			
			delete solid;
			delete chf;
			solid = 0;
			chf = 0;
			
			float tbmin[2], tbmax[2];
			tbmin[0] = tileCfg.bmin[0];
			tbmin[1] = tileCfg.bmin[2];
			tbmax[0] = tileCfg.bmax[0];
			tbmax[1] = tileCfg.bmax[2];
			int cid[256];// TODO: Make grow when returning too many items.
			const int ncid = rcGetChunksInRect(chunkyMesh, tbmin, tbmax, cid, 256);
			if (!ncid)
				continue;
			
			solid = new rcHeightfield;
			if (!solid)
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Out of memory 'solid'.", x, y);
				continue;
			}
			if (!rcCreateHeightfield(*solid, tileCfg.width, tileCfg.height, tileCfg.bmin, tileCfg.bmax, tileCfg.cs, tileCfg.ch))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not create solid heightfield.", x, y);
				continue;
			}
			
			for (int i = 0; i < ncid; ++i)
			{
				const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
				const int* tris = &chunkyMesh->tris[node.i*3];
				const int ntris = node.n;
				
				memset(triangleFlags, 0, ntris*sizeof(unsigned char));
				rcMarkWalkableTriangles(tileCfg.walkableSlopeAngle,
										verts, nverts, tris, ntris, triangleFlags);
				
				rcRasterizeTriangles(verts, nverts, tris, triangleFlags, ntris, *solid, m_cfg.walkableClimb);
			}	
			
			rcFilterLowHangingWalkableObstacles(m_cfg.walkableClimb, *solid);
			rcFilterLedgeSpans(tileCfg.walkableHeight, tileCfg.walkableClimb, *solid);
			rcFilterWalkableLowHeightSpans(tileCfg.walkableHeight, *solid);
			
			chf = new rcCompactHeightfield;
			if (!chf)
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Out of memory 'chf'.", x, y);
				continue;
			}
			if (!rcBuildCompactHeightfield(tileCfg.walkableHeight, tileCfg.walkableClimb,
										   RC_WALKABLE, *solid, *chf))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not build compact data.", x, y);
				continue;
			}
			
			// Erode the walkable area by agent radius.
			if (!rcErodeArea(RC_WALKABLE_AREA, m_cfg.walkableRadius, *chf))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Could not erode.");
				continue;
			}
			
			// (Optional) Mark areas.
			const ConvexVolume* vols = m_geom->getConvexVolumes();
			for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
				rcMarkConvexPolyArea(vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *chf);
			
			if (!rcBuildDistanceField(*chf))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not build distance fields.", x, y);
				continue;
			}
			
			if (!rcBuildRegions(*chf, tileCfg.borderSize, tileCfg.minRegionSize, tileCfg.mergeRegionSize))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not build regions.", x, y);
				continue;
			}
			
			cset = new rcContourSet;
			if (!cset)
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Out of memory 'cset'.", x, y);
				continue;
			}
			if (!rcBuildContours(*chf, tileCfg.maxSimplificationError, tileCfg.maxEdgeLen, *cset))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not create contours.", x, y);
				continue;
			}
			
			if (!cset->nconts)
			{
				delete cset;
				cset = 0;
				continue;
			}
			
			tile.pmesh = new rcPolyMesh;
			if (!tile.pmesh)
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Out of memory 'pmesh'.", x, y);
				continue;
			}
			if (!rcBuildPolyMesh(*cset, tileCfg.maxVertsPerPoly, *tile.pmesh))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not create poly mesh.", x, y);
				continue;
			}

			tile.dmesh = new rcPolyMeshDetail;
			if (!tile.dmesh)
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Out of memory 'dmesh'.", x, y);
				continue;
			}
			
			if (!rcBuildPolyMeshDetail(*tile.pmesh, *chf, tileCfg.detailSampleDist, tileCfg	.detailSampleMaxError, *tile.dmesh))
			{
				if (rcGetLog())
					rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: [%d,%d] Could not build detail mesh.", x, y);
				continue;
			}

			if (m_keepInterResults)
			{
				tile.solid = solid;
				solid = 0;
				tile.chf = chf;
				chf = 0;
				tile.cset = cset;
				cset = 0;
			}
			
			rcTimeVal endTime = rcGetPerformanceTimer();
			tile.buildTime += rcGetDeltaTimeUsec(startTime, endTime);
			
			// Some extra code to measure some per tile statistics,
			// such as build time and how many polygons there are per tile.
			if (tile.pmesh)
			{
				int bucket = tile.pmesh->npolys;
				if (bucket < 0) bucket = 0;
				if (bucket >= MAX_STAT_BUCKETS) bucket = MAX_STAT_BUCKETS-1;
				m_statPolysPerTile[bucket]++;
				m_statPolysPerTileSamples++;
			}
			int bucket = (tile.buildTime+500)/1000;
			if (bucket < 0) bucket = 0;
			if (bucket >= MAX_STAT_BUCKETS) bucket = MAX_STAT_BUCKETS-1;
			m_statTimePerTile[bucket]++;
			m_statTimePerTileSamples++;
		}
	}
	
	delete [] triangleFlags;
	delete solid;
	delete chf;
	
	// Merge per tile poly and detail meshes.
	rcPolyMesh** pmmerge = new rcPolyMesh*[m_tileSet->width*m_tileSet->height];
	if (!pmmerge)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Out of memory 'pmmerge' (%d).", m_tileSet->width*m_tileSet->height);
		return false;
	}
	
	rcPolyMeshDetail** dmmerge = new rcPolyMeshDetail*[m_tileSet->width*m_tileSet->height];
	if (!dmmerge)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildTiledNavigation: Out of memory 'dmmerge' (%d).", m_tileSet->width*m_tileSet->height);
		return false;
	}
	
	int nmerge = 0;
	for (int y = 0; y < m_tileSet->height; ++y)
	{
		for (int x = 0; x < m_tileSet->width; ++x)
		{
			Tile& tile = m_tileSet->tiles[x + y*m_tileSet->width]; 
			if (tile.pmesh)
			{
				pmmerge[nmerge] = tile.pmesh;
				dmmerge[nmerge] = tile.dmesh;
				nmerge++;
			}
		}
	}
	
	m_pmesh = new rcPolyMesh;
	if (!m_pmesh)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	rcMergePolyMeshes(pmmerge, nmerge, *m_pmesh);

	m_dmesh = new rcPolyMeshDetail;
	if (!m_dmesh)
	{
		if (rcGetLog())
			rcGetLog()->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
		return false;
	}
	rcMergePolyMeshDetails(dmmerge, nmerge, *m_dmesh);
	
	delete [] pmmerge;
	delete [] dmmerge;
	
	if (!m_keepInterResults)
	{
		for (int y = 0; y < m_tileSet->height; ++y)
		{
			for (int x = 0; x < m_tileSet->width; ++x)
			{
				Tile& tile = m_tileSet->tiles[x + y*m_tileSet->width]; 
				delete tile.cset;
				tile.cset = 0;
				delete tile.pmesh;
				tile.pmesh = 0;
				delete tile.dmesh;
				tile.dmesh = 0;
			}
		}
	}
	
	if (m_pmesh && m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;
			
			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}
		
		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		vcopy(params.bmin, m_pmesh->bmin);
		vcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		
		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			if (rcGetLog())
				rcGetLog()->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return false;
		}
		
		m_navMesh = new dtNavMesh;
		if (!m_navMesh)
		{
			delete [] navData;
			if (rcGetLog())
				rcGetLog()->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return false;
		}
		
		if (!m_navMesh->init(navData, navDataSize, true, 2048))
		{
			delete [] navData;
			if (rcGetLog())
				rcGetLog()->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return false;
		}
	}
		
	rcTimeVal totEndTime = rcGetPerformanceTimer();
	
	if (rcGetLog())
	{
		const float pc = 100.0f / rcGetDeltaTimeUsec(totStartTime, totEndTime);

		rcGetLog()->log(RC_LOG_PROGRESS, "Rasterize: %.1fms (%.1f%%)", m_buildTimes.rasterizeTriangles/1000.0f, m_buildTimes.rasterizeTriangles*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Build Compact: %.1fms (%.1f%%)", m_buildTimes.buildCompact/1000.0f, m_buildTimes.buildCompact*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Filter Border: %.1fms (%.1f%%)", m_buildTimes.filterBorder/1000.0f, m_buildTimes.filterBorder*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "Filter Walkable: %.1fms (%.1f%%)", m_buildTimes.filterWalkable/1000.0f, m_buildTimes.filterWalkable*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "Filter Reachable: %.1fms (%.1f%%)", m_buildTimes.filterMarkReachable/1000.0f, m_buildTimes.filterMarkReachable*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Erode walkable area: %.1fms (%.1f%%)", m_buildTimes.erodeArea/1000.0f, m_buildTimes.erodeArea*pc);

		rcGetLog()->log(RC_LOG_PROGRESS, "Build Distancefield: %.1fms (%.1f%%)", m_buildTimes.buildDistanceField/1000.0f, m_buildTimes.buildDistanceField*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - distance: %.1fms (%.1f%%)", m_buildTimes.buildDistanceFieldDist/1000.0f, m_buildTimes.buildDistanceFieldDist*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - blur: %.1fms (%.1f%%)", m_buildTimes.buildDistanceFieldBlur/1000.0f, m_buildTimes.buildDistanceFieldBlur*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Build Regions: %.1fms (%.1f%%)", m_buildTimes.buildRegions/1000.0f, m_buildTimes.buildRegions*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - watershed: %.1fms (%.1f%%)", m_buildTimes.buildRegionsReg/1000.0f, m_buildTimes.buildRegionsReg*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "    - expand: %.1fms (%.1f%%)", m_buildTimes.buildRegionsExp/1000.0f, m_buildTimes.buildRegionsExp*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "    - find catchment basins: %.1fms (%.1f%%)", m_buildTimes.buildRegionsFlood/1000.0f, m_buildTimes.buildRegionsFlood*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - filter: %.1fms (%.1f%%)", m_buildTimes.buildRegionsFilter/1000.0f, m_buildTimes.buildRegionsFilter*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Build Contours: %.1fms (%.1f%%)", m_buildTimes.buildContours/1000.0f, m_buildTimes.buildContours*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - trace: %.1fms (%.1f%%)", m_buildTimes.buildContoursTrace/1000.0f, m_buildTimes.buildContoursTrace*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "  - simplify: %.1fms (%.1f%%)", m_buildTimes.buildContoursSimplify/1000.0f, m_buildTimes.buildContoursSimplify*pc);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "Build Polymesh: %.1fms (%.1f%%)", m_buildTimes.buildPolymesh/1000.0f, m_buildTimes.buildPolymesh*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "Build Polymesh Detail: %.1fms (%.1f%%)", m_buildTimes.buildDetailMesh/1000.0f, m_buildTimes.buildDetailMesh*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "Merge Polymeshes: %.1fms (%.1f%%)", m_buildTimes.mergePolyMesh/1000.0f, m_buildTimes.mergePolyMesh*pc);
		rcGetLog()->log(RC_LOG_PROGRESS, "Merge Polymesh Details: %.1fms (%.1f%%)", m_buildTimes.mergePolyMeshDetail/1000.0f, m_buildTimes.mergePolyMeshDetail*pc);
		
		if (m_pmesh)
			rcGetLog()->log(RC_LOG_PROGRESS, "Polymesh: Verts:%d  Polys:%d", m_pmesh->nverts, m_pmesh->npolys);
		
		rcGetLog()->log(RC_LOG_PROGRESS, "TOTAL: %.1fms", rcGetDeltaTimeUsec(totStartTime, totEndTime)/1000.0f);
	}

	if (m_tool)
		m_tool->init(this);

	return true;
}

bool Sample_SoloMeshTiled::canDrawTile(int x, int y)
{
	if (m_highLightedTileX == -1) return true;
	return m_highLightedTileX == x && m_highLightedTileY == y;
}

void Sample_SoloMeshTiled::setHighlightedTile(const float* pos)
{
	if (!pos)
	{
		m_highLightedTileX = -1;
		m_highLightedTileY = -1;
		return;
	}
	const float* bmin = m_geom->getMeshBoundsMin();
	const float ts = m_tileSize*m_cellSize;
	m_highLightedTileX = (int)((pos[0] - bmin[0]) / ts);
	m_highLightedTileY = (int)((pos[2] - bmin[2]) / ts);
}
