#include "cg_local.h"

extern vmCvar_t	cg_omnibotdrawing, cg_omnibot_render_distance;
extern void OmnibotDrawActiveFrame();

const int OMNIBOT_LINES_INCREASE_SIZE = 256;
const int OMNIBOT_LINES_INITIAL_SIZE = 128;

//////////////////////////////////////////////////////////////////////////

typedef enum
{
	DBG_LINE,
	DBG_RADIUS,
	DBG_AABB,
} DebugShapeType;

typedef struct
{
	vec3_t start, end;	
	int color;
} UdpDebugLineMessage;

typedef struct
{
	vec3_t pos;
	float radius;
	int color;
} UdpDebugRadiusMessage;

typedef struct
{
	vec3_t mins, maxs;
	int side;
	int color;
} UdpDebugAABBMessage;

typedef struct
{
	vec3_t verts[64];
	int numverts;
	int color;
	int	expiretime;
	int	duration;
} UdpDebugPolygon_t;

typedef struct
{
	union
	{
		UdpDebugLineMessage	line;
		UdpDebugRadiusMessage radius;
		UdpDebugAABBMessage aabb;
	} info;

	int	expiretime;
	int	duration;
	int	debugtype;
} UdpDebugLines_t;

typedef struct 
{
	UdpDebugLines_t		*m_pDebugLines;
	int					m_NumDebugLines;
	int					m_MaxDebugLines;
} LineList;

typedef struct
{
	UdpDebugPolygon_t		*m_pDebugPolygons;
	int					m_NumDebugPolygons;
	int					m_MaxDebugPolygons;
} PolygonList;

LineList		g_DebugLines;
PolygonList g_DebugPolygons;

static int	g_LastRenderTime;
static char InfoText[1024];
static int InfoColor;

//////////////////////////////////////////////////////////////////////////

