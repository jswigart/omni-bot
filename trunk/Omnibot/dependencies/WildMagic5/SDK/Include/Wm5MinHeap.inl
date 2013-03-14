// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
MinHeapRecord<Generator,Scalar>::MinHeapRecord ()
{
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
MinHeapRecord<Generator,Scalar>::~MinHeapRecord ()
{
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline Generator MinHeapRecord<Generator,Scalar>::GetGenerator () const
{
    return mGenerator;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline Scalar MinHeapRecord<Generator,Scalar>::GetValue () const
{
    return mValue;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline int MinHeapRecord<Generator,Scalar>::GetIndex () const
{
    return mIndex;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
MinHeap<Generator,Scalar>::MinHeap (int maxElements, int growBy,
    Scalar initialValue)
    :
    mNumElements(0),
    mMaxElements(maxElements > 0 ? maxElements : 1),
    mGrowBy(growBy > 0 ? growBy : 1),
    mInitialValue(initialValue)
{
    mRecords = new1<MinHeapRecord<Generator,Scalar> >(mMaxElements);
    mRecordPointers = new1<MinHeapRecord<Generator,Scalar>*>(mMaxElements);
    for (int i = 0; i < mMaxElements; ++i)
    {
        mRecordPointers[i] = &mRecords[i];
        mRecordPointers[i]->mValue = mInitialValue;
        mRecordPointers[i]->mIndex = i;
    }
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
MinHeap<Generator,Scalar>::~MinHeap ()
{
    delete1(mRecords);
    delete1(mRecordPointers);
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline int MinHeap<Generator,Scalar>::GetMaxElements () const
{
    return mMaxElements;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline int MinHeap<Generator,Scalar>::GetGrowBy () const
{
    return mGrowBy;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline int MinHeap<Generator,Scalar>::GetNumElements () const
{
    return mNumElements;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline void MinHeap<Generator,Scalar>::GetMinimum (Generator& generator,
    Scalar& value) const
{
    MinHeapRecord<Generator,Scalar>* root = mRecordPointers[0];
    generator = root->mGenerator;
    value = root->mValue;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
inline const MinHeapRecord<Generator,Scalar>*
MinHeap<Generator,Scalar>::GetRecord (int i) const
{
    assertion(0 <= i && i < mNumElements, "Invalid index\n");
    return mRecordPointers[i];
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
const MinHeapRecord<Generator,Scalar>* MinHeap<Generator,Scalar>::Insert (
    Generator generator, Scalar value)
{
    // Grow the heap record array, if necessary.
    if (mNumElements == mMaxElements)
    {
        int newMaxElements = mMaxElements + mGrowBy;

        MinHeapRecord<Generator,Scalar>* newRecords =
            new1<MinHeapRecord<Generator,Scalar> >(newMaxElements);

        MinHeapRecord<Generator,Scalar>** newRecordPointers =
            new1<MinHeapRecord<Generator,Scalar>*>(newMaxElements);

        // Copy the old records to the new storage.
        size_t size = mMaxElements*sizeof(MinHeapRecord<Generator,Scalar>);
        memcpy(newRecords, mRecords, size);

        // Update the pointers to the old records.
        int i;
        for (i = 0; i < mMaxElements; ++i)
        {
            int byteOffset = (int)(mRecordPointers[i] - mRecords);
            newRecordPointers[i] = (MinHeapRecord<Generator,Scalar>*)(
                ((char*)newRecords) + byteOffset);
            newRecordPointers[i]->mIndex = i;
        }

        // Create the pointers for the new records.
        for (i = mMaxElements; i < newMaxElements; ++i)
        {
            newRecordPointers[i] = &newRecords[i];
            newRecordPointers[i]->mValue = mInitialValue;
            newRecordPointers[i]->mIndex = i;
        }

        delete1(mRecords);
        delete1(mRecordPointers);
        mMaxElements = newMaxElements;
        mRecords = newRecords;
        mRecordPointers = newRecordPointers;
    }

    // Store the input information in the last heap record, which is the last
    // leaf in the tree.
    int child = mNumElements++;
    MinHeapRecord<Generator,Scalar>* record = mRecordPointers[child];
    record->mGenerator = generator;
    record->mValue = value;

    // Propagate the information toward the root of the tree until it reaches
    // its correct position, thus restoring the tree to a valid heap.
    while (child > 0)
    {
        int parent = (child - 1)/2;
        if (mRecordPointers[parent]->mValue <= value)
        {
            // The parent has a value smaller than or equal to the child's
            // value, so we now have a valid heap.
            break;
        }

        // The parent has a larger value than the child's value.  Swap the
        // parent and child:

        // Move the parent into the child's slot.
        mRecordPointers[child] = mRecordPointers[parent];
        mRecordPointers[child]->mIndex = child;

        // Move the child into the parent's slot.
        mRecordPointers[parent] = record;
        mRecordPointers[parent]->mIndex = parent;

        child = parent;
    }

    return mRecordPointers[child];
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
void MinHeap<Generator,Scalar>::Remove (Generator& generator, Scalar& value)
{
    // Get the information from the root of the heap.
    MinHeapRecord<Generator,Scalar>* root = mRecordPointers[0];
    generator = root->mGenerator;
    value = root->mValue;

    // Restore the tree to a heap.  Abstractly, record is the new root of
    // the heap.  It is moved down the tree via parent-child swaps until it
    // is in a location that restores the tree to a heap.
    int last = --mNumElements;
    MinHeapRecord<Generator,Scalar>* record = mRecordPointers[last];
    int parent = 0, child = 1;
    while (child <= last)
    {
        if (child < last)
        {
            // Select the child with smallest value to be the one that is
            // swapped with the parent, if necessary.
            int childP1 = child + 1;
            if (mRecordPointers[child]->mValue >
                mRecordPointers[childP1]->mValue)
            {
                child = childP1;
            }
        }

        if (mRecordPointers[child]->mValue >= record->mValue)
        {
            // The tree is now a heap.
            break;
        }

        // Move the child into the parent's slot.
        mRecordPointers[parent] = mRecordPointers[child];
        mRecordPointers[parent]->mIndex = parent;

        parent = child;
        child = 2*child + 1;
    }

    // The previous 'last' record was moved to the root and propagated down
    // the tree to its final resting place, restoring the tree to a heap.
    // The slot mRecordPointers[parent] is that resting place.
    mRecordPointers[parent] = record;
    mRecordPointers[parent]->mIndex = parent;

    // The old root record must not be lost.  Attach it to the slot that
    // contained the old last record.
    mRecordPointers[last] = root;
    mRecordPointers[last]->mIndex = last;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
void MinHeap<Generator,Scalar>::Update (
    const MinHeapRecord<Generator,Scalar>* constRecord, Scalar value)
{
    // The input is 'const' to let the caller know that only MinHeap may
    // update the record.  This is essentially a form of mutability.
    MinHeapRecord<Generator,Scalar>* record =
        (MinHeapRecord<Generator,Scalar>*)constRecord;

    int parent, child, childP1, maxChild;

    if (value > record->mValue)
    {
        record->mValue = value;

        // The new value is larger than the old value.  Propagate it towards
        // the leaves.
        parent = record->mIndex;
        child = 2*parent + 1;
        while (child < mNumElements)
        {
            // At least one child exists.  Locate the one of maximum value.
            if (child < mNumElements-1)
            {
                // Two children exist.
                childP1 = child + 1;
                if (mRecordPointers[child]->mValue <=
                    mRecordPointers[childP1]->mValue)
                {
                    maxChild = child;
                }
                else
                {
                    maxChild = childP1;
                }
            }
            else
            {
                // One child exists.
                maxChild = child;
            }

            if (mRecordPointers[maxChild]->mValue >= value)
            {
                // The new value is in the correct place to restore the tree
                // to a heap.
                break;
            }

            // The child has a larger value than the parent's value.  Swap
            // the parent and child:

            // Move the child into the parent's slot.
            mRecordPointers[parent] = mRecordPointers[maxChild];
            mRecordPointers[parent]->mIndex = parent;

            // Move the parent into the child's slot.
            mRecordPointers[maxChild] = record;
            mRecordPointers[maxChild]->mIndex = maxChild;

            parent = maxChild;
            child = 2*parent + 1;
        }
    }
    else if (value < record->mValue)
    {
        record->mValue = value;

        // The new weight is smaller than the old weight.  Propagate it
        // towards the root.
        child = record->mIndex;
        while (child > 0)
        {
            // A parent exists.
            parent = (child - 1)/2;

            if (mRecordPointers[parent]->mValue <= value)
            {
                // The new value is in the correct place to restore the tree
                // to a heap.
                break;
            }

            // The parent has a smaller value than the child's value.  Swap
            // the child and parent:

            // Move the parent into the child's slot.
            mRecordPointers[child] = mRecordPointers[parent];
            mRecordPointers[child]->mIndex = child;

            // Move the child into the parent's slot.
            mRecordPointers[parent] = record;
            mRecordPointers[parent]->mIndex = parent;

            child = parent;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
bool MinHeap<Generator,Scalar>::IsValid (int start, int final)
{
    for (int child = start; child <= final; child++)
    {
        int parent = (child - 1)/2;
        if (parent > start)
        {
            if (mRecordPointers[parent]->mValue >
                mRecordPointers[child]->mValue)
            {
                return false;
            }

            if (mRecordPointers[parent]->mIndex != parent)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
bool MinHeap<Generator,Scalar>::IsValid ()
{
    return IsValid(0, mNumElements-1);
}
//----------------------------------------------------------------------------
template <typename Generator, typename Scalar>
void MinHeap<Generator,Scalar>::Print (const char* filename)
{
    std::ofstream outFile(filename);
    for (int i = 0; i < mNumElements; ++i)
    {
        MinHeapRecord<Generator,Scalar>* record = mRecordPointers[i];
        outFile << record->mIndex << ": gen = " << record->mGenerator
            << " , val = " << record->mValue << std::endl;
    }
    outFile.close();
}
//----------------------------------------------------------------------------
