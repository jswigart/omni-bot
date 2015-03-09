// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <class Mesh, class Bound>
CollisionGroup<Mesh,Bound>::CollisionGroup ()
{
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
CollisionGroup<Mesh,Bound>::~CollisionGroup ()
{
    typename std::vector<CollisionRecordPtr>::iterator rec = mRecords.begin();
    typename std::vector<CollisionRecordPtr>::iterator end = mRecords.end();
    for (/**/; rec != end; ++rec)
    {
        delete0(*rec);
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
bool CollisionGroup<Mesh,Bound>::Add (CollisionRecord<Mesh,Bound>* record)
{
    typename std::vector<CollisionRecordPtr>::iterator rec = mRecords.begin();
    typename std::vector<CollisionRecordPtr>::iterator end = mRecords.end();
    for (/**/; rec != end; ++rec)
    {
        if (record == *rec)
        {
            return false;
        }
    }

    mRecords.push_back(record);
    return true;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
bool CollisionGroup<Mesh,Bound>::Remove (CollisionRecord<Mesh,Bound>* record)
{
    typename std::vector<CollisionRecordPtr>::iterator rec = mRecords.begin();
    typename std::vector<CollisionRecordPtr>::iterator end = mRecords.end();
    for (/**/; rec != end; ++rec)
    {
        if (record == *rec)
        {
            mRecords.erase(rec);
            delete0(record);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionGroup<Mesh,Bound>::TestIntersection ()
{
    // Objects are assumed to be stationary, compare all pairs.
    const int numRecords = (int)mRecords.size();
    for (int i0 = 0; i0 < numRecords; ++i0)
    {
        CollisionRecordPtr record0 = mRecords[i0];
        for (int i1 = i0 + 1; i1 < numRecords; ++i1)
        {
            CollisionRecordPtr record1 = mRecords[i1];
            record0->TestIntersection(*record1);
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionGroup<Mesh,Bound>::FindIntersection ()
{
    // Objects are assumed to be stationary, compare all pairs.
    const int numRecords = (int)mRecords.size();
    for (int i0 = 0; i0 < numRecords; ++i0)
    {
        CollisionRecordPtr record0 = mRecords[i0];
        for (int i1 = i0 + 1; i1 < numRecords; ++i1)
        {
            CollisionRecordPtr record1 = mRecords[i1];
            record0->FindIntersection(*record1);
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionGroup<Mesh,Bound>::TestIntersection (float tmax)
{
    // Objects are assumed to be moving, compare all pairs.
    const int numRecords = (int)mRecords.size();
    for (int i0 = 0; i0 < numRecords; i0++)
    {
        CollisionRecordPtr record0 = mRecords[i0];
        for (int i1 = i0 + 1; i1 < numRecords; ++i1)
        {
            CollisionRecordPtr record1 = mRecords[i1];
            if (record0->GetVelocity() || record1->GetVelocity())
            {
                record0->TestIntersection(tmax, *record1);
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Mesh, class Bound>
void CollisionGroup<Mesh,Bound>::FindIntersection (float tmax)
{
    // Objects are assumed to be moving, compare all pairs.
    const int numRecords = (int)mRecords.size();
    for (int i0 = 0; i0 < numRecords; ++i0)
    {
        CollisionRecordPtr record0 = mRecords[i0];
        for (int i1 = i0 + 1; i1 < numRecords; ++i1)
        {
            CollisionRecordPtr record1 = mRecords[i1];
            if (record0->GetVelocity() || record1->GetVelocity())
            {
                record0->FindIntersection(tmax, *record1);
            }
        }
    }
}
//----------------------------------------------------------------------------
