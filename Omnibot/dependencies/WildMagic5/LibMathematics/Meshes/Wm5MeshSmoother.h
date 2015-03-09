// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MESHSMOOTHER_H
#define WM5MESHSMOOTHER_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM MeshSmoother
{
public:
    // The caller is responsible for deleting the input arrays.
    MeshSmoother (int numVertices, Vector3<Real>* vertices, int numTriangles,
        const int* indices);

    virtual ~MeshSmoother ();

    // For deferred construction and destruction.  The caller is responsible
    // for deleting the input arrays.
    MeshSmoother ();
    void Create (int numVertices, Vector3<Real>* vertices, int numTriangles,
        const int* indices);
    void Destroy ();

    // Input values from the constructor.
    int GetNumVertices () const;
    const Vector3<Real>* GetVertices () const;
    int GetNumTriangles () const;
    const int* GetIndices () const;

    // Derived quantites from the input mesh.
    const Vector3<Real>* GetNormals () const;
    const Vector3<Real>* GetMeans () const;

    // Apply one iteration of the smoother.  The input time is supported for
    // applications where the surface evolution is time-dependent.
    void Update (Real t = (Real)0);

protected:
    virtual bool VertexInfluenced (int i, Real t);
    virtual Real GetTangentWeight (int i, Real t);
    virtual Real GetNormalWeight (int i, Real t);

    int mNumVertices;
    Vector3<Real>* mVertices;
    int mNumTriangles;
    const int* mIndices;

    Vector3<Real>* mNormals;
    Vector3<Real>* mMeans;
    int* mNeighborCounts;
};

typedef MeshSmoother<float> MeshSmootherf;
typedef MeshSmoother<double> MeshSmootherd;

}

#endif
