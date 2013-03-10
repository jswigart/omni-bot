// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/18)

#include "Wm5ImagicsPCH.h"
#include "Wm5Lattice.h"
using namespace Wm5;

const char* Lattice::msHeader = "Magic Image";

//----------------------------------------------------------------------------
Lattice::Lattice (int numDimensions, int* bounds)
{
#ifdef WM5_USE_ASSERT
    assertion(numDimensions > 0 && bounds, "Invalid inputs\n");
    for (int i = 0; i < numDimensions; ++i)
    {
        assertion(bounds[i] > 0, "Bounds must be positive\n");
    }
#endif

    mNumDimensions = numDimensions;
    mBounds = bounds;
    mOffsets = new1<int>(numDimensions);

    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
Lattice::Lattice (const Lattice& lattice)
{
    mNumDimensions = lattice.mNumDimensions;
    mQuantity = lattice.mQuantity;
    mBounds = new1<int>(mNumDimensions);
    mOffsets = new1<int>(mNumDimensions);

    size_t numBytes = mNumDimensions*sizeof(int);
    memcpy(mBounds, lattice.mBounds, numBytes);
    memcpy(mOffsets, lattice.mOffsets, numBytes);
}
//----------------------------------------------------------------------------
Lattice::Lattice (int numDimensions)
{
    assertion(numDimensions > 0, "Dimensions must be positive\n");

    mNumDimensions = numDimensions;
    mBounds = 0;
    mOffsets = new1<int>(numDimensions);
    memset(mOffsets, 0, numDimensions*sizeof(int));
    mQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::Lattice ()
{
    mNumDimensions = 0;
    mBounds = 0;
    mOffsets = 0;
    mQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::~Lattice ()
{
    delete1(mBounds);
    delete1(mOffsets);
}
//----------------------------------------------------------------------------
void Lattice::SetBounds (int* bounds)
{
#ifdef WM5_USE_ASSERT
    assertion(bounds != 0, "Bounds must be specified\n");
    for (int i = 0; i < mNumDimensions; i++)
    {
        assertion(bounds[i] > 0, "Bounds must be positive\n");
    }
#endif

    mBounds = bounds;
    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
void Lattice::ComputeQuantityAndOffsets ()
{
    int i;

    // Calculate number of lattice points.
    mQuantity = 1;
    for (i = 0; i < mNumDimensions; ++i)
    {
        mQuantity *= mBounds[i];
    }

    // Calculate offset indices of neighboring lattice points.
    mOffsets[0] = 1;
    for (i = 1; i < mNumDimensions; ++i)
    {
        mOffsets[i] = mBounds[i - 1]*mOffsets[i - 1];
    }
}
//----------------------------------------------------------------------------
Lattice& Lattice::operator= (const Lattice& lattice)
{
    if (mNumDimensions != lattice.mNumDimensions)
    {
        delete1(mBounds);
        delete1(mOffsets);
        mNumDimensions = lattice.mNumDimensions;
        mQuantity = lattice.mQuantity;
        mBounds = new1<int>(mNumDimensions);
        mOffsets = new1<int>(mNumDimensions);
    }

    size_t numBytes = mNumDimensions*sizeof(int);
    memcpy(mBounds, lattice.mBounds, numBytes);
    memcpy(mOffsets, lattice.mOffsets, numBytes);
    mQuantity = lattice.mQuantity;

    return *this;
}
//----------------------------------------------------------------------------
bool Lattice::operator== (const Lattice& lattice) const
{
    if (mNumDimensions != lattice.mNumDimensions)
    {
        return false;
    }

    int numBytes = mNumDimensions*sizeof(int);
    return memcmp(mBounds, lattice.mBounds, numBytes) == 0;
}
//----------------------------------------------------------------------------
bool Lattice::operator!= (const Lattice& lattice) const
{
    return !operator==(lattice);
}
//----------------------------------------------------------------------------
int Lattice::GetIndex (const int* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    int index = coord[0];
    for (int i = 1; i < mNumDimensions; ++i)
    {
        index += mOffsets[i]*coord[i];
    }
    return index;
}
//----------------------------------------------------------------------------
void Lattice::GetCoordinates (int index, int* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    for (int i = 0; i < mNumDimensions; ++i)
    {
        coord[i] = index % mBounds[i];
        index /= mBounds[i];
    }
}
//----------------------------------------------------------------------------
bool Lattice::Load (FileIO& inFile)
{
    int numBytes = (int)strlen(msHeader) + 1;
    char* buffer = new1<char>(numBytes);
    inFile.Read(sizeof(char), numBytes, buffer);
    buffer[numBytes-1] = 0;
    if (strncmp(buffer, msHeader, numBytes) != 0)
    {
        delete1(buffer);
        mNumDimensions = 0;
        mQuantity = 0;
        mBounds = 0;
        mOffsets = 0;
        return false;
    }
    delete1(buffer);

    inFile.Read(sizeof(int), &mNumDimensions);

    delete1(mBounds);
    mBounds = new1<int>(mNumDimensions);
    inFile.Read(sizeof(int), mNumDimensions, mBounds);

    delete1(mOffsets);
    mOffsets = new1<int>(mNumDimensions);

    ComputeQuantityAndOffsets();

    return true;
}
//----------------------------------------------------------------------------
bool Lattice::Save (FileIO& outFile) const
{
    outFile.Write(sizeof(char), (int)strlen(msHeader) + 1, msHeader);
    outFile.Write(sizeof(int), &mNumDimensions);
    outFile.Write(sizeof(int), mNumDimensions, mBounds);
    return true;
}
//----------------------------------------------------------------------------
bool Lattice::LoadRaw (const char* filename, int& numDimensions,
    int*& bounds, int& quantity, int& rtti, int& sizeOf, char*& data)
{
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);
    if (!inFile)
    {
        return false;
    }

    int numBytes = (int)strlen(msHeader) + 1;
    char* buffer = new1<char>(numBytes);
    inFile.Read(sizeof(char), numBytes, buffer);
    buffer[numBytes-1] = 0;
    if (strncmp(buffer, msHeader, numBytes) != 0)
    {
        delete1(buffer);
        inFile.Close();
        return false;
    }
    delete1(buffer);

    inFile.Read(sizeof(int), &numDimensions);

    bounds = new1<int>(numDimensions);
    inFile.Read(sizeof(int), numDimensions, bounds);

    quantity = 1;
    for (int i = 0; i < numDimensions; ++i)
    {
        quantity *= bounds[i];
    }

    inFile.Read(sizeof(int), &rtti);
    inFile.Read(sizeof(int), &sizeOf);

    data = new1<char>(quantity*sizeOf);
    inFile.Read(sizeOf, quantity, data);

    return true;
}
//----------------------------------------------------------------------------
