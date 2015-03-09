// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MESHCURVATURE_H
#define WM5MESHCURVATURE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"
#include "Wm5Matrix2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MeshCurvature
{
public:
    // The caller is responsible for deleting the input arrays.
    MeshCurvature (int numVertices, const Vector3<Real>* vertices,
        int numTriangles, const int* indices);

    virtual ~MeshCurvature ();

    // Input values from the constructor.
    int GetNumVertices () const;
    const Vector3<Real>* GetVertices () const;
    int GetNumTriangles () const;
    const int* GetIndices () const;

    // Derived quantites from the input mesh.
    const Vector3<Real>* GetNormals () const;
    const Real* GetMinCurvatures () const;
    const Real* GetMaxCurvatures () const;
    const Vector3<Real>* GetMinDirections () const;
    const Vector3<Real>* GetMaxDirections () const;

protected:
    int mNumVertices;
    const Vector3<Real>* mVertices;
    int mNumTriangles;
    const int* mIndices;

    Vector3<Real>* mNormals;
    Real* mMinCurvatures;
    Real* mMaxCurvatures;
    Vector3<Real>* mMinDirections;
    Vector3<Real>* mMaxDirections;
};

typedef MeshCurvature<float> MeshCurvaturef;
typedef MeshCurvature<double> MeshCurvatured;

}

#endif
