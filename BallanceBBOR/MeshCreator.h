#pragma once

#include <YYCHelper.h>

namespace NSBallanceBBOR::MeshCreator {

	/**
	 * @brief Add a sphere shape for mesh.
	 * @param mesh The mesh to add geomery.
	 * @param mtl[in] The mtl used for drawing this sphere.
	 * @param segments The segment count of mesh, indicating how many vertical lines.
	 * @param rings The ring count of mesh, indicating how many horizonal lines.
	 * @param radius The radius of sphere.
	*/
	void CreateSphere(CKMesh* mesh, CKMaterial* mtl, CKDWORD segments, CKDWORD rings, float radius);

}
