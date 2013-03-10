// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CURVEMESH_H
#define WM5CURVEMESH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Polysegment.h"
#include "Wm5CurveSegment.h"
#include "Wm5FloatArray.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CurveMesh : public Polysegment
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(CurveMesh);

public:
    // Construction and destruction.  The input 'vbuffer' has N >= 2 vertices
    // which are assumed to be ordered to form a continuous curve of N-1
    // curve segments.  The curve segments are stored in 'segments', an
    // array with N-1 elements.  CurveMesh assumes responsibility for deleting
    // this array, so it must be dynamically allocated.  The array 'params'
    // stores the parameter values for the curve segment endpoints.  This
    // array must have 2*(N-1) values.  Curve segment P[i] has domain
    // [params[2*i],params[2*i+1]].  Continuity requires that
    // P[i](params[2*i+1]) = P[i+1](params[2*(i+1)]).  The polysegment
    // produced by the subdivision is flagged as open.  If you want a closed
    // polysegment, you should make certain that the first endpoint of
    // P[0] matches the last endpoint of P[N-1].
    //
    // The parameter 'allowDynamicChange' should be set to 'true' when you
    // plan to dynamically modify the curve segments.  For example, you might
    // modify the control points in a Bezier curve segment.
    CurveMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
        CurveSegmentPtr* segments, FloatArray* params,
        bool allowDynamicChange);

    virtual ~CurveMesh ();

    // Level of subdivision.  The 'level' value must be nonnegative.
    void SetLevel (int level);
    int GetLevel () const;

    // Call this function when one or more of your curve segments has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the Polysegment data.
    // The intent is that the curve mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    VertexBufferPtr mOrigVBuffer;
    FloatArrayPtr mOrigParams;
    CurveSegmentPtr* mSegments;
    int mNumFullVertices, mNumSegments, mLevel;

private:
    // Support for subdivision of the mesh.
    class Edge
    {
    public:
        Edge ();

        // Curve for subdivision evaluations.
        CurveSegmentPtr Segment;

        // Indices for the vertices.
        int V[2];

        // Curve parameter values for the endpoints.
        float Param[2];
    };

    void Allocate (int& numTotalVertices, int& numTotalEdges, Edge*& edges);
    void Subdivide (int& numVertices, int& numEdges, Edge* edges);

    // Support for dynamic changes in the curve segments.
    class CurveInfo
    {
    public:
        friend class Memory;
        CurveInfo ();

        CurveSegmentPtr Segment;
        float Param;
    };

    void InitializeCurveInfo ();

    bool mAllowDynamicChange;
    CurveInfo* mCInfo;
};

WM5_REGISTER_STREAM(CurveMesh);
typedef Pointer0<CurveMesh> CurveMeshPtr;

}

#endif
