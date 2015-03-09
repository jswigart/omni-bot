// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VERTEXFORMAT_H
#define WM5VERTEXFORMAT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VertexFormat : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VertexFormat);

public:
    enum
    {
        // The maximum number of attributes for a vertex format.
        AM_MAX_ATTRIBUTES = 16,

        // The maximum number of texture coordinate units.
        AM_MAX_TCOORD_UNITS = 8,

        // The maximum number of color units.
        AM_MAX_COLOR_UNITS = 2
    };

    // The channel types for attributes of a vertex format.
    enum WM5_GRAPHICS_ITEM AttributeType
    {
        AT_NONE,
        AT_FLOAT1,
        AT_FLOAT2,
        AT_FLOAT3,
        AT_FLOAT4,
        AT_HALF1,
        AT_HALF2,
        AT_HALF3,
        AT_HALF4,
        AT_UBYTE4,
        AT_SHORT1,
        AT_SHORT2,
        AT_SHORT4,
        AT_QUANTITY
    };

    // The usage (semantics) for attributes of a vertex format.
    enum WM5_GRAPHICS_ITEM AttributeUsage
    {
        AU_NONE,
        AU_POSITION,      // attr 0
        AU_NORMAL,        // attr 2
        AU_TANGENT,       // attr 14
        AU_BINORMAL,      // attr 15
        AU_TEXCOORD,      // attr 8-15
        AU_COLOR,         // attr 3-4
        AU_BLENDINDICES,  // attr 7
        AU_BLENDWEIGHT,   // attr 1
        AU_FOGCOORD,      // attr 5
        AU_PSIZE,         // attr 6
        AU_QUANTITY
    };

    // Construction and destruction.
    VertexFormat (int numAttributes);
    virtual ~VertexFormat ();

    // Convenience function for creating a vertex format by specifying all
    // its attributes at once.  For now this works only with a common
    // stream index of zero (stream indices are not yet implemented anyway).
    // The arguments must be triples of the form
    //   (AttributeUsage, AttributeType, usage index)
    // in that order.
    static VertexFormat* Create (int numAttributes, ...);

    // Support for deferred construction.  After calling the constructor, you
    // must set all the attributes using SetAttribute 'numAttributes' times.
    // When finished, call SetStride, which is a signal that construction is
    // complete.  The associated renderer resource is then created.
    //
    // NOTE: The read/write of VertexBufferAccessor requires that the 'offset'
    // increases with 'attribute'.  VertexFormat::Create guarantees this, but
    // you should also when using SetAttribute explicitly.
    void SetAttribute (int attribute, unsigned int streamIndex,
        unsigned int offset, AttributeType type, AttributeUsage usage,
        unsigned int usageIndex);

    void SetStride (int stride);

    // Member access.
    inline int GetNumAttributes () const;

    inline unsigned int GetStreamIndex (int attribute) const;
    inline unsigned int GetOffset (int attribute) const;
    inline AttributeType GetAttributeType (int attribute) const;
    inline AttributeUsage GetAttributeUsage (int attribute) const;
    inline unsigned int GetUsageIndex (int attribute) const;

    void GetAttribute (int attribute, unsigned int& streamIndex,
        unsigned int& offset, AttributeType& type, AttributeUsage& usage,
        unsigned int& usageIndex) const;

    inline int GetStride () const;

    // Return the index of the specified attribute usage.  The specified
    // usage index is currently used only for color channels and texture
    // coordinates.  When the attribute usage occurs in the format, the return
    // value is an index i for which 0 <= i < GetNumAttributes().  If the
    // attribute usage does not occur in the format, the return value is -1.
    int GetIndex (AttributeUsage usage, unsigned int usageIndex = 0) const;

    // The number of bytes required by a component of the attribute.
    inline static int GetComponentSize (AttributeType type);

    // The number of components of the attribute.
    inline static int GetNumComponents (AttributeType type);

    // The number of bytes for all the components of the attribute.
    inline static int GetTypeSize (AttributeType type);

protected:
    // Information for the graphics API.  Not all APIs use all the
    // information.
    class WM5_GRAPHICS_ITEM Element
    {
    public:
        unsigned int StreamIndex;
        unsigned int Offset;
        AttributeType Type;
        AttributeUsage Usage;
        unsigned int UsageIndex;
    };

    int mNumAttributes;
    Element mElements[AM_MAX_ATTRIBUTES];
    int mStride;

    static int msComponentSize[AT_QUANTITY];
    static int msNumComponents[AT_QUANTITY];
    static int msTypeSize[AT_QUANTITY];
};

WM5_REGISTER_STREAM(VertexFormat);
typedef Pointer0<VertexFormat> VertexFormatPtr;
#include "Wm5VertexFormat.inl"

}

#endif
