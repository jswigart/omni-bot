include ../Makefile.global

TARGET_DYNAMIC_SO = libOpcode.so
VERSION_DYNAMIC = 13
SUBVERSION_DYNAMIC = 1

CFLAGS =
INCLUDES = -I. -I./Ice
LIBS = 

CFLAGS_DYNAMIC = $(CFLAGS) -fPIC
LIBS_DYNAMIC = $(LIBS) -shared -W1,-soname,$(TARGET_DYNAMIC_SO).$(VERSION_DYNAMIC)

TARGET_STATIC = libOpcode.a
TARGET_DYNAMIC = $(TARGET_DYNAMIC_SO).$(VERSION_DYNAMIC).$(SUBVERSION_DYNAMIC)

SOURCES = OPC_AABBCollider.cpp OPC_AABBTree.cpp OPC_BaseModel.cpp \
			OPC_BoxPruning.cpp OPC_Collider.cpp OPC_Common.cpp \
			OPC_HybridModel.cpp	OPC_LSSCollider.cpp OPC_MeshInterface.cpp \
			OPC_Model.cpp	OPC_OBBCollider.cpp Opcode.cpp \
			OPC_OptimizedTree.cpp OPC_Picking.cpp OPC_PlanesCollider.cpp \
			OPC_RayCollider.cpp OPC_SphereCollider.cpp	OPC_SweepAndPrune.cpp \
			OPC_TreeBuilders.cpp OPC_TreeCollider.cpp OPC_VolumeCollider.cpp \
			StdAfx.cpp Ice/IceAABB.cpp Ice/IceContainer.cpp \
			Ice/IceHPoint.cpp Ice/IceIndexedTriangle.cpp  \
			Ice/IceMatrix3x3.cpp Ice/IceMatrix4x4.cpp Ice/IceOBB.cpp \
			Ice/IcePlane.cpp Ice/IcePoint.cpp Ice/IceRandom.cpp \
			Ice/IceRay.cpp Ice/IceRevisitedRadix.cpp Ice/IceSegment.cpp \
			Ice/IceTriangle.cpp Ice/IceUtils.cpp 

HEADERS = OPC_AABBCollider.h OPC_MeshInterface.h OPC_SphereAABBOverlap.h \
			OPC_AABBTree.h OPC_Model.h OPC_SphereCollider.h \
			OPC_BaseModel.h OPC_OBBCollider.h OPC_SphereTriOverlap.h \
			OPC_BoxBoxOverlap.h OPC_OptimizedTree.h OPC_SweepAndPrune.h \
			OPC_BoxPruning.h OPC_Picking.h OPC_TreeBuilders.h \
			OPC_Collider.h OPC_PlanesAABBOverlap.h OPC_TreeCollider.h \
			OPC_Common.h OPC_PlanesCollider.h OPC_TriBoxOverlap.h \
			OPC_HybridModel.h OPC_PlanesTriOverlap.h OPC_TriTriOverlap.h \
			OPC_IceHook.h OPC_RayAABBOverlap.h OPC_VolumeCollider.h \
			OPC_LSSAABBOverlap.h OPC_RayCollider.h Opcode.h \
			OPC_LSSCollider.h OPC_RayTriOverlap.h Stdafx.h \
			OPC_LSSTriOverlap.h OPC_Settings.h

OBJECTS = IceAABB.o IceTriangle.o OPC_OptimizedTree.o IceContainer.o \
			IceUtils.o OPC_Picking.o IceHPoint.o OPC_AABBCollider.o \
			OPC_PlanesCollider.o IceIndexedTriangle.o OPC_AABBTree.o \
			OPC_RayCollider.o IceMatrix3x3.o OPC_BaseModel.o \
			OPC_SphereCollider.o IceMatrix4x4.o OPC_BoxPruning.o \
			OPC_SweepAndPrune.o IceOBB.o OPC_Collider.o OPC_TreeBuilders.o \
			IcePlane.o OPC_Common.o OPC_TreeCollider.o IcePoint.o \
			OPC_HybridModel.o OPC_VolumeCollider.o IceRandom.o \
			OPC_LSSCollider.o Opcode.o IceRay.o OPC_MeshInterface.o StdAfx.o \
			IceRevisitedRadix.o OPC_Model.o IceSegment.o OPC_OBBCollider.o 

.PHONY : all
all: $(TARGET_DYNAMIC)
 
.PHONY : static
static: $(TARGET_STATIC)

.PHONY : clean

clean:
	rm *.o *.a *.so*

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) $(CFLAGS) $(INCLUDES)

OBJECTS_DYNAMIC: $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) $(CFLAGS_DYNAMIC) $(INCLUDES)

	
$(TARGET_STATIC): $(OBJECTS)
	ar rcs $(TARGET_STATIC) $(OBJECTS)
 	
$(TARGET_DYNAMIC): OBJECTS_DYNAMIC $(OBJECTS)
	$(CC) $(LIBS_DYNAMIC) -o $(TARGET_DYNAMIC) $(OBJECTS)
	ln -sf $(TARGET_DYNAMIC) $(TARGET_DYNAMIC_SO).$(VERSION_DYNAMIC)
	ln -sf $(TARGET_DYNAMIC_SO).$(VERSION_DYNAMIC) $(TARGET_DYNAMIC_SO)

