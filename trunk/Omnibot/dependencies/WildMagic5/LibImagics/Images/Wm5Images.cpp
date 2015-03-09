// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5Images.h"
using namespace Wm5;

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
namespace Wm5
{
template class TImage<Echar>;
template class TImage<Euchar>;
template class TImage<Eshort>;
template class TImage<Eushort>;
template class TImage<Eint>;
template class TImage<Euint>;
template class TImage<Elong>;
template class TImage<Eulong>;
template class TImage<Efloat>;
template class TImage<Edouble>;
template class TImage<Ergb5>;
template class TImage<Ergb8>;

template class TImage2D<Echar>;
template class TImage2D<Euchar>;
template class TImage2D<Eshort>;
template class TImage2D<Eushort>;
template class TImage2D<Eint>;
template class TImage2D<Euint>;
template class TImage2D<Elong>;
template class TImage2D<Eulong>;
template class TImage2D<Efloat>;
template class TImage2D<Edouble>;
template class TImage2D<Ergb5>;
template class TImage2D<Ergb8>;

template class TImage3D<Echar>;
template class TImage3D<Euchar>;
template class TImage3D<Eshort>;
template class TImage3D<Eushort>;
template class TImage3D<Eint>;
template class TImage3D<Euint>;
template class TImage3D<Elong>;
template class TImage3D<Eulong>;
template class TImage3D<Efloat>;
template class TImage3D<Edouble>;
template class TImage3D<Ergb5>;
template class TImage3D<Ergb8>;
}
//----------------------------------------------------------------------------
