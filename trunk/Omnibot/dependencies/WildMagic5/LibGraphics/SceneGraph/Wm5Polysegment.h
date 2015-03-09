// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5POLYSEGMENT_H
#define WM5POLYSEGMENT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Visual.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Polysegment : public Visual
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Polysegment);

public:
    // Construction and destruction.  If 'contiguous' is 'true', then the
    // vertices form a true polysegment in the sense that each pair of
    // consecutive vertices are connected by a line segment.  For example,
    // {V0,V1,V2,V3} form segments <V0,V1>, <V1,V2>, and <V2,V3>.  If you
    // want a closed polysegment, the input vertex buffer's last element must
    // be a duplicate of the first element.  For example, {V0,V1,V2,V3=V0}
    // forms the triangle with segments <V0,V1>, <V1,V2>, and <V2,V0>.
    // If 'contiguous' is 'false', the vertices form a set of disconnected
    // line segments.  For example, {V0,V1,V2,V3} form segments <V0,V1>
    // and <V2,V3>.  In this case, the input vertex buffer must have an
    // even number of elements.
    Polysegment (VertexFormat* vformat, VertexBuffer* vbuffer,
        bool contiguous);

    virtual ~Polysegment ();

    // Member access.  Allow the application to specify fewer than the number
    // of segments in the polysegment to draw.  For a contiguous polysegment,
    // 0 <= numSegments <= numVertices-1.  For disjoint line segments,
    // 0 <= numSegments <= numVertices/2.
    int GetMaxNumSegments () const;
    void SetNumSegments (int numSegments);
    inline int GetNumSegments () const;

    // The polysegment is a sequence of connected segments when 'contiguous'
    // is 'true'.  It is a set of disjoint segments when 'contiguous' is
    // 'false'.
    inline bool GetContiguous () const;

protected:
    // The number of segments currently active.
    int mNumSegments;
    
    // The polyline has contiguous or disjoint segments.
    bool mContiguous;
};

WM5_REGISTER_STREAM(Polysegment);
typedef Pointer0<Polysegment> PolysegmentPtr;
#include "Wm5Polysegment.inl"

}

#endif
