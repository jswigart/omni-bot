// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PICKER_H
#define WM5PICKER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5PickRecord.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Picker
{
public:
    // Construction and destruction.
    Picker ();
    ~Picker ();

    // The linear component is parameterized by P + t*D, where P is a point on
    // the component (P is the origin), D is a unit-length direction, and t is
    // a scalar in the interval [tmin,tmax] with tmin < tmax.  The P and D
    // values must be in world coordinates.  The choices for tmin and tmax are
    //   line:     tmin = -Mathf::MAX_REAL, tmax = Mathf::MAX_REAL
    //   ray:      tmin = 0, tmax = Mathf::MAX_REAL
    //   segment:  tmin = 0, tmax > 0;
    //
    // A call to this function will automatically clear the Records array.
    // If you need any information from this array obtained by a previous
    // call to Execute, you must save it first.
    void Execute (Spatial* scene, const APoint& origin,
        const AVector& direction, float tmin, float tmax);

    // The following three functions return the record satisfying the
    // constraints.  They should be called only when mRecords.size() > 0.

    // Locate the record whose T value is smallest in absolute value.
    const PickRecord& GetClosestToZero () const;

    // Locate the record with nonnegative T value closest to zero.
    const PickRecord& GetClosestNonnegative () const;

    // Locate the record with nonpositive T value closest to zero.
    const PickRecord& GetClosestNonpositive () const;

    // Access to all the records for the pick operation.
    std::vector<PickRecord> Records;

private:
    // The picking occurs recursively by traversing the input scene.
    void ExecuteRecursive (Spatial* object);

    APoint mOrigin;
    AVector mDirection;
    float mTMin, mTMax;

    // The value returned if the Get* functions are called when mRecords has
    // no elements.
    static const PickRecord msInvalid;
};

}

#endif
