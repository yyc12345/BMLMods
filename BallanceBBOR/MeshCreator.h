#pragma once

#include <YYCHelper.h>

namespace NSBallanceBBOR::MeshCreator {

	/**
	 * @brief Transform the whole mesh.
	 * @param mesh[in] The mesh to be transformed
	 * @param mat[in] Transform matrix.
	*/
	void TransformMesh(CKMesh* mesh, const VxMatrix& mat);

	/**
	 * @brief Add a cube shape to mesh.
	 * @param mesh[in] The mesh to add geomery.
	 * @param mtl[in] The mtl used for drawing this geometry.
	 * @param size[in] The edge length of this cube.
	*/
	void CreateCube(CKMesh* mesh, CKMaterial* mtl, float size);

	/**
	 * @brief Add a plane (square rectangle) shape to mesh. The face normal will point to +Y.
	 * @param mesh[in] The mesh to add geomery.
	 * @param mtl[in] The mtl used for drawing this geometry.
	 * @param size[in] The edge length of this square.
	*/
	void CreatePlane(CKMesh* mesh, CKMaterial* mtl, float size);

	/**
	 * @brief Add a sphere shape to mesh.
	 * @param mesh[in] The mesh to add geomery.
	 * @param mtl[in] The mtl used for drawing this sphere.
	 * @param segments[in] The segment count of mesh, indicating how many vertical lines.
	 * @param rings[in] The ring count of mesh, indicating how many horizonal lines.
	 * @param radius[in] The radius of sphere.
	*/
	void CreateSphere(CKMesh* mesh, CKMaterial* mtl, int segments, int rings, float radius);

}
