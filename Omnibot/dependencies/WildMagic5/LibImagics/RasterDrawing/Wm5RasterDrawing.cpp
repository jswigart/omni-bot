// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5RasterDrawing.h"
#include "Wm5Integer.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
void Wm5::Line2D (int x0, int y0, int x1, int y1, void (*callback)(int,int))
{
    // Starting point of line.
    int x = x0, y = y0;

    // Direction of line.
    int dx = x1-x0, dy = y1-y0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // Decision parameters for pixel selection.
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    int ax = 2*dx, ay = 2*dy;
    int decX, decY;

    // Determine largest direction component, single-step related variable.
    int maxValue = dx, var = 0;
    if (dy > maxValue)
    {
        var = 1;
    }

    // Traverse Bresenham line.
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        decY = ay - dx;
        for (/**/; /**/; x += sx, decY += ay)
        {
            // Process pixel.
            callback(x, y);

            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (decY >= 0)
            {
                decY -= ax;
                y += sy;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        decX = ax - dy;
        for (/**/; /**/; y += sy, decX += ax)
        {
            // Process pixel.
            callback(x, y);

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= ay;
                x += sx;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void Wm5::Line3D (int x0, int y0, int z0, int x1, int y1, int z1,
    void (*callback)(int,int,int))
{
    // Starting point of line.
    int x = x0, y = y0, z = z0;

    // Direction of line.
    int dx = x1-x0, dy = y1-y0, dz = z1-z0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));
    int sz = (dz > 0 ? 1 : (dz < 0 ? -1 : 0));

    // Decision parameters for voxel selection.
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    if (dz < 0)
    {
        dz = -dz;
    }
    int ax = 2*dx, ay = 2*dy, az = 2*dz;
    int decX, decY, decZ;

    // Determine largest direction component, single-step related variable.
    int maxValue = dx, var = 0;
    if (dy > maxValue)
    {
        maxValue = dy;
        var = 1;
    }
    if (dz > maxValue)
    {
        var = 2;
    }

    // Traverse Bresenham line.
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        decY = ay - dx;
        decZ = az - dx;
        for (/**/; /**/; x += sx, decY += ay, decZ += az)
        {
            // Process voxel.
            callback(x, y, z);

            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (decY >= 0)
            {
                decY -= ax;
                y += sy;
            }
            if (decZ >= 0)
            {
                decZ -= ax;
                z += sz;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        decX = ax - dy;
        decZ = az - dy;
        for (/**/; /**/; y += sy, decX += ax, decZ += az)
        {
            // Process voxel.
            callback(x, y, z);

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= ay;
                x += sx;
            }
            if (decZ >= 0)
            {
                decZ -= ay;
                z += sz;
            }
        }
        break;
    case 2:  // Single-step in z-direction.
        decX = ax - dz;
        decY = ay - dz;
        for (/**/; /**/; z += sz, decX += ax, decY += ay)
        {
            // Process voxel.
            callback(x,y,z);

            // Take Bresenham step.
            if (z == z1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= az;
                x += sx;
            }
            if (decY >= 0)
            {
                decY -= az;
                y += sy;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void Wm5::Line4D (int x0, int y0, int z0, int w0, int x1, int y1, int z1,
    int w1,  void (*callback)(int,int,int,int))
{
    // Starting point of line.
    int x = x0, y = y0, z = z0, w = w0;

    // Direction of line.
    int dx = x1-x0, dy = y1-y0, dz = z1-z0, dw = w1-w0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));
    int sz = (dz > 0 ? 1 : (dz < 0 ? -1 : 0));
    int sw = (dw > 0 ? 1 : (dw < 0 ? -1 : 0));

    // Decision parameters for hypervoxel selection.
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    if (dz < 0)
    {
        dz = -dz;
    }
    if (dw < 0)
    {
        dw = -dw;
    }
    int ax = 2*dx, ay = 2*dy, az = 2*dz, aw = 2*dw;
    int decX, decY, decZ, decW;

    // Determine largest direction component, single-step related variable.
    int maxValue = dx, var = 0;
    if (dy > maxValue)
    {
        maxValue = dy;
        var = 1;
    }
    if (dz > maxValue)
    {
        maxValue = dz;
        var = 2;
    }
    if (dw > maxValue)
    {
        var = 3;
    }

    // Traverse Bresenham line.
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        decY = ay - dx;
        decZ = az - dx;
        decW = aw - dx;
        for (/**/; /**/; x += sx, decY += ay, decZ += az, decW += aw)
        {
            // Process hypervoxel.
            callback(x, y, z, w);

            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (decY >= 0)
            {
                decY -= ax;
                y += sy;
            }
            if (decZ >= 0)
            {
                decZ -= ax;
                z += sz;
            }
            if (decW >= 0)
            {
                decW -= ax;
                w += sw;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        decX = ax - dy;
        decZ = az - dy;
        decW = aw - dy;
        for (/**/; /**/; y += sy, decX += ax, decZ += az, decW += aw)
        {
            // Process hypervoxel.
            callback(x, y, z, w);

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= ay;
                x += sx;
            }
            if (decZ >= 0)
            {
                decZ -= ay;
                z += sz;
            }
            if (decW >= 0)
            {
                decW -= ay;
                w += sw;
            }
        }
        break;
    case 2:  // Single-step in z-direction.
        decX = ax - dz;
        decY = ay - dz;
        decW = aw - dz;
        for (/**/; /**/; z += sz, decX += ax, decY += ay, decW += aw)
        {
            // Process hypervoxel.
            callback(x, y, z, w);

            // Take Bresenham step.
            if (z == z1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= az;
                x += sx;
            }
            if (decY >= 0)
            {
                decY -= az;
                y += sy;
            }
            if (decW >= 0)
            {
                decW -= az;
                w += sw;
            }
        }
        break;
    case 3:  // Single-step in w-direction.
        decX = ax - dw;
        decY = ay - dw;
        decZ = az - dw;
        for (/**/; /**/; w += sw, decX += ax, decY += ay, decZ += az)
        {
            // Process hypervoxel.
            callback(x, y, z, w);

            // Take Bresenham step.
            if (w == w1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= aw;
                x += sx;
            }
            if (decY >= 0)
            {
                decY -= aw;
                y += sy;
            }
            if (decZ >= 0)
            {
                decZ -= aw;
                z += sz;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void Wm5::Circle2D (int xc, int yc, int radius, void (*callback)(int,int))
{
    for (int x = 0, y = radius, dec = 3-2*radius; x <= y; ++x)
    {
        callback(xc+x, yc+y);
        callback(xc+x, yc-y);
        callback(xc-x, yc+y);
        callback(xc-x, yc-y);
        callback(xc+y, yc+x);
        callback(xc+y, yc-x);
        callback(xc-y, yc+x);
        callback(xc-y, yc-x);

        if (dec >= 0)
        {
            dec += -4*(y--) + 4;
        }
        dec += 4*x + 6;
    }
}
//----------------------------------------------------------------------------
void Wm5::Ellipse2D (int xc, int yc, int A, int B, void (*callback)(int,int))
{
    int A2 = A*A, B2 = B*B;
    int x, y, dec;

    for (x = 0, y = B, dec = 2*B2+A2*(1-2*B); B2*x <= A2*y; ++x)
    {
        callback(xc+x, yc+y);
        callback(xc-x, yc+y);
        callback(xc+x, yc-y);
        callback(xc-x, yc-y);

        if (dec >= 0)
        {
            dec += 4*A2*(1-(y--));
        }
        dec += B2*(4*x+6);
    }
    if (y == 0 && x < A)
    {
        // The discretization caused us to reach the y-axis before the
        // x-values reached the ellipse vertices.  Draw a solid line along
        // the x-axis to those vertices.
        for (/**/; x <= A; ++x)
        {
            callback(xc+x, yc);
            callback(xc-x, yc);
        }
        return;
    }

    for (x = A, y = 0, dec = 2*A2+B2*(1-2*A); A2*y <= B2*x; ++y)
    {
        callback(xc+x, yc+y);
        callback(xc-x, yc+y);
        callback(xc+x, yc-y);
        callback(xc-x, yc-y);

        if (dec >= 0)
        {
            dec += 4*B2*(1-(x--));
        }
        dec += A2*(4*y+6);
    }
    if (x == 0 && y < B)
    {
        // The discretization caused us to reach the x-axis before the
        // y-values reached the ellipse vertices.  Draw a solid line along
        // the y-axis to those vertices.
        for (/**/; y <= B; ++y)
        {
            callback(xc, yc+y);
            callback(xc, yc-y);
        }
    }
}
//----------------------------------------------------------------------------
static void SelectEllipsePoint (int A2, int B2, float fx, float fy, int& x,
    int& y)
{
    int xFloor = int(Mathf::Floor(fx));
    int yFloor = int(Mathf::Floor(fy));
    int xIncr = B2*(2*xFloor + 1);
    int yIncr = A2*(2*yFloor + 1);
    int base = B2*xFloor*xFloor + A2*yFloor*yFloor - A2*B2;
    int a00 = abs(base);
    int a10 = abs(base + xIncr);
    int a01 = abs(base + yIncr);
    int a11 = abs(base + xIncr + yIncr);

    int minValue = a00;
    x = xFloor;
    y = yFloor;
    if (a10 < minValue)
    {
        minValue = a10;
        x = xFloor + 1;
        y = yFloor;
    }
    if (a01 < minValue)
    {
        minValue = a01;
        x = xFloor;
        y = yFloor + 1;
    }
    if (a11 < minValue)
    {
        minValue = a11;
        x = xFloor + 1;
        y = yFloor + 1;
    }
}
//----------------------------------------------------------------------------
static int WhichArc (int A2, int B2, int x, int y)
{
    if (x > 0)
    {
        if (y > 0)
        {
            return (B2*x <  A2*y ? 0 : 1);
        }
        else if (y < 0)
        {
            return (B2*x > -A2*y ? 2 : 3);
        }
        else
        {
            return 2;
        }
    }
    else if (x < 0)
    {
        if (y < 0)
        {
            return (A2*y <  B2*x ? 4 : 5);
        }
        else if (y > 0)
        {
            return (A2*y < -B2*x ? 6 : 7);
        }
        else
        {
            return 6;
        }
    }
    else
    {
        return (y > 0 ? 0 : 4);
    }
}
//----------------------------------------------------------------------------
void Wm5::EllipseArc2D (int xc, int yc, int A, int B, float fx0,
    float fy0, float fx1, float fy1, void (*callback)(int,int))
{
    // Assert (within floating point roundoff errors):
    //   (x0-xc)^2/a^2 + (y0-yc)^2/b^2 = 1
    //   (x1-xc)^2/a^2 + (y1-yc)^2/b^2 = 1
    // Assume if (x0,y0) == (x1,y1), then entire ellipse should be drawn.
    //
    // Pixels on arc are guaranteed to be traversed clockwise.

    const int A2 = A*A, B2 = B*B;

    // Get integer endpoints for arc.
    int x0, y0, x1, y1;
    SelectEllipsePoint(A2, B2, fx0 - xc, fy0 - yc, x0, y0);
    SelectEllipsePoint(A2, B2, fx1 - xc, fy1 - yc, x1, y1);

    int dx = x0 - x1, dy = y0 - y1, sqrLen = dx*dx + dy*dy;
    if (sqrLen == 1 || (sqrLen == 2 && abs(dx) == 1))
    {
        callback(xc+x0, yc+y0);
        callback(xc+x1, yc+y1);
        return;
    }

    // Determine initial case for arc drawing.
    int arc = WhichArc(A2,B2,x0,y0);
    bool notDone = true;
    while (notDone)
    {
        // Process the pixel.
        callback(xc+x0, yc+y0);

        // Determine next pixel to process.  Notation <(x,y),dy/dx> indicates
        // point on ellipse and slope at that point.
        int sigma;
        switch (arc)
        {
        case 0:  // <(0,b),0> to <(u0,v0),-1>
            x0++;
            dx++;
            sigma = B2*x0*x0 + A2*(y0-1)*(y0-1) - A2*B2;
            if (sigma >= 0)
            {
                y0--;
                dy--;
            }
            if (B2*x0 >= A2*y0)
            {
                // Slope dy/dx is no longer between 0 and -1.  Switch to next
                // arc drawer.  For large a and b, you expect to go to
                // 'arc = 1'.  But for small a or b, it is possible that the
                // next arc is so small (on the discrete raster) that it is
                // skipped.
                arc = (y0 > 0 ? 1 : 2);
            }
            break;
        case 1:  // <(u0,v0),-1> to <(a,0),infinity>
            y0--;
            dy--;
            sigma = B2*x0*x0 + A2*y0*y0 - A2*B2;
            if (sigma < 0)
            {
                x0++;
                dx++;
            }
            if (y0 == 0)
            {
                arc = 2;
            }
            break;
        case 2:  // <(a,0),infinity> to <(u1,v1),+1>
            y0--;
            dy--;
            sigma = B2*(x0-1)*(x0-1) + A2*y0*y0 - A2*B2;
            if (sigma >= 0)
            {
                x0--;
                dx--;
            }
            if (B2*x0 <= -A2*y0)
            {
                // Slope dy/dx is no longer between 0 and +1.  Switch to next
                // arc drawer.  For large a and b, you expect to go to
                // 'arc = 3'.  But for small a or b, it is possible that the
                // next arc is so small (on the discrete raster) that it is
                // skipped.
                arc = (x0 > 0 ? 3 : 4);
            }
            break;
        case 3:  // <(u1,v1),+1> to <(0,-b),0>
            x0--;
            dx--;
            sigma = B2*x0*x0 + A2*y0*y0 - A2*B2;
            if (sigma < 0)
            {
                y0--;
                dy--;
            }
            if (x0 == 0)
            {
                arc = 4;
            }
            break;
        case 4:  // <(0,-b),0> to <(u2,v2,-1)>
            x0--;
            dx--;
            sigma = B2*x0*x0 + A2*(y0+1)*(y0+1) - A2*B2;
            if (sigma >= 0)
            {
                y0++;
                dy++;
            }
            if (A2*y0 >= B2*x0)
            {
                // Slope dy/dx is no longer between 0 and -1.  Switch to next
                // arc drawer.  For large a and b, you expect to go to
                // 'arc = 5'.  But for small a or b, it is possible that the
                // next arc is so small (on the discrete raster) that it is
                // skipped.
                arc = (y0 < 0 ? 5 : 6);
            }
            break;
        case 5:  // <(u2,v2,-1)> to <(-a,0),infinity>
            y0++;
            dy++;
            sigma = B2*x0*x0 + A2*y0*y0 - A2*B2;
            if (sigma < 0)
            {
                x0--;
                dx--;
            }
            if (y0 == 0)
            {
                arc = 6;
            }
            break;
        case 6:  // <(-a,0),infinity> to <(u3,v3),+1>
            y0++;
            dy++;
            sigma = B2*(x0+1)*(x0+1) + A2*y0*y0 - A2*B2;
            if (sigma >= 0)
            {
                x0++;
                dx++;
            }
            if (A2*y0 >= -B2*x0)
            {
                // Slope dy/dx is no longer between 0 and +1.  Switch to next
                // arc drawer.  For large a and b, you expect to go to
                // 'arc = 7'.  But for small a or b, it is possible that the
                // next arc is so small (on the discrete raster) that it is
                // skipped.
                arc = (x0 < 0 ? 7 : 8);
            }
            break;
        case 7:  // <(u3,v3),+1> to <(0,b),0>
            x0++;
            dx++;
            sigma = B2*x0*x0 + A2*y0*y0 - A2*B2;
            if (sigma < 0)
            {
                y0++;
                dy++;
            }
            if (x0 == 0)
            {
                arc = 0;
            }
            break;
        }

        if (sqrLen <= 1)
        {
            notDone = false;
            continue;
        }

        sqrLen = dx*dx + dy*dy;
    }
}
//----------------------------------------------------------------------------
void Wm5::GeneralEllipse2D (int xc, int yc, int xa, int ya, int xb, int yb,
    void (*callback)(int,int))
{
    // Change sign and/or swap the direction vectors in order to satisfy this
    // algorithm's requirement for visiting pixels in a particular order.
    int save;
    if (xa <= 0 && ya > 0)
    {
        // Use (-xb,-yb,xa,ya).
        save = xa;
        xa = -xb;
        xb = save;
        save = ya;
        ya = -yb;
        yb = save;
    }
    else if (xa < 0 && ya <= 0)
    {
        // Use (-xa,-ya,-xb,-yb).
        xa = -xa;
        ya = -ya;
        xb = -xb;
        yb = -yb;
    }
    else if (xa >= 0 && ya < 0)
    {
        // Use (xb,yb,-xa,-ya).
        save = xa;
        xa = xb;
        xb = -save;
        save = ya;
        ya = yb;
        yb = -save;
    }
    // else (xa > 0 && ya >= 0): use (xa,ya,xb,yb)

    // Ellipse is a*(x-xc)^2+2*b*(x-xc)*(y-yc)+c*(y-yc)^2 = d where
    //
    //   a = xa^2*Lb^4 + xb^2*La^4
    //   b = xa*ya*Lb^4 + xb*yb*La^4
    //   c = ya^2*Lb^4 + yb^2*La^4
    //   d = La^4*Lb^4
    //   La^2 = xa^2+ya^2
    //   Lb^2 = xb^2+yb^2
    //
    // Pixel determination is performed relative to origin (0,0).  The
    // ellipse at origin is a*x^2+b*x*y+c*y^2=d.  Slope of curve is
    // dy/dx = -(a*x+b*y)/(b*x+c*y).  Slope at (xb,yb) is
    // dy/dx = -xb/yb >= 0 and slope at (xa,ya) is dy/dx = -xa/ya < 0.

    Integer<4> iXA2 = xa*xa;
    Integer<4> iYA2 = ya*ya;
    Integer<4> iXB2 = xb*xb;
    Integer<4> iYB2 = yb*yb;
    Integer<4> iXAYA = xa*ya;
    Integer<4> iXBYB = xb*yb;
    Integer<4> iLa2 = iXA2+iYA2;
    Integer<4> iLa4 = iLa2*iLa2;
    Integer<4> iLb2 = iXB2+iYB2;
    Integer<4> iLb4 = iLb2*iLb2;
    Integer<4> iA = iXA2*iLb4 + iXB2*iLa4;
    Integer<4> iB = iXAYA*iLb4 + iXBYB*iLa4;
    Integer<4> iC = iYA2*iLb4 + iYB2*iLa4;
    Integer<4> iD = iLa4*iLb4;

    Integer<4> dx, dy, sigma;
    int x, y, xp1, ym1, yp1;

    if (ya <= xa)
    {
        // Start at (-xA,-yA).
        x = -xa;
        y = -ya;
        dx = -(iB*xa + iC*ya);
        dy = iA*xa + iB*ya;

        // Arc from (-xA,-yA) to point (x0,y0) where dx/dy = 0.
        while (dx <= 0)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            y++;
            sigma = iA*x*x + 2*iB*x*y + iC*y*y - iD;
            if (sigma < 0)
            {
                dx -= iB;
                dy += iA;
                x--;
            }
            dx += iC;
            dy -= iB;
        }

        // Arc from (x0,y0) to point (x1,y1) where dy/dx = 1.
        while (dx <= dy)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            y++;
            xp1 = x+1;
            sigma = iA*xp1*xp1 + 2*iB*xp1*y + iC*y*y - iD;
            if (sigma >= 0)
            {
                dx += iB;
                dy -= iA;
                x = xp1;
            }
            dx += iC;
            dy -= iB;
        }

        // Arc from (x1,y1) to point (x2,y2) where dy/dx = 0.
        while (dy >= 0)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            x++;
            sigma = iA*x*x + 2*iB*x*y + iC*y*y - iD;
            if (sigma < 0)
            {
                dx += iC;
                dy -= iB;
                y++;
            }
            dx += iB;
            dy -= iA;
        }

        // Arc from (x2,y2) to point (x3,y3) where dy/dx = -1.
        while (dy >= -dx)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            x++;
            ym1 = y-1;
            sigma = iA*x*x + 2*iB*x*ym1 + iC*ym1*ym1 - iD;
            if (sigma >= 0)
            {
                dx -= iC;
                dy += iB;
                y = ym1;
            }
            dx += iB;
            dy -= iA;
        }

        // Arc from (x3,y3) to (xa,ya).
        while (y >= ya)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            y--;
            sigma = iA*x*x + 2*iB*x*y + iC*y*y - iD;
            if (sigma < 0)
            {
                dx += iB;
                dy -= iA;
                x++;
            }
            dx -= iC;
            dy += iB;
        }
    }
    else
    {
        // Start at (-xa,-ya).
        x = -xa;
        y = -ya;
        dx = -(iB*xa + iC*ya);
        dy = iA*xa + iB*ya;

        // Arc from (-xa,-ya) to point (x0,y0) where dy/dx = -1.
        while (-dx >= dy)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            x--;
            yp1 = y+1;
            sigma = iA*x*x + 2*iB*x*yp1 + iC*yp1*yp1 - iD;
            if (sigma >= 0)
            {
                dx += iC;
                dy -= iB;
                y = yp1;
            }
            dx -= iB;
            dy += iA;
        }

        // Arc from (x0,y0) to point (x1,y1) where dx/dy = 0.
        while (dx <= 0)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            y++;
            sigma = iA*x*x + 2*iB*x*y + iC*y*y - iD;
            if (sigma < 0)
            {
                dx -= iB;
                dy += iA;
                x--;
            }
            dx += iC;
            dy -= iB;
        }

        // Arc from (x1,y1) to point (x2,y2) where dy/dx = 1.
        while (dx <= dy)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            y++;
            xp1 = x+1;
            sigma = iA*xp1*xp1 + 2*iB*xp1*y+iC*y*y - iD;
            if (sigma >= 0)
            {
                dx += iB;
                dy -= iA;
                x = xp1;
            }
            dx += iC;
            dy -= iB;
        }

        // Arc from (x2,y2) to point (x3,y3) where dy/dx = 0.
        while (dy >= 0)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            x++;
            sigma = iA*x*x + 2*iB*x*y + iC*y*y - iD;
            if (sigma < 0)
            {
                dx += iC;
                dy -= iB;
                y++;
            }
            dx += iB;
            dy -= iA;
        }

        // Arc from (x3,y3) to (xa,ya).
        while (x <= xa)
        {
            callback(xc+x, yc+y);
            callback(xc-x, yc-y);
            x++;
            ym1 = y-1;
            sigma = iA*x*x + 2*iB*x*ym1 + iC*ym1*ym1 - iD;
            if (sigma >= 0)
            {
                dx -= iC;
                dy += iB;
                y = ym1;
            }
            dx += iB;
            dy -= iA;
        }
    }
}
//----------------------------------------------------------------------------
void Wm5::RecursiveFill (int x, int y, int xMax, int yMax, int** image,
    int foreColor, int backColor)
{
    image[y][x] = foreColor;

    int xp1 = x+1;
    if (xp1 < xMax && image[y][xp1] == backColor)
    {
        RecursiveFill(xp1, y, xMax, yMax, image, foreColor, backColor);
    }

    int xm1 = x-1;
    if (0 <= xm1 && image[y][xm1] == backColor)
    {
        RecursiveFill(xm1, y, xMax, yMax, image, foreColor, backColor);
    }

    int yp1 = y+1;
    if (yp1 < yMax && image[yp1][x] == backColor)
    {
        RecursiveFill(x, yp1, xMax, yMax, image, foreColor, backColor);
    }

    int ym1 = y-1;
    if (0 <= ym1 && image[ym1][x] == backColor)
    {
        RecursiveFill(x, ym1, xMax, yMax, image, foreColor, backColor);
    }
}
//----------------------------------------------------------------------------
void Wm5::NonrecursiveFill (int x, int y, int xMax, int yMax,
    int** image, int foreColor, int backColor)
{
    // Allocate the maximum amount of space needed.  If you prefer less, you
    // need to modify this data structure to allow for dynamic reallocation
    // when it is needed.  An empty stack has top == -1.
    int quantity = xMax*yMax;
    int* xStack = new1<int>(quantity);
    int* yStack = new1<int>(quantity);

    // Push seed point onto stack if it has the background color.  All points
    // pushed onto stack have background color backColor.
    int top = 0;
    xStack[top] = x;
    yStack[top] = y;

    while (top >= 0)  // stack is not empty
    {
        // Read top of stack.  Do not pop since we need to return to this
        // top value later to restart the fill in a different direction.
        x = xStack[top];
        y = yStack[top];

        // Fill the pixel.
        image[y][x] = foreColor;

        int xp1 = x+1;
        if (xp1 < xMax && image[y][xp1] == backColor)
        {
            // Push pixel with background color.
            ++top;
            xStack[top] = xp1;
            yStack[top] = y;
            continue;
        }

        int xm1 = x-1;
        if (0 <= xm1 && image[y][xm1] == backColor)
        {
            // Push pixel with background color.
            ++top;
            xStack[top] = xm1;
            yStack[top] = y;
            continue;
        }

        int yp1 = y+1;
        if (yp1 < yMax && image[yp1][x] == backColor)
        {
            // Push pixel with background color.
            ++top;
            xStack[top] = x;
            yStack[top] = yp1;
            continue;
        }

        int ym1 = y-1;
        if (0 <= ym1 && image[ym1][x] == backColor)
        {
            // Push pixel with background color.
            ++top;
            xStack[top] = x;
            yStack[top] = ym1;
            continue;
        }

        // Done in all directions, pop and return to search other directions.
        --top;
    }

    delete1(xStack);
    delete1(yStack);
}
//----------------------------------------------------------------------------
