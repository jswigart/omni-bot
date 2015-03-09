// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <int N, typename Real>
UniqueVerticesTriangles<N,Real>::UniqueVerticesTriangles (int numTriangles,
    const Tuple<N,Real>* inVertices, int& numOutVertices,
    Tuple<N,Real>*& outVertices, int*& outIndices)
{
    assertion(numTriangles > 0 && inVertices,
        "Invalid inputs to UniqueVerticesTriangles constructor\n");

    ConstructUniqueVertices(3*numTriangles,inVertices,numOutVertices,
        outVertices);

    // The input index array is implicitly {<0,1,2>,<3,4,5>,...,<n-3,n-2,n-1>}
    // where n is the number of vertices.  The output index array is the same
    // as the mapping array.
    int numIndices = 3*numTriangles;
    outIndices = new1<int>(numIndices);
    size_t numBytes = numIndices*sizeof(int);
    memcpy(outIndices, mInToOutMapping, numBytes);
}
//----------------------------------------------------------------------------
template <int N, typename Real>
UniqueVerticesTriangles<N,Real>::UniqueVerticesTriangles (int numInVertices,
    const Tuple<N,Real>* inVertices, int numTriangles, const int* inIndices,
    int& numOutVertices, Tuple<N,Real>*& outVertices, int*& outIndices)
{
    assertion(
        numInVertices > 0 && inVertices && numTriangles > 0 && inIndices,
        "Invalid inputs to UniqueVerticesTriangles constructor\n");

    ConstructUniqueVertices(numInVertices, inVertices, numOutVertices,
        outVertices);

    // The input index array needs it indices mapped to the unique vertex
    // indices.
    int numIndices = 3*numTriangles;
    outIndices = new1<int>(numIndices);
    for (int i = 0; i < numIndices; ++i)
    {
        assertion(0 <= inIndices[i] && inIndices[i] < numInVertices,
            "Invalid condition in UniqueVerticesTriangles constructor\n");

        outIndices[i] = mInToOutMapping[inIndices[i]];

        assertion(0 <= outIndices[i] && outIndices[i] < numOutVertices,
            "Invalid condition in UniqueVerticesTriangles constructor\n");
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
UniqueVerticesTriangles<N,Real>::~UniqueVerticesTriangles ()
{
    delete1(mInToOutMapping);
}
//----------------------------------------------------------------------------
template <int N, typename Real>
int UniqueVerticesTriangles<N,Real>::GetOutputIndexFor (int inputIndex) const
{
    assertion(0 <= inputIndex && inputIndex < mNumInVertices,
        "Invalid condition in GetOutputIndexFor\n");

    return mInToOutMapping[inputIndex];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void UniqueVerticesTriangles<N,Real>::ConstructUniqueVertices (
    int numInVertices, const Tuple<N,Real>* inVertices, int& numOutVertices,
    Tuple<N,Real>*& outVertices)
{
    // Construct the unique vertices.
    mNumInVertices = numInVertices;
    mInToOutMapping = new1<int>(mNumInVertices);
    std::map<Tuple<N,Real>,int> table;
    mNumOutVertices = 0;
    typename std::map<Tuple<N,Real>,int>::iterator iter;
    for (int i = 0; i < mNumInVertices; ++i)
    {
        iter = table.find(inVertices[i]);
        if (iter != table.end())
        {
            // Vertex i is a duplicate of one inserted earlier into the
            // table.  Map vertex i to the first-found copy.
            mInToOutMapping[i] = iter->second;
        }
        else
        {
            // Vertex i is the first occurrence of such a point.
            table.insert(std::make_pair(inVertices[i], mNumOutVertices));
            mInToOutMapping[i] = mNumOutVertices;
            mNumOutVertices++;
        }
    }

    // Pack the unique vertices into an array in the correct order.
    numOutVertices = mNumOutVertices;
    outVertices = new1<Tuple<N,Real> >(mNumOutVertices);
    for (iter = table.begin(); iter != table.end(); iter++)
    {
        assertion(0 <= iter->second && iter->second < mNumOutVertices,
            "Invalid condition in ConstructUniqueVertices\n");

        outVertices[iter->second] = iter->first;
    }
}
//----------------------------------------------------------------------------
