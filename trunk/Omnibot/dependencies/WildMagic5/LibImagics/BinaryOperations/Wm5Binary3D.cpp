// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/04/30)

#include "Wm5ImagicsPCH.h"
#include "Wm5Binary3D.h"
using namespace Wm5;

//----------------------------------------------------------------------------
void Binary3D::GetComponents26 (ImageInt3D& image, bool storeZeros,
    std::vector<IndexArray>& components)
{
    // Incremental 1D offsets for 26-connected neighbors.  Store +1 and -1
    // first, the xy-offsets second, to be cache friendly during the
    // depth-first search.
    const int bound0 = image.GetBound(0);
    const int bound01 = bound0*image.GetBound(1);
    const int delta[26] =
    {
        +1,
        -1,
        -1 - bound0,
         0 - bound0,
        +1 - bound0,
        -1 + bound0,
         0 + bound0,
        +1 + bound0,
         0          + bound01,
        +1          + bound01,
        -1          + bound01,
        -1 - bound0 + bound01,
         0 - bound0 + bound01,
        +1 - bound0 + bound01,
        -1 + bound0 + bound01,
         0 + bound0 + bound01,
        +1 + bound0 + bound01,
         0          - bound01,
        +1          - bound01,
        -1          - bound01,
        -1 - bound0 - bound01,
         0 - bound0 - bound01,
        +1 - bound0 - bound01,
        -1 + bound0 - bound01,
         0 + bound0 - bound01,
        +1 + bound0 - bound01
    };

    GetComponents(26, delta, storeZeros, image, components);
}
//----------------------------------------------------------------------------
void Binary3D::GetComponents18 (ImageInt3D& image, bool storeZeros,
    std::vector<IndexArray>& components)
{
    // Incremental 1D offsets for 18-connected neighbors.  Store +1 and -1
    // first, the xy-offsets second, to be cache friendly during the
    // depth-first search.
    const int bound0 = image.GetBound(0);
    const int bound01 = bound0*image.GetBound(1);
    const int delta[18] =
    {
        +1,
        -1,
        -1 - bound0,
         0 - bound0,
        +1 - bound0,
        -1 + bound0,
         0 + bound0,
        +1 + bound0,
         0          + bound01,
        +1          + bound01,
        -1          + bound01,
         0 - bound0 + bound01,
         0 + bound0 + bound01,
         0          - bound01,
        +1          - bound01,
        -1          - bound01,
         0 - bound0 - bound01,
         0 + bound0 - bound01
    };

    GetComponents(18, delta, storeZeros, image, components);
}
//----------------------------------------------------------------------------
void Binary3D::GetComponents6 (ImageInt3D& image, bool storeZeros,
    std::vector<IndexArray>& components)
{
    // Incremental 1D offsets for 6-connected neighbors.  Store +1 and -1
    // first to be cache friendly during the depth-first search.
    const int bound0 = image.GetBound(0);
    const int bound01 = bound0*image.GetBound(1);
    const int delta[6] =
    {
        +1,
        -1,
        -bound0,
        +bound0,
        +bound01,
        -bound01
    };

    GetComponents(6, delta, storeZeros, image, components);
}
//----------------------------------------------------------------------------
void Binary3D::GetComponents (const int numNeighbors, const int delta[],
    bool storeZeros, ImageInt3D& image, std::vector<IndexArray>& components)
{
    const int numVoxels = image.GetQuantity();
    int* numElements = new1<int>(numVoxels);
    int i, numComponents = 0, label = 2;
    int* vstack = new1<int>(numVoxels);
    for (i = 0; i < numVoxels; ++i)
    {
        if (image[i] == 1)
        {
            int top = -1;
            vstack[++top] = i;

            int& count = numElements[numComponents + 1];
            count = 0;
            while (top >= 0)
            {
                int v = vstack[top];
                image[v] = -1;
                int j;
                for (j = 0; j < numNeighbors; ++j)
                {
                    int adj = v + delta[j];
                    if (image[adj] == 1)
                    {
                        vstack[++top] = adj;
                        break;
                    }
                }
                if (j == numNeighbors)
                {
                    image[v] = label;
                    ++count;
                    --top;
                }
            }

            ++numComponents;
            ++label;
        }
    }
    delete1(vstack);

    if (storeZeros)
    {
        if (numComponents > 0)
        {
            int numZeros = numVoxels;
            components.resize(numComponents + 1);
            for (i = 1; i <= numComponents; ++i)
            {
                int count = numElements[i];
                components[i].resize(count);
                numZeros -= count;
                numElements[i] = 0;
            }
            components[0].resize(numZeros);
            numZeros = 0;
            int* zeros = &components[0][0];

            for (i = 0; i < numVoxels; ++i)
            {
                int value = image[i];
                if (value != 0)
                {
                    // Labels started at 2 to support the depth-first search,
                    // so they need to be decremented for the correct labels.
                    image[i] = --value;
                    components[value][numElements[value]] = i;
                    ++numElements[value];
                }
                else
                {
                    zeros[numZeros++] = i;
                }
            }
        }
        else
        {
            components.resize(1);
            components[0].resize(numVoxels);
            int* zeros = &components[0][0];
            for (i = 0; i < numVoxels; ++i)
            {
                zeros[i] = i;
            }
        }
    }
    else
    {
        if (numComponents > 0)
        {
            components.resize(numComponents + 1);
            for (i = 1; i <= numComponents; ++i)
            {
                components[i].resize(numElements[i]);
                numElements[i] = 0;
            }

            for (i = 0; i < numVoxels; ++i)
            {
                int value = image[i];
                if (value != 0)
                {
                    // Labels started at 2 to support the depth-first search,
                    // so they need to be decremented for the correct labels.
                    image[i] = --value;
                    components[value][numElements[value]] = i;
                    ++numElements[value];
                }
            }
        }
    }
    delete1(numElements);
}
//----------------------------------------------------------------------------
