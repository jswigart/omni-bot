// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5LATTICE_H
#define WM5LATTICE_H

#include "Wm5ImagicsLIB.h"
#include "Wm5FileIO.h"
#include "Wm5Memory.h"

namespace Wm5
{

class WM5_IMAGICS_ITEM Lattice
{
public:
    // Abstract base class for TImage.
    virtual ~Lattice ();

    // Data access.
    inline int GetDimensions () const;
    inline const int* GetBounds () const;
    inline int GetBound (int i) const;
    inline int GetQuantity () const;
    inline const int* GetOffsets () const;
    inline int GetOffset (int i) const;

    // Assignment.
    Lattice& operator= (const Lattice& lattice);

    // Comparisons.
    bool operator== (const Lattice& lattice) const;
    bool operator!= (const Lattice& lattice) const;

    // Conversions between n-dim and 1-dim structures.  The coordinate arrays
    // must have the same number of elements as the dimensions of the lattice.
    int GetIndex (const int* coord) const;
    void GetCoordinates (int index, int* coord) const;

    // Streaming.
    bool Load (FileIO& inFile);
    bool Save (FileIO& outFile) const;

    static bool LoadRaw (const char* filename, int& numDimensions,
        int*& bounds, int& quantity, int& rtti, int& sizeOf, char*& data);

protected:
    // Construction.  Lattice accepts responsibility for deleting the
    // bound array.
    Lattice (int numDimensions, int* bounds);
    Lattice (const Lattice& lattice);
    Lattice ();

    // For deferred creation of bounds.  Lattice accepts responsibility
    // for deleting the bound array.
    Lattice (int numDimensions);
    void SetBounds (int* bounds);
    void ComputeQuantityAndOffsets ();

    int mNumDimensions;
    int* mBounds;
    int mQuantity;
    int* mOffsets;

    // Streaming.
    static const char* msHeader;
};

#include "Wm5Lattice.inl"

}

#endif
