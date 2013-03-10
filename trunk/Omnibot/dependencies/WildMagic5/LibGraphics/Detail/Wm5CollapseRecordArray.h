// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5COLLAPSERECORDARRAY_H
#define WM5COLLAPSERECORDARRAY_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5CollapseRecord.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CollapseRecordArray : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(CollapseRecordArray);

public:
    // Construction and destruction.
    CollapseRecordArray (int numRecords = 0, CollapseRecord* records = 0);
    virtual ~CollapseRecordArray ();

    // Member access.
    inline int GetNumRecords () const;
    inline CollapseRecord* GetRecords () const;

protected:
    int mNumRecords;
    CollapseRecord* mRecords;
};

WM5_REGISTER_STREAM(CollapseRecordArray);
typedef Pointer0<CollapseRecordArray> CollapseRecordArrayPtr;
#include "Wm5CollapseRecordArray.inl"

}

#endif
