// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Terrain.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, Terrain);
WM5_IMPLEMENT_STREAM(Terrain);
WM5_IMPLEMENT_FACTORY(Terrain);

//----------------------------------------------------------------------------
Terrain::Terrain (const std::string& heightName, VertexFormat* vformat,
    Camera* camera, int mode)
    :
    mMode(mode),
    mVFormat(vformat),
    mCameraRow(-1),
    mCameraCol(-1),
    mCamera(camera)
{
    // Load global terrain information.
    LoadHeader(heightName);

    // Load terrain pages.
    mPages = new2<TerrainPagePtr>(mNumCols, mNumRows);
    int row, col;
    for (row = 0; row < mNumRows; ++row)
    {
        for (col = 0; col < mNumCols; ++col)
        {
            char temp[64];
            sprintf(temp, "%d.%d", row, col);
            std::string heightSuffix(temp);
            LoadPage(row, col, heightName, heightSuffix);
        }
    }

    // Attach the terrain pages to the terrain node.
    mChild.resize(mNumRows*mNumCols);
    for (row = 0; row < mNumRows; ++row)
    {
        for (col = 0; col < mNumCols; ++col)
        {
            AttachChild(mPages[row][col]);
        }
    }
}
//----------------------------------------------------------------------------
Terrain::~Terrain ()
{
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            mPages[row][col] = 0;
        }
    }
    delete2(mPages);
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetPage (int row, int col)
{
    if (0 <= row && row < mNumRows && 0 <= col && col < mNumCols)
    {
        return mPages[row][col];
    }

    assertion(false, "Invalid row or column index\n");
    return 0;
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetCurrentPage (float x, float y) const
{
    float invLength = 1.0f/(mSpacing*(float)(mSize - 1));

    int col = (int)Mathf::Floor(x*invLength);
    col %= mNumCols;
    if (col < 0)
    {
        col += mNumCols;
    }

    int row = (int)Mathf::Floor(y*invLength);
    row %= mNumRows;
    if (row < 0)
    {
        row += mNumRows;
    }

    return mPages[row][col];
}
//----------------------------------------------------------------------------
float Terrain::GetHeight (float x, float y) const
{
    TerrainPage* page = GetCurrentPage(x,y);

    // Subtract off the translation due to wrap-around.
    x -= page->LocalTransform.GetTranslate().X();
    y -= page->LocalTransform.GetTranslate().Y();

    return page->GetHeight(x, y);
}
//----------------------------------------------------------------------------
AVector Terrain::GetNormal (float x, float y) const
{
    float xp = x + mSpacing;
    float xm = x - mSpacing;
    float yp = y + mSpacing;
    float ym = y - mSpacing;

    TerrainPage* page = GetCurrentPage(xp, y);
    float xtmp = xp - page->LocalTransform.GetTranslate().X();
    float ytmp = y - page->LocalTransform.GetTranslate().Y();
    float hpz = page->GetHeight(xtmp,ytmp);

    page = GetCurrentPage(xm, y);
    xtmp = xm - page->LocalTransform.GetTranslate().X();
    ytmp = y - page->LocalTransform.GetTranslate().Y();
    float hmz = page->GetHeight(xtmp,ytmp);

    page = GetCurrentPage(x, yp);
    xtmp = x - page->LocalTransform.GetTranslate().X();
    ytmp = yp - page->LocalTransform.GetTranslate().Y();
    float hzp = page->GetHeight(xtmp,ytmp);

    page = GetCurrentPage(x, ym);
    xtmp = x - page->LocalTransform.GetTranslate().X();
    ytmp = ym - page->LocalTransform.GetTranslate().Y();
    float hzm = page->GetHeight(xtmp,ytmp);

    AVector normal(hmz - hpz, hzm - hzp, 1.0f);
    normal.Normalize();
    return normal;
}
//----------------------------------------------------------------------------
void Terrain::LoadHeader (const std::string& heightName)
{
    // Load the data into temporary variables.  EndianCopy uses memcpy, so
    // if you were to load directly into the class members and use memcpy,
    // the source and destination overlap, in which case memcpy results are
    // unpredictable.
    std::string fileName = heightName + ".wmhf";
    FileIO header(fileName, mMode);
    assertion(header, "Cannot open file %s\n", fileName);

    header.Read(sizeof(int), &mNumRows);
    header.Read(sizeof(int), &mNumCols);

    unsigned short size;
    header.Read(sizeof(unsigned short), &size);
    mSize = (int)size;

    header.Read(sizeof(float), &mMinElevation);
    header.Read(sizeof(float), &mMaxElevation);
    header.Read(sizeof(float), &mSpacing);

    header.Close();
}
//----------------------------------------------------------------------------
void Terrain::LoadPage (int row, int col, const std::string& heightName,
    const std::string& heightSuffix)
{
    int numHeights = mSize*mSize;
    unsigned short* heights = new1<unsigned short>(numHeights);
    std::string fileName = heightName + "." + heightSuffix + ".wmhf";
    FileIO heightFile(fileName, mMode);
    assertion(heightFile, "Cannot open file %s\n", fileName);
    if (heightFile)
    {
        heightFile.Read(sizeof(unsigned short), numHeights, heights);
        heightFile.Close();
    }
    else
    {
        memset(heights, 0, numHeights*sizeof(unsigned short));
    }

    float length = mSpacing*(float)(mSize - 1);
    Float2 origin(col*length, row*length);
    TerrainPage* page = new0 TerrainPage(mVFormat, mSize, heights, origin,
        mMinElevation, mMaxElevation, mSpacing);

    mPages[row][col] = page;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int row, int col,
    const std::string& heightName, const std::string& heightSuffix)
{
    if (0 <= row && row < mNumRows && 0 <= col && col < mNumCols)
    {
        TerrainPagePtr save = mPages[row][col];
        LoadPage(row, col, heightName, heightSuffix);
        return save;
    }

    assertion(false, "Invalid row or column index\n");
    return 0;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int row, int col, TerrainPage* newPage)
{
    if (0 <= row && row < mNumRows && 0 <= col && col < mNumCols)
    {
        TerrainPagePtr save = mPages[row][col];
        mPages[row][col] = newPage;
        return save;
    }

    assertion(false, "Invalid row or column index\n");
    return 0;
}
//----------------------------------------------------------------------------
void Terrain::OnCameraMotion ()
{
    assertion(mCamera != 0, "Camera must exist\n");
    if (!mCamera)
    {
        return;
    }

    // Get camera location/direction in model space of terrain.
    APoint worldEye = mCamera->GetPosition();
    AVector worldDir = mCamera->GetDVector();
    APoint modelEye = WorldTransform.Inverse()*worldEye;
    AVector modelDir = WorldTransform.Inverse()*worldDir;

    // Update the model-space origins of the terrain pages.  Start the
    // process by locating the page that contains the camera.
    float length = mSpacing*(float)(mSize - 1);
    float invLength = 1.0f/length;
    int newCameraCol = (int)Mathf::Floor(modelEye.X()*invLength);
    int newCameraRow = (int)Mathf::Floor(modelEye.Y()*invLength);
    if (newCameraCol != mCameraCol || newCameraRow != mCameraRow)
    {
        mCameraCol = newCameraCol;
        mCameraRow = newCameraRow;

        // Translate page origins for toroidal wraparound.
        int cminO = mCameraCol - mNumCols/2;
        int cminP = cminO % mNumCols;
        if (cminP < 0)
        {
            cminP += mNumCols;
        }

        int rminO = mCameraRow - mNumRows/2;
        int rminP = rminO % mNumRows;
        if (rminP < 0)
        {
            rminP += mNumRows;
        }

        int rO = rminO, rP = rminP;
        for (int row = 0; row < mNumRows; ++row)
        {
            int cO = cminO, cP = cminP;
            for (int col = 0; col < mNumCols; ++col)
            {
                TerrainPage* page = mPages[rP][cP];
                Float2 oldOrigin = page->GetOrigin();
                Float2 newOrigin(cO*length, rO*length);
                APoint pageTrn(
                    newOrigin[0] - oldOrigin[0],
                    newOrigin[1] - oldOrigin[1],
                    page->LocalTransform.GetTranslate().Z());
                page->LocalTransform.SetTranslate(pageTrn);

                ++cO;
                if (++cP == mNumCols)
                {
                    cP = 0;
                }
            }

            ++rO;
            if (++rP == mNumRows)
            {
                rP = 0;
            }
        }
        Update();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* Terrain::GetObjectByName (const std::string& name)
{
    Object* found = Node::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mVFormat, name, found);
    WM5_GET_OBJECT_BY_NAME(mCamera, name, found);
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            WM5_GET_OBJECT_BY_NAME(mPages[row][col], name, found);
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Terrain::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Node::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mVFormat, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mCamera, name, objects);
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            WM5_GET_ALL_OBJECTS_BY_NAME(mPages[row][col], name, objects);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Terrain::Terrain (LoadConstructor value)
    :
    Node(value),
    mMode(FileIO::FM_NONE),
    mNumRows(0),
    mNumCols(0),
    mSize(0),
    mMinElevation(0.0f),
    mMaxElevation(0.0f),
    mSpacing(0.0f),
    mPages(0),
    mCameraRow(0),
    mCameraCol(0)
{
}
//----------------------------------------------------------------------------
void Terrain::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadEnum(mMode);
    source.Read(mNumRows);
    source.Read(mNumCols);
    source.Read(mSize);
    source.Read(mMinElevation);
    source.Read(mMaxElevation);
    source.Read(mSpacing);
    source.Read(mCameraRow);
    source.Read(mCameraCol);
    source.ReadPointer(mVFormat);
    source.ReadPointer(mCamera);

    mPages = new2<TerrainPagePtr>(mNumCols, mNumRows);
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            source.ReadPointer(mPages[row][col]);
        }
    }

    WM5_END_DEBUG_STREAM_LOAD(Terrain, source);
}
//----------------------------------------------------------------------------
void Terrain::Link (InStream& source)
{
    Node::Link(source);

    source.ResolveLink(mVFormat);
    source.ResolveLink(mCamera);
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            source.ResolveLink(mPages[row][col]);
        }
    }
}
//----------------------------------------------------------------------------
void Terrain::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool Terrain::Register (OutStream& target) const
{
    if (Node::Register(target))
    {
        target.Register(mVFormat);
        target.Register(mCamera);
        for (int row = 0; row < mNumRows; ++row)
        {
            for (int col = 0; col < mNumCols; ++col)
            {
                target.Register(mPages[row][col]);
            }
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Terrain::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.WriteEnum(mMode);
    target.Write(mNumRows);
    target.Write(mNumCols);
    target.Write(mSize);
    target.Write(mMinElevation);
    target.Write(mMaxElevation);
    target.Write(mSpacing);
    target.Write(mCameraRow);
    target.Write(mCameraCol);
    target.WritePointer(mVFormat);
    target.WritePointer(mCamera);

    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            target.WritePointer(mPages[row][col]);
        }
    }

    WM5_END_DEBUG_STREAM_SAVE(Terrain, target);
}
//----------------------------------------------------------------------------
int Terrain::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += WM5_ENUMSIZE(mMode);
    size += sizeof(mNumRows);
    size += sizeof(mNumCols);
    size += sizeof(mSize);
    size += sizeof(mMinElevation);
    size += sizeof(mMaxElevation);
    size += sizeof(mSpacing);
    size += sizeof(mCameraRow);
    size += sizeof(mCameraCol);
    size += WM5_POINTERSIZE(mVFormat);
    size += WM5_POINTERSIZE(mCamera);
    size += mNumRows*mNumCols*WM5_POINTERSIZE(mPages[0][0]);
    return size;
}
//----------------------------------------------------------------------------
