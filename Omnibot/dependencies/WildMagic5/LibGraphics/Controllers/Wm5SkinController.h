// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SKINCONTROLLER_H
#define WM5SKINCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5Node.h"
#include "Wm5Vector3.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM SkinController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SkinController);

public:
    // Construction and destruction.  The numbers of vertices and bones are
    // fixed for the lifetime of the object.
    SkinController (int numVertices, int numBones);
    virtual ~SkinController ();

    // Member access.  After calling the constructor, you must set the data
    // using these functions.
    inline int GetNumVertices () const;
    inline int GetNumBones () const;
    inline Node** GetBones () const;
    inline float** GetWeights () const;
    inline APoint** GetOffsets () const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    int mNumVertices;   // nv
    int mNumBones;      // nb
    Node** mBones;      // bones[nb]
    float** mWeights;   // weight[nv][nb], index b+nb*v
    APoint** mOffsets;  // offset[nv][nb], index b+nb*v
};

WM5_REGISTER_STREAM(SkinController);
typedef Pointer0<SkinController> SkinControllerPtr;
#include "Wm5SkinController.inl"

}

#endif
