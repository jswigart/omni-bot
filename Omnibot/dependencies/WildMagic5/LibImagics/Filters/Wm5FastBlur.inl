// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Numeric>
void FastBlur1 (int xBound, Numeric* image, Numeric* temp, double scale,
    double logBase)
{
    int x;
    for (x = 0; x < xBound; ++x) 
    {
        double rxp = x + scale;
        double rxm = x - scale;
        int xp = (int)Mathd::Floor(rxp);
        int xm = (int)Mathd::Ceil(rxm);

        double xsum = -2.0*image[x];

        if (xp >= xBound-1)  // use boundary value
        {
            xsum += image[xBound-1];
        }
        else  // linearly interpolate
        {
            xsum += image[xp] + (rxp - xp)*(image[xp+1] - image[xp]);
        }

        if (xm <= 0)  // use boundary value
        {
            xsum += image[0];
        }
        else  // linearly interpolate
        {
            xsum += image[xm] + (rxm - xm)*(image[xm] - image[xm-1]);
        }

        temp[x] = (Numeric)(image[x] + logBase*xsum);
    }

    for (x = 0; x < xBound; ++x)
    {
        image[x] = temp[x];
    }
}
//----------------------------------------------------------------------------
template <typename Numeric>
void FastBlur2 (int xBound, int yBound, Numeric** image, Numeric** temp,
    double scale, double logBase)
{
    int x, y;
    for (y = 0; y < yBound; ++y) 
    {
        double ryp = y + scale;
        double rym = y - scale;
        int yp = (int)Mathd::Floor(ryp);
        int ym = (int)Mathd::Ceil(rym);

        for (x = 0; x < xBound; ++x) 
        {
            double rxp = x + scale;
            double rxm = x - scale;
            int xp = (int)Mathd::Floor(rxp);
            int xm = (int)Mathd::Ceil(rxm);

            // x portion of second central difference
            double xsum = -2.0*image[y][x];
            if (xp >= xBound-1)  // use boundary value
            {
                xsum += image[y][xBound-1];
            }
            else  // linearly interpolate
            {
                xsum += image[y][xp] + (rxp - xp)*(image[y][xp+1] -
                    image[y][xp]);
            }

            if (xm <= 0)  // use boundary value
            {
                xsum += image[y][0];
            }
            else  // linearly interpolate
            {
                xsum += image[y][xm] + (rxm - xm)*(image[y][xm] -
                    image[y][xm-1]);
            }

            // y portion of second central difference
            double ysum = -2.0*image[y][x];
            if (yp >= yBound-1)  // use boundary value
            {
                ysum += image[yBound-1][x];
            }
            else  // linearly interpolate
            {
                ysum += image[yp][x] + (ryp - yp)*(image[yp+1][x] -
                    image[yp][x]);
            }

            if (ym <= 0)  // use boundary value
            {
                ysum += image[0][x];
            }
            else  // linearly interpolate
            {
                ysum += image[ym][x] + (rym - ym)*(image[ym][x] -
                    image[ym-1][x]);
            }

            temp[y][x] = (Numeric)(image[y][x] + logBase*(xsum + ysum));
        }
    }

    for (y = 0; y < yBound; ++y)
    {
        for (x = 0; x < xBound; ++x)
        {
            image[y][x] = temp[y][x];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Numeric>
void FastBlur3 (int xBound, int yBound, int zBound, Numeric*** image,
    Numeric*** temp, double scale, double logBase)
{
    int x, y, z;
    for (z = 0; z < zBound; ++z)
    {
        double rzp = z + scale;
        double rzm = z - scale;
        int zp = (int)Mathd::Floor(rzp);
        int zm = (int)Mathd::Ceil(rzm);

        for (y = 0; y < yBound; ++y) 
        {
            double ryp = y + scale;
            double rym = y - scale;
            int yp = (int)Mathd::Floor(ryp);
            int ym = (int)Mathd::Ceil(rym);

            for (x = 0; x < xBound; ++x) 
            {
                double rxp = x + scale;
                double rxm = x - scale;
                int xp = (int)Mathd::Floor(rxp);
                int xm = (int)Mathd::Ceil(rxm);

                // x portion of second central difference
                double xsum = -2.0*image[z][y][x];
                if (xp >= xBound-1)  // use boundary value
                {
                    xsum += image[z][y][xBound-1];
                }
                else  // linearly interpolate
                {
                    xsum += image[z][y][xp] + (rxp - xp)*(
                        image[z][y][xp+1] - image[z][y][xp]);
                }

                if (xm <= 0)  // use boundary value
                {
                    xsum += image[z][y][0];
                }
                else  // linearly interpolate
                {
                    xsum += image[z][y][xm] + (rxm - xm)*(
                        image[z][y][xm] - image[z][y][xm-1]);
                }

                // y portion of second central difference
                double ysum = -2.0*image[z][y][x];
                if (yp >= yBound-1)  // use boundary value
                {
                    ysum += image[z][yBound-1][x];
                }
                else  // linearly interpolate
                {
                    ysum += image[z][yp][x] + (ryp-yp)*(
                        image[z][yp+1][x] - image[z][yp][x]);
                }

                if (ym <= 0)  // use boundary value
                {
                    ysum += image[z][0][x];
                }
                else  // linearly interpolate
                {
                    ysum += image[z][ym][x] + (rym - ym)*(
                        image[z][ym][x] - image[z][ym-1][x]);
                }

                // z portion of second central difference
                double zsum = -2.0*image[z][y][x];
                if (zp >= zBound-1)  // use boundary value
                {
                    zsum += image[zBound-1][y][x];
                }
                else  // linearly interpolate
                {
                    zsum += image[zp][y][x] + (rzp - zp)*(
                        image[zp+1][y][x] - image[zp][y][x]);
                }

                if (zm <= 0)  // use boundary value
                {
                    zsum += image[0][y][x];
                }
                else  // linearly interpolate
                {
                    zsum += image[zm][y][x] + (rzm - zm)*(
                        image[zm][y][x] - image[zm-1][y][x]);
                }

                temp[z][y][x] = (Numeric)(image[z][y][x] +
                    logBase*(xsum + ysum + zsum));
            }
        }
    }

    for (z = 0; z < zBound; ++z)
    {
        for (y = 0; y < yBound; ++y)
        {
            for (x = 0; x < xBound; ++x)
            {
                image[z][y][x] = temp[z][y][x];
            }
        }
    }
}
//----------------------------------------------------------------------------
