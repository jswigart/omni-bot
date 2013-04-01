#pragma once
#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include <vector>
#include <set>

#include "common.h"

#include "Opcode.h"
#include "OPC_IceHook.h"

class CollisionModel
{
public:
	CollisionModel();
	~CollisionModel();

	void Add( const Vector3List & poly, void * attrib );

	bool Build( bool createDefaultIfEmpty );

	bool CollideRay( const Vector3f & from, const Vector3f & to, Vector3f & hitPos, Vector3f & hitNormal, void *& hitAttrib );

	typedef std::vector<void*> AttribList;
	typedef std::set<void*> AttribSet;

	// all attribs, even duplicates
	bool CollideAABB( const AABB & worldaabb, AttribList & hitAttribs );
	bool CollideOBB( const Box3f & obb, AttribList & hitAttribs );

	// get only unique attribs
	bool CollideAABB( const AABB & aabb, AttribSet & hitAttribs );
	bool CollideOBB( const Box3f & obb, AttribSet & hitAttribs );

	void Free();

	size_t NumTriangles() const { return mVerts.size() / 3; }

	void Render( const Vector3f & offset );
	void RenderSkipSet( const AttribSet & skip, const Vector3f & offset );
private:
	IceMaths::Matrix4x4				mTransform;
	Opcode::Model *					mCollisionTree;
	Opcode::MeshInterface *			mMeshInterface;

	std::vector<IceMaths::Point>	mVerts;
	std::vector<void*>				mAttribs;
};

#endif