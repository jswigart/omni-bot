// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::PlanarGraph ()
{
}
//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::~PlanarGraph ()
{
    typename Vertices::iterator iter = mVertices.begin();
    typename Vertices::iterator end = mVertices.end();
    for (/**/; iter != end; ++iter)
    {
        Vertex* vertex = iter->second;
        delete0(vertex);
    }
}
//----------------------------------------------------------------------------
template <typename Point2>
const typename PlanarGraph<Point2>::Vertices&
PlanarGraph<Point2>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Point2>
const typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetVertex (int index) const
{
    typename Vertices::const_iterator iter = mVertices.find(index);
    return (iter != mVertices.end() ? iter->second : 0);
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::InsertVertex (const Point2& position, int index)
{
    typename Vertices::iterator iter = mVertices.find(index);
    if (iter != mVertices.end())
    {
        return false;
    }

    // Insert the vertex into the vertex set.  The adjacency array has already
    // been initialized to empty.
    Vertex* vertex = new0 Vertex(position, index);
    mVertices[index] = vertex;
    return true;
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::RemoveVertex (int index)
{
    typename Vertices::iterator iter = mVertices.find(index);
    if (iter != mVertices.end())
    {
        Vertex* vertex = iter->second;
        if (vertex->Adjacent.size() == 0)
        {
            mVertices.erase(iter);
            delete0(vertex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Point2>
const typename PlanarGraph<Point2>::Edges&
PlanarGraph<Point2>::GetEdges () const
{
    return mEdges;
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::InsertEdge (int index0, int index1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.  The typecast supports conceptual constness from the
    // users perspective.
    Vertex* vertex0 = (Vertex*)GetVertex(index0);
    if (!vertex0)
    {
        return false;
    }

    Vertex* vertex1 = (Vertex*)GetVertex(index1);
    if (!vertex1)
    {
        return false;
    }

    EdgeKey eKey(index0, index1);
    std::map<EdgeKey,bool>::iterator iter = mEdges.find(eKey);
    if (iter == mEdges.end())
    {
        // The edge does not exist, insert it into the set.  The edge is
        // tagged as "not a cycle".
        mEdges[eKey] = false;

        // Update the vertex-adjacency information.  The graph is undirected,
        // so each vertex must know about the other.
        vertex0->Insert(vertex1);
        vertex1->Insert(vertex0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::RemoveEdge (int index0, int index1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.  The typecast supports conceptual constness from the
    // users perspective.
    Vertex* vertex0 = (Vertex*)GetVertex(index0);
    if (!vertex0)
    {
        return false;
    }

    Vertex* vertex1 = (Vertex*)GetVertex(index1);
    if (!vertex1)
    {
        return false;
    }

    EdgeKey eKey(index0, index1);
    std::map<EdgeKey,bool>::iterator iter = mEdges.find(eKey);
    if (iter != mEdges.end())
    {
        // The edge exists, remove it from the set.
        mEdges.erase(iter);

        // Update the vertex-adjacency information.  The graph is undirected,
        // so each vertex knows about the other.
        vertex0->Remove(vertex1);
        vertex1->Remove(vertex0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::ExtractPrimitives (
    std::vector<Primitive*>& primitives)
{
    // Create a heap of vertices sorted lexicographically.
    std::set<VertexPtr> heap;
    typename Vertices::iterator iter = mVertices.begin();
    typename Vertices::iterator end = mVertices.end();
    for (/**/; iter != end; ++iter)
    {
        heap.insert(iter->second);
    }

    while (!heap.empty())
    {
        // Get the vertex of minimum x-value.
        VertexPtr VPtr = *heap.begin();
        Vertex* V0 = (Vertex*)VPtr;

        if (V0->Adjacent.size() == 0)
        {
            ExtractIsolatedVertex(V0, heap, primitives);
        }
        else if (V0->Adjacent.size() == 1)
        {
            ExtractFilament(V0, V0->Adjacent[0], heap, primitives);
        }
        else
        {
            // The primitive can be a filament or a minimal cycle.
            ExtractPrimitive(V0, heap, primitives);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::SetCycleEdge (int index0, int index1,
    bool cycleEdge)
{
    EdgeKey eKey(index0, index1);
    typename Edges::iterator iter = mEdges.find(eKey);
    if (iter != mEdges.end())
    {
        iter->second = cycleEdge;
    }
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::GetCycleEdge (int index0, int index1) const
{
    EdgeKey eKey(index0, index1);
    typename Edges::const_iterator iter = mEdges.find(eKey);
    if (iter != mEdges.end())
    {
        return iter->second;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::ExtractIsolatedVertex (Vertex* V0,
    std::set<VertexPtr>& heap, std::vector<Primitive*>& primitives)
{
    Primitive* primitive = new0 Primitive(PT_ISOLATED_VERTEX);

    primitive->Sequence.push_back(std::make_pair(V0->Position, V0->Index));
    heap.erase(V0);
    RemoveVertex(V0->Index);

    primitives.push_back(primitive);
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::ExtractFilament (Vertex* V0, Vertex* V1,
    std::set<VertexPtr>& heap, std::vector<Primitive*>& primitives)
{
    // (V0,V1) is the first edge of the purported filament.
    assertion(V0->Adjacent.size() != 2, "Unexpected condition\n");
    if (GetCycleEdge(V0->Index, V1->Index))
    {
        // The edge is from an earlier visited minimal cycle.  Delete the
        // purported filament because it is an imposter.
        if (V0->Adjacent.size() >= 3)
        {
            // V0 is a branch point.  Break the connection.
            RemoveEdge(V0->Index, V1->Index);
            V0 = V1;
            if (V0->Adjacent.size() == 1)
            {
                V1 = V0->Adjacent[0];
            }
        }

        while (V0->Adjacent.size() == 1)
        {
            V1 = V0->Adjacent[0];
            if (GetCycleEdge(V0->Index, V1->Index))
            {
                heap.erase(V0);
                RemoveEdge(V0->Index, V1->Index);
                RemoveVertex(V0->Index);
                V0 = V1;
            }
            else
            {
                break;
            }
        }

        if (V0->Adjacent.size() == 0)
        {
            heap.erase(V0);
            RemoveVertex(V0->Index);
        }
    }
    else
    {
        // A real filament has been found.
        Primitive* primitive = new0 Primitive(PT_FILAMENT);

        if (V0->Adjacent.size() >= 3)
        {
            // V0 is a branch point.  Store it and break the connection.
            primitive->Sequence.push_back(
                std::make_pair(V0->Position, V0->Index));
            RemoveEdge(V0->Index, V1->Index);
            V0 = V1;
            if (V0->Adjacent.size() == 1)
            {
                V1 = V0->Adjacent[0];
            }
        }

        while (V0->Adjacent.size() == 1)
        {
            V1 = V0->Adjacent[0];
            primitive->Sequence.push_back(
                std::make_pair(V0->Position, V0->Index));
            heap.erase(V0);
            RemoveEdge(V0->Index, V1->Index);
            RemoveVertex(V0->Index);
            V0 = V1;
        }

        primitive->Sequence.push_back(
            std::make_pair(V0->Position, V0->Index));

        if (V0->Adjacent.size() == 0)
        {
            heap.erase(V0);
            RemoveVertex(V0->Index);
        }

        primitives.push_back(primitive);
    }
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::ExtractPrimitive (Vertex* V0,
    std::set<VertexPtr>& heap, std::vector<Primitive*>& primitives)
{
    std::set<Vertex*> visited;
    std::vector<std::pair<Point2,int> > sequence;
    sequence.push_back(std::make_pair(V0->Position, V0->Index));
    Vertex* V1 = GetClockwiseMost(0,V0);
    Vertex* VPrev = V0;
    Vertex* VCurr = V1;

    while (VCurr && VCurr != V0
        && visited.find(VCurr) == visited.end())
    {
        sequence.push_back(std::make_pair(VCurr->Position, VCurr->Index));
        visited.insert(VCurr);
        Vertex* VNext = GetCounterclockwiseMost(VPrev, VCurr);
        VPrev = VCurr;
        VCurr = VNext;
    }

    if (!VCurr)
    {
        // A filament has been found.  It is not necessarily rooted at V0.
        assertion(VPrev->Adjacent.size() == 1, "Unexpected condition\n");
        ExtractFilament(VPrev, VPrev->Adjacent[0], heap, primitives);
    }
    else if (VCurr == V0)
    {
        // A minimal cycle has been found.
        Primitive* primitive = new0 Primitive(PT_MINIMAL_CYCLE);
        primitive->Sequence = sequence;
        primitives.push_back(primitive);

        // Mark the edges to indicate they are part of a cycle.
        int sQuantity = (int)sequence.size();
        for (int i0 = sQuantity-1, i1 = 0; i1 < sQuantity; i0 = i1++)
        {
            int iV0 = sequence[i0].second;
            int iV1 = sequence[i1].second;
            SetCycleEdge(iV0, iV1, true);
        }

        // Remove any vertices and edges not needed by other primitives.
        RemoveEdge(V0->Index, V1->Index);

        // Since the edges are marked, the calls to GetFilament in this block
        // will only delete more edges but not create a primitive.
        if (V0->Adjacent.size() == 1)
        {
            ExtractFilament(V0, V0->Adjacent[0], heap, primitives);
        }

        if (V1->Adjacent.size() == 1)
        {
            ExtractFilament(V1, V1->Adjacent[0], heap, primitives);
        }
    }
    else  // VCurr has been visited before
    {
        // A cycle has been found, but it is not guaranteed to be a minimal
        // cycle.  V0 is therefore part of a filament.
        
        // Find a filament starting vertex.
        while (V0->Adjacent.size() == 2)
        {
            if (V0->Adjacent[0] != V1)
            {
                V1 = V0;
                V0 = V0->Adjacent[0];
            }
            else
            {
                V1 = V0;
                V0 = V0->Adjacent[1];
            }
        }

        // Create the primitive.
        ExtractFilament(V0, V1, heap, primitives);
    }
}
//----------------------------------------------------------------------------
template <typename Point2>
typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetClockwiseMost (Vertex* VPrev, Vertex* VCurr)
{
    Vertex* VNext = 0;
    Point2 DCurr =
        (VPrev ? VCurr->Position - VPrev->Position : Point2(0,-1));
    Point2 DNext;
    bool VCurrConvex = false;

    for (int i = 0; i < (int)VCurr->Adjacent.size(); ++i)
    {
        // Get an adjacent vertex.
        Vertex* VAdj = VCurr->Adjacent[i];

        // No backtracking allowed.
        if (VAdj == VPrev)
        {
            continue;
        }

        // The potential direction to move in.
        Point2 DAdj = VAdj->Position - VCurr->Position;

        // Select the first candidate.
        if (!VNext)
        {
            VNext = VAdj;
            DNext = DAdj;
            VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            continue;
        }

        // Update if the next candidate is clockwise of the current
        // clockwise-most vertex.
        if (VCurrConvex)
        {
            if (DCurr[0]*DAdj[1]-DCurr[1]*DAdj[0] < 0
            ||  DNext[0]*DAdj[1]-DNext[1]*DAdj[0] < 0)
            {
                VNext = VAdj;
                DNext = DAdj;
                VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            }
        }
        else
        {
            if (DCurr[0]*DAdj[1]-DCurr[1]*DAdj[0] < 0
            &&  DNext[0]*DAdj[1]-DNext[1]*DAdj[0] < 0)
            {
                VNext = VAdj;
                DNext = DAdj;
                VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            }
        }
    }

    return VNext;
}
//----------------------------------------------------------------------------
template <typename Point2>
typename PlanarGraph<Point2>::Vertex*
PlanarGraph<Point2>::GetCounterclockwiseMost (Vertex* VPrev, Vertex* VCurr)
{
    Vertex* VNext = 0;
    Point2 DCurr =
        (VPrev ? VCurr->Position - VPrev->Position : Point2(0,-1));
    Point2 DNext;
    bool VCurrConvex = false;

    for (int i = 0; i < (int)VCurr->Adjacent.size(); ++i)
    {
        // Get an adjacent vertex.
        Vertex* VAdj = VCurr->Adjacent[i];

        // No backtracking allowed.
        if (VAdj == VPrev)
        {
            continue;
        }

        // The potential direction to move in.
        Point2 DAdj = VAdj->Position - VCurr->Position;

        // Select the first candidate.
        if (!VNext)
        {
            VNext = VAdj;
            DNext = DAdj;
            VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            continue;
        }

        // Update if the next candidate is clockwise of the current
        // clockwise-most vertex.
        if (VCurrConvex)
        {
            if (DCurr[0]*DAdj[1]-DCurr[1]*DAdj[0] > 0
            &&  DNext[0]*DAdj[1]-DNext[1]*DAdj[0] > 0)
            {
                VNext = VAdj;
                DNext = DAdj;
                VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            }
        }
        else
        {
            if (DCurr[0]*DAdj[1]-DCurr[1]*DAdj[0] > 0
            ||  DNext[0]*DAdj[1]-DNext[1]*DAdj[0] > 0)
            {
                VNext = VAdj;
                DNext = DAdj;
                VCurrConvex = (DNext[0]*DCurr[1] - DNext[1]*DCurr[0] <= 0);
            }
        }
    }

    return VNext;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// PlanarGraph::Vertex
//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::Vertex::Vertex (const Point2& position, int index)
    :
    Position(position),
    Index(index)
{
}
//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::Vertex::~Vertex ()
{
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::Vertex::Insert (Vertex* adjacent)
{
    Adjacent.push_back(adjacent);
}
//----------------------------------------------------------------------------
template <typename Point2>
void PlanarGraph<Point2>::Vertex::Remove (Vertex* adjacent)
{
    // Maintain a compact array.
    int numAdjacents = (int)Adjacent.size();
    for (int i = 0; i < numAdjacents; ++i)
    {
        if (adjacent == Adjacent[i])
        {
            // Maintain a compact array.
            --numAdjacents;
            if (i < numAdjacents)
            {
                Adjacent[i] = Adjacent[numAdjacents];
            }
            Adjacent.pop_back();
            return;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// PlanarGraph::Primitive
//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::Primitive::Primitive (PrimitiveType type)
    :
    Type(type)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// PlanarGraph::VertexPtr
//----------------------------------------------------------------------------
template <typename Point2>
PlanarGraph<Point2>::VertexPtr::VertexPtr (Vertex* vertex)
    :
    mVertex(vertex)
{
}
//----------------------------------------------------------------------------
template <typename Point2>
bool PlanarGraph<Point2>::VertexPtr::operator< (const VertexPtr& vertexPtr)
    const
{
    if (mVertex->Position[0] < vertexPtr.mVertex->Position[0])
    {
        return true;
    }

    if (mVertex->Position[0] > vertexPtr.mVertex->Position[0])
    {
        return false;
    }

    return mVertex->Position[1] < vertexPtr.mVertex->Position[1];
}
//----------------------------------------------------------------------------
