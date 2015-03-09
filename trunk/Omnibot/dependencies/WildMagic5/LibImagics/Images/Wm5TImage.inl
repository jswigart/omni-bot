// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename T>
TImage<T>::TImage (int numDimensions, int* bounds, T* data)
    :
    Lattice(numDimensions, bounds)
{
    SetData(data);
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>::TImage (const TImage& image)
    :
    Lattice(image)
{
    mData = new1<T>(mQuantity);
    memcpy(mData, image.mData, mQuantity*sizeof(T));
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>::TImage (const char* filename)
{
    Load(filename);
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>::TImage (int numDimensions)
    :
    Lattice(numDimensions)
{
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>::~TImage ()
{
    delete1(mData);
}
//----------------------------------------------------------------------------
template <typename T>
void TImage<T>::SetData (T* data)
{
    if (data)
    {
        mData = data;
    }
    else
    {
        assertion(mQuantity > 0, "Quantity must be positive\n");
        mData = new1<T>(mQuantity);
        memset(mData, 0, mQuantity*sizeof(T));
    }
}
//----------------------------------------------------------------------------
template <typename T>
T* TImage<T>::GetData () const
{
    return mData;
}
//----------------------------------------------------------------------------
template <typename T>
T& TImage<T>::operator[] (int i) const
{
    return mData[i];
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>& TImage<T>::operator= (const TImage& image)
{
    Lattice::operator=(image);

    delete1(mData);
    mData = new1<T>(mQuantity);
    memcpy(mData, image.mData, mQuantity*sizeof(T));

    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
TImage<T>& TImage<T>::operator= (T value)
{
    for (int i = 0; i < mQuantity; ++i)
    {
        mData[i] = value;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
bool TImage<T>::operator== (const TImage& image) const
{
    if (Lattice::operator!=(image))
    {
        return false;
    }

    return memcmp(mData, image.mData, mQuantity*sizeof(T)) == 0;
}
//----------------------------------------------------------------------------
template <typename T>
bool TImage<T>::operator!= (const TImage& image) const
{
    return !operator==(image);
}
//----------------------------------------------------------------------------
template <typename T>
bool TImage<T>::Load (const char* filename)
{
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);
    if (!inFile)
    {
        mNumDimensions = 0;
        mQuantity = 0;
        mBounds = 0;
        mOffsets = 0;
        mData = 0;
        return false;
    }

    if (!Lattice::Load(inFile))
    {
        mData = 0;
        inFile.Close();
        return false;
    }

    int rtti, sizeOf;
    inFile.Read(sizeof(int), &rtti);
    inFile.Read(sizeof(int), &sizeOf);

    mData = new1<T>(mQuantity);
    if (rtti == T::GetRTTI())
    {
        inFile.Read(sizeof(T), mQuantity, mData);
    }
    else
    {
        char* fileData = new1<char>(mQuantity*sizeOf);
        inFile.Read(sizeOf, mQuantity, fileData);
        ImageConvert(mQuantity, rtti, fileData, T::GetRTTI(), mData);
        delete1(fileData);
    }

    inFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
bool TImage<T>::Save (const char* filename) const
{
    FileIO outFile(filename, FileIO::FM_DEFAULT_WRITE);
    if (!outFile)
    {
        return false;
    }

    if (!Lattice::Save(outFile))
    {
        outFile.Close();
        return false;
    }

    int rtti = T::GetRTTI();
    outFile.Write(sizeof(int), &rtti);

    int sizeOf = (int)(sizeof(T));
    outFile.Write(sizeof(int), &sizeOf);

    outFile.Write(sizeof(T), mQuantity, mData);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------