void CG_DrawLine(float *start, float *end, int color)
{
	vec3_t		forward, right;
	polyVert_t	verts[4];
	vec3_t		line;
	const float fLineWidth = 2.0f;

	//////////////////////////////////////////////////////////////////////////
	// Check the distance.
	// This is fast, but incorrect. 
	// A very long line is invisible if the player is near line center.
	{
		vec3_t toline;
		float fRenderDistanceSq = cg_omnibot_render_distance.integer * cg_omnibot_render_distance.integer;
		VectorSubtract(cg_entities[0].currentState.pos.trBase, start, toline);
		if(VectorLengthSquared(toline) > fRenderDistanceSq){
			VectorSubtract(cg_entities[0].currentState.pos.trBase, end, toline);
			if(VectorLengthSquared(toline) > fRenderDistanceSq)
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	VectorSubtract( end, start, forward );

	line[0] = DotProduct( forward, cg.refdef_current->viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef_current->viewaxis[2] );

	VectorScale( cg.refdef_current->viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef_current->viewaxis[2], right );
	VectorNormalize( right );

	//////////////////////////////////////////////////////////////////////////

	VectorMA( end, fLineWidth, right, verts[0].xyz );
	verts[0].st[0] = 1;
	verts[0].st[1] = 1;

	VectorMA( end, -fLineWidth, right, verts[1].xyz );
	verts[1].st[0] = 1;
	verts[1].st[1] = 0;

	VectorMA( start, -fLineWidth, right, verts[2].xyz );
	verts[2].st[0] = 0;
	verts[2].st[1] = 0;

	VectorMA( start, fLineWidth, right, verts[3].xyz );
	verts[3].st[0] = 0;
	verts[3].st[1] = 1;

	*(int*)verts[0].modulate = *(int*)verts[1].modulate = *(int*)verts[2].modulate = *(int*)verts[3].modulate = color;

	trap_R_AddPolyToScene( cgs.media.railCoreShader, 4, verts );
}

void CG_DrawDebugLine(UdpDebugLineMessage *lineinfo)
{
	CG_DrawLine(lineinfo->start, lineinfo->end, lineinfo->color);
}

void CG_DrawDebugRadius(UdpDebugRadiusMessage *_lineinfo)
{	
	UdpDebugLineMessage lne;
	const int iNumSteps = 7;
	const float fStepSize = M_PI / iNumSteps;
	const float stepSin = sin(fStepSize);
	const float stepCos = cos(fStepSize);
	float tmp;
	int i;
	vec3_t pnt;

	if (!cgs.media.railCoreShader)
		cgs.media.railCoreShader = trap_R_RegisterShader("railCore");

	lne.color = _lineinfo->color;

	VectorSet(pnt, 0, _lineinfo->radius, 0);

	for(i = 0; i < iNumSteps; ++i)
	{
		VectorAdd(_lineinfo->pos, pnt, lne.start);
		VectorSubtract(_lineinfo->pos, pnt, lne.end);
		CG_DrawDebugLine(&lne);

		// rotate point
		tmp = pnt[0] * stepSin;
		pnt[0] = pnt[0] * stepCos - pnt[1] * stepSin;
		pnt[1] = tmp + pnt[1] * stepCos;
	}
}

void CG_DrawDebugAABB(UdpDebugAABBMessage *_aabbinfo)
{
	UdpDebugLineMessage l;
	vec3_t vVertex[8];

	vVertex[0][0] = _aabbinfo->mins[0];
	vVertex[0][1] = _aabbinfo->mins[1];
	vVertex[0][2] = _aabbinfo->mins[2];

	vVertex[1][0] = _aabbinfo->maxs[0];
	vVertex[1][1] = _aabbinfo->mins[1];
	vVertex[1][2] = _aabbinfo->mins[2];

	vVertex[2][0] = _aabbinfo->maxs[0];
	vVertex[2][1] = _aabbinfo->maxs[1];
	vVertex[2][2] = _aabbinfo->mins[2];

	vVertex[3][0] = _aabbinfo->mins[0];
	vVertex[3][1] = _aabbinfo->maxs[1];
	vVertex[3][2] = _aabbinfo->mins[2];

	vVertex[4][0] = _aabbinfo->mins[0];
	vVertex[4][1] = _aabbinfo->mins[1];
	vVertex[4][2] = _aabbinfo->maxs[2];

	vVertex[5][0] = _aabbinfo->maxs[0];
	vVertex[5][1] = _aabbinfo->mins[1];
	vVertex[5][2] = _aabbinfo->maxs[2];

	vVertex[6][0] = _aabbinfo->maxs[0];
	vVertex[6][1] = _aabbinfo->maxs[1];
	vVertex[6][2] = _aabbinfo->maxs[2];

	vVertex[7][0] = _aabbinfo->mins[0];
	vVertex[7][1] = _aabbinfo->maxs[1];
	vVertex[7][2] = _aabbinfo->maxs[2];

	l.color = _aabbinfo->color; 

	// Top
	if(_aabbinfo->side == 4 || _aabbinfo->side == 6)
	{
			VectorCopy(vVertex[4], l.start);
			VectorCopy(vVertex[5], l.end);			
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[5], l.start);
			VectorCopy(vVertex[6], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[6], l.start);
			VectorCopy(vVertex[7], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[7], l.start);
			VectorCopy(vVertex[4], l.end);
			CG_DrawDebugLine(&l);
	}		

	// Bottom
	if(_aabbinfo->side == 5 || _aabbinfo->side == 6)
	{
			VectorCopy(vVertex[0], l.start);
			VectorCopy(vVertex[1], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[1], l.start);
			VectorCopy(vVertex[2], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[2], l.start);
			VectorCopy(vVertex[3], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[3], l.start);
			VectorCopy(vVertex[0], l.end);
			CG_DrawDebugLine(&l);
	}

	// Sides
	if(_aabbinfo->side == 6)
	{
			VectorCopy(vVertex[4], l.start);
			VectorCopy(vVertex[0], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[5], l.start);
			VectorCopy(vVertex[1], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[6], l.start);
			VectorCopy(vVertex[2], l.end);
			CG_DrawDebugLine(&l);

			VectorCopy(vVertex[7], l.start);
			VectorCopy(vVertex[3], l.end);
			CG_DrawDebugLine(&l);
	}
}

void CG_DrawDebugPolygon(UdpDebugPolygon_t *_polyinfo)
{
	int i = 0;
	polyVert_t	verts[65];

	for(; i < _polyinfo->numverts; ++i)
	{
		VectorCopy(_polyinfo->verts[_polyinfo->numverts-i-1], verts[i].xyz);

		verts[i].st[0] = 0;
		verts[i].st[1] = 0;
		*(int*)verts[i].modulate = _polyinfo->color;
	}
	VectorCopy(_polyinfo->verts[0], verts[i].xyz);

	trap_R_AddPolyToScene( cgs.media.whiteShader, i, verts );
}

//////////////////////////////////////////////////////////////////////////

void ClearDebugDraw()
{
	g_DebugLines.m_NumDebugLines = 0;
	if(g_DebugLines.m_pDebugLines)
	{
		free(g_DebugLines.m_pDebugLines);
		g_DebugLines.m_pDebugLines = 0;
	}
	g_DebugLines.m_MaxDebugLines = 0;

	g_DebugPolygons.m_NumDebugPolygons = 0;
	if(g_DebugPolygons.m_pDebugPolygons)
	{
		free(g_DebugPolygons.m_pDebugPolygons);
		g_DebugPolygons.m_pDebugPolygons = 0;
	}
	g_DebugPolygons.m_MaxDebugPolygons = 0;
}

UdpDebugLines_t* AddToLineList()
{
	UdpDebugLines_t *lne;
	LineList *_list = &g_DebugLines;

	if(_list->m_NumDebugLines >= _list->m_MaxDebugLines)
	{
		// We've gone over, so we need to reallocate.
		int iNewBufferSize = (_list->m_MaxDebugLines == 0) ? OMNIBOT_LINES_INITIAL_SIZE : 
			_list->m_MaxDebugLines + OMNIBOT_LINES_INCREASE_SIZE;
		UdpDebugLines_t *pNewBuffer = (UdpDebugLines_t*)calloc(iNewBufferSize, sizeof(UdpDebugLines_t));

		// Copy the old buffer to the new.
		memcpy(pNewBuffer, _list->m_pDebugLines, sizeof(UdpDebugLines_t) * _list->m_MaxDebugLines);

		// Free the old buffer.
		free(_list->m_pDebugLines);

		// Update the pointer to the new buffer.
		_list->m_pDebugLines = pNewBuffer;

		// Save the new size.
		_list->m_MaxDebugLines = iNewBufferSize;		
	}

	// Add it to the list.
	lne = &_list->m_pDebugLines[_list->m_NumDebugLines++];
	lne->expiretime = 0;
	return lne;
}

UdpDebugPolygon_t* AddToPolygonList()
{
	UdpDebugPolygon_t *lne;
	PolygonList *_list = &g_DebugPolygons;

	if(_list->m_NumDebugPolygons >= _list->m_MaxDebugPolygons)
	{
		// We've gone over, so we need to reallocate.
		int iNewBufferSize = _list->m_MaxDebugPolygons + OMNIBOT_LINES_INCREASE_SIZE;
		UdpDebugPolygon_t *pNewBuffer = (UdpDebugPolygon_t*)calloc(iNewBufferSize, sizeof(UdpDebugPolygon_t));

		// Copy the old buffer to the new.
		memcpy(pNewBuffer, _list->m_pDebugPolygons, sizeof(UdpDebugPolygon_t) * _list->m_MaxDebugPolygons);

		// Free the old buffer.
		free(_list->m_pDebugPolygons);

		// Update the pointer to the new buffer.
		_list->m_pDebugPolygons = pNewBuffer;

		// Save the new size.
		_list->m_MaxDebugPolygons = iNewBufferSize;
	}

	// Add it to the list.
	lne = &_list->m_pDebugPolygons[_list->m_NumDebugPolygons++];
	lne->expiretime = 0;
	return lne;
}

//////////////////////////////////////////////////////////////////////////

void DrawDebugLine(float *_start, float *_end, int _duration, int _color)
{
	if(cg_omnibotdrawing.integer){
		int i;
		UdpDebugLines_t *lne = AddToLineList();

		lne->debugtype = DBG_LINE;
		lne->duration = _duration;
		lne->info.line.color = _color;

		for(i = 0; i < 3; ++i)
		{
			lne->info.line.start[i] = _start[i];
			lne->info.line.end[i] = _end[i];
		}
	}
}

void DrawDebugRadius(float *_start, float _radius, int _duration, int _color)
{
	if(cg_omnibotdrawing.integer){
		int i;
		UdpDebugLines_t *lne = AddToLineList();

		lne->debugtype = DBG_RADIUS;
		lne->duration = _duration;
		lne->info.radius.color = _color;

		for(i = 0; i < 3; ++i)
			lne->info.radius.pos[i] = _start[i];
		lne->info.radius.radius = _radius;
	}
}

void DrawDebugAABB(float *mins, float *_maxs, int _duration, int _color, int _side)
{
	if(cg_omnibotdrawing.integer){
		int i;
		UdpDebugLines_t *lne = AddToLineList();

		lne->debugtype = DBG_AABB;
		lne->duration = _duration;
		lne->info.aabb.color = _color;

		for(i = 0; i < 3; ++i)
		{
			lne->info.aabb.mins[i] = mins[i];
			lne->info.aabb.maxs[i] = _maxs[i];
		}
		lne->info.aabb.side = _side;
	}
}

void DrawDebugPolygon(vec3_t *verts, int _numverts, int _duration, int _color)
{
	if(cg_omnibotdrawing.integer && _numverts <= 64){
		UdpDebugPolygon_t *lne = AddToPolygonList();

		lne->duration = _duration;
		lne->color = _color;
		lne->numverts = _numverts;
		memcpy(lne->verts, verts, _numverts * sizeof(vec3_t));
	}
}

qboolean CG_AddOnScreenText( const char *text, vec3_t origin, int _color, float duration );

void DrawDebugText(float *_start, const char *_msg, int _duration, int _color)
{
	if(cg_omnibotdrawing.integer){
		if(_start && !VectorCompare(_start, vec3_origin))
		{
			vec3_t v3;
			VectorSet(v3, _start[0], _start[1], _start[2]);
			CG_AddOnScreenText(_msg, v3, _color, (float)_duration/1000.f);
		}
		else
		{
			Q_strncpyz(InfoText, _msg, sizeof(InfoText));
			InfoColor = _color;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void OmnibotRenderDebugLines()
{
	int i;
	UdpDebugLines_t* line;
	UdpDebugPolygon_t* polygon;

	if(!cg_omnibotdrawing.integer) return;

	for(i = 0; i < g_DebugLines.m_NumDebugLines;)
	{
		line = &g_DebugLines.m_pDebugLines[i];
		if(line->expiretime <= cg.time)
		{
			if(!line->expiretime){
				line->expiretime = cg.time + line->duration;
			}
			else{
				// swap with the last and reduce the num
				*line = g_DebugLines.m_pDebugLines[--g_DebugLines.m_NumDebugLines];
				continue;
			}
		}

		switch(line->debugtype)
		{
			case DBG_LINE:
				CG_DrawDebugLine(&line->info.line);
				break;
			case DBG_RADIUS:
				CG_DrawDebugRadius(&line->info.radius);
				break;
			case DBG_AABB:
				CG_DrawDebugAABB(&line->info.aabb);
				break;
		}

		g_LastRenderTime = cg.time;
		++i;
	}

	// draw polygons
	for(i = 0; i < g_DebugPolygons.m_NumDebugPolygons;)
	{
		polygon = &g_DebugPolygons.m_pDebugPolygons[i];
		if(polygon->expiretime <= cg.time)
		{
			if(!polygon->expiretime){
				polygon->expiretime = cg.time + polygon->duration;
			}
			else{
				// swap with the last and reduce the num
				*polygon = g_DebugPolygons.m_pDebugPolygons[--g_DebugPolygons.m_NumDebugPolygons];
				continue;
			}
		}
		CG_DrawDebugPolygon(polygon);

		g_LastRenderTime = cg.time;
		++i;
	}


	// After a timeout, free the memory.
	if(g_LastRenderTime + 10000 < cg.time && g_DebugLines.m_pDebugLines) ClearDebugDraw();

	// ensiform's updated func to fix fpinfo
	if(InfoText[0])
	{
		vec4_t v4Color;
		union
		{
			char		m_RGBA[4];
			int			m_RGBAi;
		} ColorUnion;
		ColorUnion.m_RGBAi = InfoColor;

		Vector4Set(v4Color, (float)ColorUnion.m_RGBA[0]/255.f,
			(float)ColorUnion.m_RGBA[1]/255.f,
			(float)ColorUnion.m_RGBA[2]/255.f,
			(float)ColorUnion.m_RGBA[3]/255.f);

		trap_R_SetColor(v4Color);
		CPri(InfoText);
		trap_R_SetColor(NULL);
		InfoText[0]=0;
	}

	//callback to draw waypoints
	OmnibotDrawActiveFrame();
}

