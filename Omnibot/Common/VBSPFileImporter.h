////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef VBSPFILEIMPORTER_H_INC
#define VBSPFILEIMPORTER_H_INC

#include <vector>
#include <string>

#include "Omni-Bot_Color.h"
#include "Wm5Mathematics.h"
using namespace Wm5;

#include "CollisionModel.h"

namespace MapFormat
{
	namespace VBSP
	{
		struct VBSPModel;
		struct dleaf_t;
	}

	class VBSPFileImporter
	{
	public:
		VBSPFileImporter();
		~VBSPFileImporter();
	public:
		bool ParseFile( CollisionWorld & scene, const char * memoryBuffer, size_t memoryBufferSize );
	private:
		void CreateSceneGraph( CollisionWorld & scene, NodePtr & parent, const VBSP::VBSPModel & bspmodel );
		void WalkNodeTree( CollisionWorld & scene, ModelPtr geom, const VBSP::VBSPModel & bspmodel, int subModelIndex, int bspNodeIndex );
		void ProcessFace( CollisionWorld & scene, ModelPtr geom, const VBSP::VBSPModel & bspmodel, int subModelIndex, const VBSP::dleaf_t & face );
	};

	// ------------------------------------------------------------------------------------------------
};

#endif
