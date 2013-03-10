// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MORPHCONTROLLER_H
#define WM5MORPHCONTROLLER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Controller.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM MorphController : public Controller
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(MorphController);

public:
    // Construction and destruction.  The numbers of vertices, morph targets,
    // and the keys are fixed for the lifetime of the object.  The constructor
    // does some of the work of creating the controller.  The vertices per
    // target, the times, and the weights must all be assigned by the
    // appropriate member accessors.
    //
    // numVertices:  The number of vertices per target.  All targets have the
    //   same number of vertices.
    //
    // numTargets:  The number of targets to morph.
    //
    // numKeys:  The number of keys, each key occurring at a specific time.
    //
    MorphController (int numVertices, int numTargets, int numKeys);
    virtual ~MorphController ();

    // Member access.  After calling the constructor, you must set the vertex
    // arrays, times, and weights using these functions.
    inline int GetNumVertices () const;
    inline int GetNumTargets () const;
    inline int GetNumKeys () const;
    inline APoint* GetBaseVertices () const;
    inline AVector* GetDeltaVertices (int target) const;
    inline float* GetTimes () const;
    inline float* GetWeights (int key) const;

    // The animation update.  The application time is in milliseconds.
    virtual bool Update (double applicationTime);

protected:
    // Lookup on bounding keys.
    void GetKeyInfo (float ctrlTime, float& normTime, int& i0, int& i1);

    // Target geometry.  The number of vertices per target must match the
    // number of vertices in the managed geometry object.  The array of
    // vertices at location 0 are those of one of the targets.  Based on the
    // comments about "Morph keys" (below), the array at location i >= 1 is
    // computed as the difference between the i-th target and the 0-th target.
    int mNumVertices;
    int mNumTargets;
    APoint** mVertices;  // [numTargets][numVertices]

    // Morph keys.  The morphed object is a combination of N targets by
    // weights w[0] through w[N-1] with w[i] in [0,1] and sum_i w[i] = 1.
    // Each combination is sum_{i=0}^{N-1} w[i]*X[i] where X[i] is a vertex
    // of the i-th target.  This can be rewritten as a combination
    // X[0] + sum_{i=0}^{N-2} w'[i] Y[i] where w'[i] = w[i+1] and
    // Y[i] = X[i+1] - X[0].  The weights stored in this class are the
    // w'[i] (to reduce storage).  This also reduces computation time by a
    // small amount (coefficient of X[0] is 1, so no multiplication must
    // occur).
    int mNumKeys;
    float* mTimes;  // [numKeys]
    float** mWeights;  // [numKeys][numTargets-1]

    // For O(1) lookup on bounding keys.
    int mLastIndex;
};

WM5_REGISTER_STREAM(MorphController);
typedef Pointer0<MorphController> MorphControllerPtr;
#include "Wm5MorphController.inl"

}

#endif
