// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CLODMESH_H
#define WM5CLODMESH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5CollapseRecordArray.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM ClodMesh : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ClodMesh);

public:
    // Construction and destruction.  ClodMesh will make a copy of the index
    // buffer of 'mesh', because it needs to be able to update the indices
    // independently when two or more ClodMesh objects share the same vertex
    // buffer and collapse records.
    ClodMesh (TriMesh* mesh, CollapseRecordArray* recordArray);
    virtual ~ClodMesh ();

    // LOD selection is based on manual selection by the application.  To
    // use distance from camera or screen space coverage, derive a class
    // from ClodMesh and override 'GetAutomatedTargetRecord'.
    inline int GetNumRecords () const;
    inline int& TargetRecord ();
    inline virtual int GetAutomatedTargetRecord ();

    // Geometric updates.  The Draw method will call this update and adjust
    // the TriMesh quantities according to the current value of the target
    // record.  You can call this manually in an application that does not
    // need to display the mesh.
    void SelectLevelOfDetail ();

protected:
    // Support for hierarchical culling.
    virtual void GetVisibleSet (Culler& culler, bool noCull);

    // Members for selection of LOD.
    int mCurrentRecord, mTargetRecord;
    CollapseRecordArrayPtr mRecordArray;
};

WM5_REGISTER_STREAM(ClodMesh);
typedef Pointer0<ClodMesh> ClodMeshPtr;
#include "Wm5ClodMesh.inl"

}

#endif
