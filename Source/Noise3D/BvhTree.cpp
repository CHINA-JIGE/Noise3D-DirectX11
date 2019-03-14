/***********************************************************

					 BVH Tree and Node

		desc: BVH(Bounding Volumn Hierarchy) node and tree
		commonly used in acceleration of collision test 
		and render culling. Leaf node of BVH should have a 
		concrete scene object. Non-leaf node have just AABB
		to accelerate collision test
		(pruning branches ASAP in a fast manner)

***********************************************************/

#include "Noise3D.h"

using namespace Noise3D;
