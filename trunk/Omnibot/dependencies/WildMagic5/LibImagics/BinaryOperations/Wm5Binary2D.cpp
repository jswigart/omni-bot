// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/04/30)

#include "Wm5ImagicsPCH.h"
#include "Wm5Binary2D.h"
using namespace Wm5;

//----------------------------------------------------------------------------
// Extraction of boundary from binary objects in image.
//
// Directions are:  W=0, NW=1, N=2, NE=3, E=4, SE=5, S=6, SW=7.  If a pixel
// was reached from the direction indicated, then its eight neighbors are
// searched in the order shown:
//
//     W      NW     N      NE     E      SE     S      SW
//     0 1 2  7 0 1  6 7 0  5 6 7  4 5 6  3 4 5  2 3 4  1 2 3
//     7 * 3  6 * 2  5 * 1  4 * 0  3 * 7  2 * 6  1 * 5  0 * 4
//     6 5 4  5 4 3  4 3 2  3 2 1  2 1 0  1 0 7  0 7 6  7 6 5
//
// The formula for the next direction is:  newdir = (index+5+olddir) MOD 8
// where index is the number of the neighbor in the search.  For example, in
// the configuration,
//
//   010
//   010
//   001
//
// suppose the center pixel was reached from the North (olddir=2).  Neighbors
// at index=0 and index=1 are zero.  The first nonzero neighbor is at index=2.
// Thus, newdir = (2+5+2) MOD 8 = 1, so the direction from which the next
// pixel is reached is NorthWest.
//
// One-pixel thick structures are treated as having more thickness in the
// sense that they are traversed twice, once per "side".
//----------------------------------------------------------------------------
void Binary2D::GetBoundaries (ImageInt2D& image,
    std::vector<IndexArray>& boundaries)
{
    // Background pixels are 0 and foreground pixels are 1.  Set the interior
    // foreground pixels to 2 and the boundary pixels to 3.  A 3 indicates
    // the boundary pixel has not yet been included in a boundary find.
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound0m1 = bound0 - 1;
    const int bound1m1 = bound1 - 1;
    int x, y;
    for (y = 1; y < bound1m1; ++y)
    {
        for (x = 1; x < bound0m1; ++x)
        {
            if (image(x, y) != 0)
            {
                if (image(x-1, y) != 0 && image(x+1, y) != 0
                &&  image(x, y-1) != 0 && image(x, y+1) != 0)
                {
                    image(x, y) = 2;
                }
                else
                {
                    image(x, y) = 3;
                }
            }
        }
    }

    // Search image for boundary points and extract the full boundaries.
    for (y = 1; y < bound1m1; ++y)
    {
        for (x = 0; x < bound0m1; ++x)
        {
            if (image(x, y) == 3)
            {
                boundaries.push_back(std::vector<int>());
                ExtractBoundary(x, y, image, boundaries.back());
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::ExtractBoundary (int x0, int y0, ImageInt2D& image,
    IndexArray& boundary)
{
    // Incremental 2D offsets for 8-connected neighborhood of (x,y).
    const int dx[8] = { -1,  0, +1, +1, +1,  0, -1, -1 };
    const int dy[8] = { -1, -1, -1,  0, +1, +1, +1,  0 };

    // Insert the initial boundary point.
    boundary.push_back(image.GetIndex(x0, y0));

    // Compute the direction from background (0) to boundary pixel (1).
    int cx = x0, cy = y0;
    int nx = 0, ny = 0, dir;
    for (dir = 0; dir < 8; ++dir)
    {
        nx = cx + dx[dir];
        ny = cy + dy[dir];
        if (image(nx, ny) == 0)
        {
            dir = (dir + 1) % 8;
            break;
        }
    }

    // Traverse boundary in clockwise order.  Mark visited pixels with 1.
    image(cx, cy) = 1;
    bool notDone = true;
    while (notDone)
    {
        int i, nbr;
        for (i = 0, nbr = dir; i < 8; ++i, nbr = (nbr + 1) % 8)
        {
            nx = cx + dx[nbr];
            ny = cy + dy[nbr];
            if (image(nx, ny))
            {
                // The next boundary pixel was found.
                break;
            }
        }

        if (i == 8)
        {
            // (cx,cy) is an isolated pixel.
            notDone = false;
            continue;
        }

        if (nx == x0 && ny == y0)
        {
            // The boundary traversal is completed.
            notDone = false;
            continue;
        }

        // (nx,ny) is the next boundary point, so add the pixel to the list.
        boundary.push_back(image.GetIndex(nx, ny));

        // Mark visited pixels with 1.
        image(nx, ny) = 1;

        // Start the search for the next boundary point.
        cx = nx;
        cy = ny;
        dir = (i + 5 + dir) % 8;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Connected component labeling.  The algorithm is simply a nonrecursive
// depth-first search of the graph.
//----------------------------------------------------------------------------
void Binary2D::GetComponents8 (ImageInt2D& image, bool storeZeros,
    std::vector<IndexArray>& components)
{
    // Incremental 1D offsets for 8-connected neighbors.  Store +1 and -1
    // first to be cache friendly during the depth-first search.
    const int bound0 = image.GetBound(0);
    const int delta[8] =
    {
        +1,
        -1,
        -1 - bound0,
           - bound0,
        +1 - bound0,
        -1 + bound0,
           + bound0,
        +1 + bound0
    };

    GetComponents(8, delta, storeZeros, image, components);
}
//----------------------------------------------------------------------------
void Binary2D::GetComponents4 (ImageInt2D& image, bool storeZeros,
    std::vector<IndexArray>& components)
{
    // Incremental 1D offsets for 8-connected neighbors.  Store +1 and -1
    // first to be cache friendly during the depth-first search.
    const int bound0 = image.GetBound(0);
    const int delta[4] =
    {
        +1,
        -1,
        -bound0,
        bound0
    };

    GetComponents(4, delta, storeZeros, image, components);
}
//----------------------------------------------------------------------------
void Binary2D::GetComponents (const int numNeighbors, const int delta[],
    bool storeZeros, ImageInt2D& image, std::vector<IndexArray>& components)
{
    const int numPixels = image.GetQuantity();
    int* numElements = new1<int>(numPixels);
    int i, numComponents = 0, label = 2;
    int* vstack = new1<int>(numPixels);
    for (i = 0; i < numPixels; ++i)
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
            int numZeros = numPixels;
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

            for (i = 0; i < numPixels; ++i)
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
            components[0].resize(numPixels);
            int* zeros = &components[0][0];
            for (i = 0; i < numPixels; ++i)
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

            for (i = 0; i < numPixels; ++i)
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

//----------------------------------------------------------------------------
// L1 Distance Transform.
//
// This distance is also known as the "city block" distance or whatever.
// North, South, East, and West neighbors are 1 unit away.  NorthWest,
// NorthEast, SouthWest, and SouthEast neighbors are 2 units away.
//----------------------------------------------------------------------------
void Binary2D::GetL1Distance (ImageInt2D& image, int& maxDistance)
{
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound0m1 = bound0 - 1;
    const int bound1m1 = bound1 - 1;

    // Use a grass-fire approach, computing distance from boundary to
    // interior one pass at a time.
    bool changeMade = true;
    int distance;
    for (distance = 1; changeMade; ++distance)
    {
        changeMade = false;
        int distanceP1 = distance + 1;
        for (int y = 1; y < bound1m1; ++y)
        {
            for (int x = 1; x < bound0m1; ++x)
            {
                if (image(x, y) == distance)
                {
                    if (image(x-1, y) >= distance
                    &&  image(x+1, y) >= distance
                    &&  image(x, y-1) >= distance
                    &&  image(x, y+1) >= distance)
                    {
                        image(x, y) = distanceP1;
                        changeMade = true;
                    }
                }
            }
        }
    }
    maxDistance = distance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// L2 Distance Transform (Euclidean Distance Transform)
//
// This program calculates the Euclidean distance transform of a binary
// input image.  The adaptive algorithm is guaranteed to give exact
// distances for all distances < 100.  Algorithm sent to me by John Gauch.
//
// From John Gauch at University of Kansas:
// The basic idea is similar to a EDT described recently in PAMI by Laymarie
// from McGill.  By keeping the dx and dy offset to the nearest edge (feature)
// point in the image, we can search to see which dx dy is closest to a given
// point by examining a set of neighbors.  The Laymarie method (and Borgfors)
// look at a fixed 3x3 or 5x5 neighborhood and call it a day.  What we did was
// calculate (painfully) what neighborhoods you need to look at to guarentee
// that the exact distance is obtained.  Thus, you will see in the code, that
// we L2Check the current distance and depending on what we have so far, we
// extend the search region.  Since our algorithm for L2Checking the exactness
// of each neighborhood is on the order N^4, we have only gone to N=100.  In
// theory, you could make this large enough to get all distances exact.  We
// have implemented the algorithm to get all distances < 100 to be exact. 
//----------------------------------------------------------------------------
void Binary2D::GetL2Distance (const ImageInt2D& image, double& maxDistance,
    ImageDouble2D& transform)
{
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound0m1 = bound0 - 1;
    const int bound1m1 = bound1 - 1;
    int x, y, distance;

    // Create and initialize intermediate images.
    ImageInt2D xNear(bound0, bound1);
    ImageInt2D yNear(bound0, bound1);
    ImageInt2D dist(bound0, bound1);
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            if (image(x, y) != 0)
            {
                xNear(x, y) = 0;
                yNear(x, y) = 0;
                dist(x, y) = INT_MAX;
            }
            else
            {
                xNear(x, y) = x;
                yNear(x, y) = y;
                dist(x, y) = 0;
            }
        }
    }

    const int K1 = 1;
    const int K2 = 169;   // 13^2
    const int K3 = 961;   // 31^2
    const int K4 = 2401;  // 49^2
    const int K5 = 5184;  // 72^2

    // Pass in the ++ direction.
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            distance = dist(x, y);
            if (distance > K1)
            { 
                L2Check(x, y, -1,  0, xNear, yNear, dist); 
                L2Check(x, y, -1, -1, xNear, yNear, dist); 
                L2Check(x, y,  0, -1, xNear, yNear, dist); 
            }
            if (distance > K2)
            { 
                L2Check(x, y, -2, -1, xNear, yNear, dist); 
                L2Check(x, y, -1, -2, xNear, yNear, dist); 
            }
            if (distance > K3)
            { 
                L2Check(x, y, -3, -1, xNear, yNear, dist); 
                L2Check(x, y, -3, -2, xNear, yNear, dist); 
                L2Check(x, y, -2, -3, xNear, yNear, dist); 
                L2Check(x, y, -1, -3, xNear, yNear, dist); 
            }
            if (distance > K4)
            { 
                L2Check(x, y, -4, -1, xNear, yNear, dist); 
                L2Check(x, y, -4, -3, xNear, yNear, dist); 
                L2Check(x, y, -3, -4, xNear, yNear, dist); 
                L2Check(x, y, -1, -4, xNear, yNear, dist); 
            }
            if (distance > K5)
            { 
                L2Check(x, y, -5, -1, xNear, yNear, dist); 
                L2Check(x, y, -5, -2, xNear, yNear, dist); 
                L2Check(x, y, -5, -3, xNear, yNear, dist); 
                L2Check(x, y, -5, -4, xNear, yNear, dist);
                L2Check(x, y, -4, -5, xNear, yNear, dist); 
                L2Check(x, y, -2, -5, xNear, yNear, dist); 
                L2Check(x, y, -3, -5, xNear, yNear, dist); 
                L2Check(x, y, -1, -5, xNear, yNear, dist); 
            }
        }
    }

    // Pass in -- direction.
    for (y = bound1m1; y >= 0; --y)
    {
        for (x = bound0m1; x >= 0; --x)
        {
            distance = dist(x, y);
            if (distance > K1)
            { 
                L2Check(x, y, 1, 0, xNear, yNear, dist); 
                L2Check(x, y, 1, 1, xNear, yNear, dist); 
                L2Check(x, y, 0, 1, xNear, yNear, dist); 
            }
            if (distance > K2)
            { 
                L2Check(x, y, 2, 1, xNear, yNear, dist); 
                L2Check(x, y, 1, 2, xNear, yNear, dist); 
            }
            if (distance > K3)
            { 
                L2Check(x, y, 3, 1, xNear, yNear, dist); 
                L2Check(x, y, 3, 2, xNear, yNear, dist); 
                L2Check(x, y, 2, 3, xNear, yNear, dist); 
                L2Check(x, y, 1, 3, xNear, yNear, dist); 
            }
            if (distance > K4)
            { 
                L2Check(x, y, 4, 1, xNear, yNear, dist); 
                L2Check(x, y, 4, 3, xNear, yNear, dist); 
                L2Check(x, y, 3, 4, xNear, yNear, dist); 
                L2Check(x, y, 1, 4, xNear, yNear, dist); 
            }
            if (distance > K5)
            { 
                L2Check(x, y, 5, 1, xNear, yNear, dist); 
                L2Check(x, y, 5, 2, xNear, yNear, dist); 
                L2Check(x, y, 5, 3, xNear, yNear, dist); 
                L2Check(x, y, 5, 4, xNear, yNear, dist);
                L2Check(x, y, 4, 5, xNear, yNear, dist); 
                L2Check(x, y, 2, 5, xNear, yNear, dist); 
                L2Check(x, y, 3, 5, xNear, yNear, dist); 
                L2Check(x, y, 1, 5, xNear, yNear, dist); 
            }
        }
    }

    // Pass in the +- direction.
    for (y = bound1m1; y >= 0; --y)
    {
        for (x = 0; x < bound0; ++x)
        {
            distance = dist(x, y);
            if (distance > K1)
            { 
                L2Check(x, y, -1, 0, xNear, yNear, dist); 
                L2Check(x, y, -1, 1, xNear, yNear, dist); 
                L2Check(x, y,  0, 1, xNear, yNear, dist); 
            }
            if (distance > K2)
            { 
                L2Check(x, y, -2, 1, xNear, yNear, dist); 
                L2Check(x, y, -1, 2, xNear, yNear, dist); 
            }
            if (distance > K3)
            { 
                L2Check(x, y, -3, 1, xNear, yNear, dist); 
                L2Check(x, y, -3, 2, xNear, yNear, dist); 
                L2Check(x, y, -2, 3, xNear, yNear, dist); 
                L2Check(x, y, -1, 3, xNear, yNear, dist); 
            }
            if (distance > K4)
            { 
                L2Check(x, y, -4, 1, xNear, yNear, dist); 
                L2Check(x, y, -4, 3, xNear, yNear, dist); 
                L2Check(x, y, -3, 4, xNear, yNear, dist); 
                L2Check(x, y, -1, 4, xNear, yNear, dist); 
            }
            if (distance > K5)
            { 
                L2Check(x, y, -5, 1, xNear, yNear, dist); 
                L2Check(x, y, -5, 2, xNear, yNear, dist); 
                L2Check(x, y, -5, 3, xNear, yNear, dist); 
                L2Check(x, y, -5, 4, xNear, yNear, dist);
                L2Check(x, y, -4, 5, xNear, yNear, dist); 
                L2Check(x, y, -2, 5, xNear, yNear, dist); 
                L2Check(x, y, -3, 5, xNear, yNear, dist); 
                L2Check(x, y, -1, 5, xNear, yNear, dist); 
            }
        }
    }

    // Pass in the -+ direction.
    for (y = 0; y < bound1; ++y)
    {
        for (x = bound0m1; x >= 0; --x)
        {
            distance = dist(x, y);
            if (distance > K1)
            { 
                L2Check(x, y, 1,  0, xNear, yNear, dist); 
                L2Check(x, y, 1, -1, xNear, yNear, dist); 
                L2Check(x, y, 0, -1, xNear, yNear, dist); 
            }
            if (distance > K2)
            { 
                L2Check(x, y, 2, -1, xNear, yNear, dist); 
                L2Check(x, y, 1, -2, xNear, yNear, dist); 
            }
            if (distance > K3)
            { 
                L2Check(x, y, 3, -1, xNear, yNear, dist); 
                L2Check(x, y, 3, -2, xNear, yNear, dist); 
                L2Check(x, y, 2, -3, xNear, yNear, dist); 
                L2Check(x, y, 1, -3, xNear, yNear, dist); 
            }
            if (distance > K4)
            { 
                L2Check(x, y, 4, -1, xNear, yNear, dist); 
                L2Check(x, y, 4, -3, xNear, yNear, dist); 
                L2Check(x, y, 3, -4, xNear, yNear, dist); 
                L2Check(x, y, 1, -4, xNear, yNear, dist); 
            }
            if (distance > K5)
            { 
                L2Check(x, y, 5, -1, xNear, yNear, dist); 
                L2Check(x, y, 5, -2, xNear, yNear, dist); 
                L2Check(x, y, 5, -3, xNear, yNear, dist); 
                L2Check(x, y, 5, -4, xNear, yNear, dist);
                L2Check(x, y, 4, -5, xNear, yNear, dist); 
                L2Check(x, y, 2, -5, xNear, yNear, dist); 
                L2Check(x, y, 3, -5, xNear, yNear, dist); 
                L2Check(x, y, 1, -5, xNear, yNear, dist); 
            }
        }
    }

    maxDistance = 0.0;
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            double distance = sqrt((double)dist(x, y));
            if (distance > maxDistance)
            {
                maxDistance = distance;
            }
            transform(x, y) = distance;
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2Check (int x, int y, int dx, int dy, ImageInt2D& xNear,
    ImageInt2D& yNear, ImageInt2D& dist)
{
    const int bound0 = dist.GetBound(0);
    const int bound1 = dist.GetBound(1);
    int xp = x + dx, yp = y + dy;
    if (0 <= xp && xp < bound0 && 0 <= yp && yp < bound1)
    {
        if (dist(xp, yp) < dist(x, y))
        {
            int dx0 = xNear(xp, yp) - x;
            int dy0 = yNear(xp, yp) - y;
            int newDist = dx0*dx0 + dy0*dy0;
            if (newDist < dist(x, y))
            {
                xNear(x, y) = xNear(xp, yp);
                yNear(x, y) = yNear(xp, yp);
                dist(x, y) = newDist;
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Skeletonization.
//
// Boundary pixels are trimmed from the object one layer at a time based on
// their adjacency to interior pixels.  At each step the connectivity and
// cycles of the object are preserved.
//----------------------------------------------------------------------------
void Binary2D::GetSkeleton (ImageInt2D& image)
{
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);

    // Trim pixels, mark interior as 4.
    bool notDone = true;
    while (notDone)
    {
        if (MarkInterior(image, 4, Interior4))
        {
            // No interior pixels, trimmed set is at most 2-pixels thick.
            notDone = false;
            continue;
        }

        if (ClearInteriorAdjacent(image, 4))
        {
            // All remaining interior pixels are either articulation points
            // or part of blobs whose boundary pixels are all articulation
            // points.  An example of the latter case is shown below.  The
            // background pixels are marked with '.' rather than '0' for
            // readability.  The interior pixels are marked with '4' and the
            // boundary pixels are marked with '1'.
            //
            //   .........
            //   .....1...
            //   ..1.1.1..
            //   .1.141...
            //   ..14441..
            //   ..1441.1.
            //   .1.11.1..
            //   ..1..1...
            //   .........
            //
            // This is a pathological problem where there are many small holes
            // (0-pixel with north, south, west, and east neighbors all
            // 1-pixels) that your application can try to avoid by an initial
            // pass over the image to fill in such holes.  Of course, you do
            // have problems with checkerboard patterns...
            notDone = false;
            continue;
        }
    }

    // Trim pixels, mark interior as 3.
    notDone = true;
    while (notDone)
    {
        if (MarkInterior(image, 3, Interior3))
        {
            // No interior pixels, trimmed set is at most 2-pixels thick.
            notDone = false;
            continue;
        }

        if (ClearInteriorAdjacent(image, 3))
        {
            // All remaining 3-values can be safely removed since they are
            // not articulation points and the removal will not cause new
            // holes.
            for (int y = 0; y < bound1; ++y)
            {
                for (int x = 0; x < bound0; ++x)
                {
                    if (image(x, y) == 3 && !IsArticulation(image, x, y))
                    {
                        image(x, y) = 0;
                    }
                }
            }
            notDone = false;
            continue;
        }
    }

    // Trim pixels, mark interior as 2.
    notDone = true;
    while (notDone)
    {
        if (MarkInterior(image, 2, Interior2))
        {
            // No interior pixels, trimmed set is at most 1-pixel thick.
            // Call it a skeleton.
            notDone = false;
            continue;
        }

        if (ClearInteriorAdjacent(image, 2))
        {
            // Removes 2-values that are not articulation points.
            for (int y = 0; y < bound1; ++y)
            {
                for (int x = 0; x < bound0; ++x)
                {
                    if (image(x, y) == 2 && !IsArticulation(image, x, y))
                    {
                        image(x, y) = 0;
                    }
                }
            }
            notDone = false;
            continue;
        }
    }

    // Make the skeleton a binary image.
    for (int i = 0; i < image.GetQuantity(); ++i)
    {
        if (image[i] != 0)
        {
            image[i] = 1;
        }
    }
}
//----------------------------------------------------------------------------
bool Binary2D::Interior4 (ImageInt2D& image, int x, int y)
{
    return image(x-1, y) != 0
        && image(x+1, y) != 0
        && image(x, y-1) != 0
        && image(x, y+1) != 0;
}
//----------------------------------------------------------------------------
bool Binary2D::Interior3 (ImageInt2D& image, int x, int y)
{
    int numNeighbors = 0;
    if (image(x-1, y) != 0)
    {
        ++numNeighbors;
    }
    if (image(x+1, y) != 0)
    {
        ++numNeighbors;
    }
    if (image(x, y-1) != 0)
    {
        ++numNeighbors;
    }
    if (image(x, y+1) != 0)
    {
        ++numNeighbors;
    }
    return numNeighbors == 3;
}
//----------------------------------------------------------------------------
bool Binary2D::Interior2 (ImageInt2D& image, int x, int y)
{
    bool b1 = (image(x, y-1) != 0);
    bool b3 = (image(x+1, y) != 0);
    bool b5 = (image(x, y+1) != 0);
    bool b7 = (image(x-1, y) != 0);
    return (b1 && b3) || (b3 && b5) || (b5 && b7) || (b7 && b1);
}
//----------------------------------------------------------------------------
bool Binary2D::MarkInterior (ImageInt2D& image, int value,
    InteriorFunction function)
{
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    bool noInterior = true;
    for (int y = 0; y < bound1; ++y)
    {
        for (int x = 0; x < bound0; ++x)
        {
            if (image(x, y) > 0)
            {
                if (function(image, x, y))
                {
                    image(x, y) = value;
                    noInterior = false;
                }
                else
                {
                    image(x, y) = 1;
                }
            }
        }
    }
    return noInterior;
}
//----------------------------------------------------------------------------
bool Binary2D::IsArticulation (ImageInt2D& image, int x, int y)
{
    // Converts 8 neighbors of pixel (x,y) to an 8-bit value, bit = 1 iff
    // pixel is set.
    int byteMask = 0;
    if (image(x-1, y-1) != 0)
    {
        byteMask |= 0x01;
    }
    if (image(x, y-1) != 0)
    {
        byteMask |= 0x02;
    }
    if (image(x+1, y-1) != 0)
    {
        byteMask |= 0x04;
    }
    if (image(x+1, y) != 0)
    {
        byteMask |= 0x08;
    }
    if (image(x+1, y+1) != 0)
    {
        byteMask |= 0x10;
    }
    if (image(x, y+1) != 0)
    {
        byteMask |= 0x20;
    }
    if (image(x-1, y+1) != 0)
    {
        byteMask |= 0x40;
    }
    if (image(x-1, y) != 0)
    {
        byteMask |= 0x80;
    }

    return msArticulation[byteMask] == 1;
}
//----------------------------------------------------------------------------
bool Binary2D::ClearInteriorAdjacent (ImageInt2D& image, int value)
{
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    bool noRemoval = true;
    for (int y = 0; y < bound1; y++)
    {
        for (int x = 0; x < bound0; x++)
        {
            if (image(x,y) == 1)
            {
                bool interiorAdjacent =
                    image(x-1, y-1) == value ||
                    image(x  , y-1) == value ||
                    image(x+1, y-1) == value ||
                    image(x+1, y  ) == value ||
                    image(x+1, y+1) == value ||
                    image(x  , y+1) == value ||
                    image(x-1, y+1) == value ||
                    image(x-1, y  ) == value;

                if (interiorAdjacent && !IsArticulation(image, x, y))
                {
                    image(x, y) = 0;
                    noRemoval = false;
                }
            }
        }
    }
    return noRemoval;
}
//----------------------------------------------------------------------------
const int Binary2D::msArticulation[256] =
{
    0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,
    0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
    0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
    0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
    0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
    0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,
    0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,
    0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0
};
//----------------------------------------------------------------------------
