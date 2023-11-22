#include "MeshCreator.h"
#include "MatrixBuilder.h"
#include <numbers>
#include <array>

namespace NSBallanceBBOR::MeshCreator {

	// MARK: i don't know why, but it seems that Virtools use CW (clockwise), not CCW (counter-clockerwise) to check face normal.
	// But all evidence show that Virtools should be CCW one. That is insane.
	// So i was forced reset all face as CW order.

#pragma region Help Functions

	constexpr float YYC_PI = std::numbers::pi_v<float>;
	constexpr float YYC_2PI = YYC_PI * 2.0f;

	/**
	 * @brief A circular index with very limited usage but have high performance.
	 * @param index[int] The index to compute circular index. It should not minus than zero or greater than `(2 * max_index) - 1`
	 * because this function only can handle only at least 1 loop.
	 * @param max_index[in] The max index value. Can not <= 0.
	 * @return
	*/
	static int NotRobustCircularIndex(int index, int max_index) {
		if (index < max_index) return index;
		else return index - max_index;
	}

	/**
	 * @brief Force;y notify mesh its data changed.
	 * @param mesh[in] Mesh to be notified.
	*/
	static void OrderUpdateMesh(CKMesh* mesh) {
		mesh->VertexMove();
		mesh->NormalChanged();
		mesh->UVChanged();
	}

	/**
	 * @brief Transform vertices of mesh with specific range.
	 * @param mesh[in] The mesh.
	 * @param mat[in] The transform matrix
	 * @param vtx_start[in] The vertex start index
	 * @param vtx_count[in] The count of operated vertices.
	*/
	static void TransformVertexRange(CKMesh* mesh, const VxMatrix& mat, int vtx_start, int vtx_count) {
		// prepare 2 processing cahce variables.
		VxVector get_val, set_val;

		// prepare normal used matrix.
		// https://zhuanlan.zhihu.com/p/96717729
		MatrixBuilder builder;
		VxMatrix nml_mat(builder.Set(mat).Inverse().Transpose().Build());

		// process vertex in range
		for (int i = 0; i < vtx_count; ++i) {
			int idx = vtx_start + i;

			// transform position
			mesh->GetVertexPosition(idx, &get_val);
			Vx3DMultiplyMatrixVector(&set_val, mat, &get_val);
			mesh->SetVertexPosition(idx, &set_val);

			// transform normal
			mesh->GetVertexNormal(idx, &get_val);
			Vx3DMultiplyMatrixVector(&set_val, nml_mat, &get_val);
			set_val.Normalize();
			mesh->SetVertexNormal(idx, &set_val);
		}
	}

#pragma endregion

	void TransformMesh(CKMesh* mesh, const VxMatrix& mat) {
		TransformVertexRange(mesh, mat, 0, mesh->GetVertexCount());
	}

	void CreateCube(CKMesh* mesh, CKMaterial* mtl, float size) {
		// regulate size
		if (size <= 0.0f) size = 5.0f;
		float halfsize = size / 2;

		// create cube use plane creator and vertex transfomer to create 6 face of this geometry
		// prepare 6 faces matrix
		MatrixBuilder builder;
		VxMatrix px(builder.Scale(size, 1, size).RotateWithEulerAngles(0, 0, -90).Move(halfsize, 0, 0).Build());
		VxMatrix pz(builder.Scale(size, 1, size).RotateWithEulerAngles(90, 0, 0).Move(0, 0, halfsize).Build());
		VxMatrix nx(builder.Scale(size, 1, size).RotateWithEulerAngles(0, 0, 90).Move(-halfsize, 0, 0).Build());
		VxMatrix nz(builder.Scale(size, 1, size).RotateWithEulerAngles(-90, 0, 0).Move(0, 0, -halfsize).Build());
		VxMatrix py(builder.Scale(size, 1, size).Move(0, halfsize, 0).Build());
		VxMatrix ny(builder.Scale(size, 1, size).RotateWithEulerAngles(180, 0, 0).Move(0, -halfsize, 0).Build());

		static std::array<VxMatrix*, 6> face_mats {
			&px, &pz, &nx, &nz, &py, &ny
		};

		// create face
		static int vtx_per_pace = 4;
		int vtx_base = 0;
		for (const auto* pmat : face_mats) {
			CreatePlane(mesh, mtl, 1.0f);
			TransformVertexRange(mesh, *pmat, vtx_base, vtx_per_pace);
			vtx_base += vtx_per_pace;
		}

		// notify changes
		OrderUpdateMesh(mesh);
	}

	void CreatePlane(CKMesh* mesh, CKMaterial* mtl, float size) {
		// create cahce and normal (+Y)
		VxVector universal_normal(0.0f, 1.0f, 0.0f);
		VxVector cache;
		// regulate size
		if (size <= 0.0f) size = 5.0f;
		// shrink size to half size.
		size /= 2.0f;

		// create 4 vector.
		// we created plane is in XZ plane.
		// we use p ad positive(+) and n for negtative(-), so pxpz is mean the point in +x +y.
		// resize first
		int vertex_base = mesh->GetVertexCount();
		mesh->SetVertexCount(vertex_base + 4);

		// set index and assign
		int pxpz_idx = vertex_base,
			nxpz_idx = vertex_base + 1,
			nxnz_idx = vertex_base + 2,
			pxnz_idx = vertex_base + 3;

		cache.Set(size, 0.0f, size);
		mesh->SetVertexPosition(pxpz_idx, &cache);
		mesh->SetVertexNormal(pxpz_idx, &universal_normal);
		mesh->SetVertexTextureCoordinates(pxpz_idx, 0.0f, 0.0f);

		cache.Set(-size, 0.0f, size);
		mesh->SetVertexPosition(nxpz_idx, &cache);
		mesh->SetVertexNormal(nxpz_idx, &universal_normal);
		mesh->SetVertexTextureCoordinates(nxpz_idx, 0.0f, 0.0f);

		cache.Set(-size, 0.0f, -size);
		mesh->SetVertexPosition(nxnz_idx, &cache);
		mesh->SetVertexNormal(nxnz_idx, &universal_normal);
		mesh->SetVertexTextureCoordinates(nxnz_idx, 0.0f, 0.0f);

		cache.Set(size, 0.0f, -size);
		mesh->SetVertexPosition(pxnz_idx, &cache);
		mesh->SetVertexNormal(pxnz_idx, &universal_normal);
		mesh->SetVertexTextureCoordinates(pxnz_idx, 0.0f, 0.0f);

		// create 2 face.
		int face_base = mesh->GetFaceCount();
		mesh->SetFaceCount(face_base + 2);
		int pxpz_face_idx = face_base,
			nxnz_face_idx = face_base + 1;

		mesh->SetFaceVertexIndex(pxpz_face_idx, pxpz_idx, pxnz_idx, nxpz_idx);
		mesh->SetFaceMaterial(pxpz_face_idx, mtl);

		mesh->SetFaceVertexIndex(nxnz_face_idx, nxnz_idx, nxpz_idx, pxnz_idx);
		mesh->SetFaceMaterial(nxnz_face_idx, mtl);

		// notify changes
		OrderUpdateMesh(mesh);
	}

	void CreateCircle(CKMesh* mesh, CKMaterial* mtl, int segments, float radius) {
		// create cahce
		VxVector cache;
		// regulate segments
		if (segments < 3) segments = 3;
		// regulate radius
		if (radius <= 0.0f) radius = 5.0f;


		// get vertex size and resize
		int vertex_base = mesh->GetVertexCount();
		mesh->SetVertexCount(vertex_base + 1 + segments);	// 1 is center vertex.

		// calc some offset
		int center_vtx_idx = vertex_base,
			other_vtx_start = vertex_base + 1;

		// assign centervertex at first place
		cache.Set(0, 0, 0);
		mesh->SetVertexPosition(center_vtx_idx, &cache);
		cache.Set(0, 1.0f, 0);
		mesh->SetVertexNormal(center_vtx_idx, &cache);
		mesh->SetVertexTextureCoordinates(center_vtx_idx, 0.0f, 0.0f);

		// assign other vertex
		int idx = other_vtx_start;
		float fsegments = static_cast<float>(segments);
		for (int iseg = 0; iseg < segments; ++iseg) {
			// set vertex position
			// use triangle function to compute
			float circle_theta = iseg / fsegments * YYC_2PI;
			cache.Set(
				std::cos(circle_theta) * radius,
				0.0f,
				std::sin(circle_theta) * radius
			);
			mesh->SetVertexPosition(idx, &cache);

			// set normal
			cache.Set(0, 1.0f, 0);
			mesh->SetVertexNormal(idx, &cache);

			// set uv and inc index
			mesh->SetVertexTextureCoordinates(idx, 0.0f, 0.0f);
			++idx;
		}

		// resize face count.
		// all faces is triangle faces, and equal with the segments count
		int face_base = mesh->GetFaceCount();
		mesh->SetFaceCount(face_base + segments);

		// create faces
		idx = face_base;
		for (int iseg = 0; iseg < segments; ++iseg) {
			// compute vertex index
			int this_vtx = other_vtx_start + iseg,
				next_vtx = other_vtx_start + NotRobustCircularIndex(iseg + 1, segments);

			// create face
			mesh->SetFaceVertexIndex(idx, center_vtx_idx, next_vtx, this_vtx);
			mesh->SetFaceMaterial(idx, mtl);
			++idx;
		}

		// notify changes
		OrderUpdateMesh(mesh);
	}

	void CreateCylinder(CKMesh* mesh, CKMaterial* mtl, int segments, float radius, float depth, bool has_cap) {
		// create cahce
		VxVector cache;
		// regulate segments
		if (segments < 3) segments = 3;
		// regulate radius and depth
		if (radius <= 0.0f) radius = 5.0f;
		if (depth <= 0.0f) depth = 1.0f;
		// shrink depth to half size.
		depth /= 2.0f;

		// get vertex size and resize
		int vertex_base = mesh->GetVertexCount();
		mesh->SetVertexCount(vertex_base + segments * 2);	// we have top ring and bottom ring, so we have double segment vertices

		// calc some offset
		int top_vtx_idx = vertex_base,
			bottom_vtx_idx = vertex_base + segments;

		// assign vertex
		float fsegments = static_cast<float>(segments);
		for (int iseg = 0; iseg < segments; ++iseg) {
			// compute index
			int top_seg_idx = top_vtx_idx + iseg,
				bottom_seg_idx = bottom_vtx_idx + iseg;

			// set top ring vertex position for this segment
			float circle_theta = iseg / fsegments * YYC_2PI;
			cache.Set(
				std::cos(circle_theta) * radius,
				depth,
				std::sin(circle_theta) * radius
			);
			mesh->SetVertexPosition(top_seg_idx, &cache);
			// then invert y factor and set for bottom ring corresponding vertex
			cache.y = -cache.y;
			mesh->SetVertexPosition(bottom_seg_idx, &cache);

			// set normal, set y to 0 and normalize to make it radiate to outside.
			cache.y = 0.0f;
			cache.Normalize();
			// and set both of top and bottom rings
			mesh->SetVertexNormal(top_seg_idx, &cache);
			mesh->SetVertexNormal(bottom_seg_idx, &cache);

			// set uv
			mesh->SetVertexTextureCoordinates(top_seg_idx, 0.0f, 0.0f);
			mesh->SetVertexTextureCoordinates(bottom_seg_idx, 0.0f, 0.0f);
		}

		// resize face count.
		// all side face is rectangle faces, and it should be splitted into 2 triangle faces.
		// we have `segments` rectangles, so we have `2 * segments` triangles.
		int face_base = mesh->GetFaceCount();
		mesh->SetFaceCount(face_base + 2 * segments);

		// create faces
		int idx = face_base;
		for (int iseg = 0; iseg < segments; ++iseg) {
			// compute vertex index
			int top_line_this_vtx = top_vtx_idx + iseg,
				top_line_next_vtx = top_vtx_idx + NotRobustCircularIndex(iseg + 1, segments);
			int bottom_line_this_vtx = bottom_vtx_idx + iseg,
				bottom_line_next_vtx = bottom_vtx_idx + NotRobustCircularIndex(iseg + 1, segments);

			// create face
			mesh->SetFaceVertexIndex(idx, top_line_this_vtx, top_line_next_vtx, bottom_line_this_vtx);
			mesh->SetFaceMaterial(idx, mtl);
			++idx;
			mesh->SetFaceVertexIndex(idx, top_line_next_vtx, bottom_line_next_vtx, bottom_line_this_vtx);
			mesh->SetFaceMaterial(idx, mtl);
			++idx;
		}

		// create cap by CreateCircle
		if (has_cap) {
			// create mat builder and create top and bottom cap matrix
			MatrixBuilder builder;
			VxMatrix top_cap(builder.Move(0, depth, 0).Build());
			VxMatrix bottom_cap(builder.RotateWithEulerAngles(180, 0, 0).Move(0, -depth, 0).Build());

			// start create cap
			int vtx_per_pace = segments + 1;
			int vtx_base = mesh->GetVertexCount();
			// create top cap
			CreateCircle(mesh, mtl, segments, radius);
			TransformVertexRange(mesh, top_cap, vtx_base, vtx_per_pace);
			vtx_base += vtx_per_pace;
			// create bottom cap
			CreateCircle(mesh, mtl, segments, radius);
			TransformVertexRange(mesh, bottom_cap, vtx_base, vtx_per_pace);
			vtx_base += vtx_per_pace;
		}
		
		// notify changes
		OrderUpdateMesh(mesh);
	}

	void CreateSphere(CKMesh* mesh, CKMaterial* mtl, int segments, int rings, float radius) {
		// create cahce
		VxVector cache;
		// regulate segments and rings
		if (segments < 3) segments = 3;
		if (rings < 3) rings = 3;
		// regulate radius
		if (radius <= 0.0f) radius = 5.0f;

		// get vertex size and resize
		int vertex_base = mesh->GetVertexCount();
		mesh->SetVertexCount(vertex_base + 2 + segments * rings);	// 2 is top and bottom vertex.

		// calc some offset
		int top_vtx_idx = vertex_base,
			bottom_vtx_idx = vertex_base + 1,
			other_vtx_start = vertex_base + 2;

		// assign top and bottom vertex at first 2 place
		cache.Set(0, radius, 0);
		mesh->SetVertexPosition(top_vtx_idx, &cache);
		cache.Normalize();
		mesh->SetVertexNormal(top_vtx_idx, &cache);
		mesh->SetVertexTextureCoordinates(top_vtx_idx, 0.0f, 0.0f);

		cache.Set(0, -radius, 0);
		mesh->SetVertexPosition(bottom_vtx_idx, &cache);
		cache.Normalize();
		mesh->SetVertexNormal(bottom_vtx_idx, &cache);
		mesh->SetVertexTextureCoordinates(bottom_vtx_idx, 0.0f, 0.0f);

		// assign other vertex
		// we draw ring from top to bottom (+y -> -y).
		// and draw segment from +x -> +z -> -x -> -z.
		float frings = static_cast<float>(rings + 1),
			fsegments = static_cast<float>(segments);
		int idx = other_vtx_start;
		for (int iring = 0; iring < rings; ++iring) {
			for (int iseg = 0; iseg < segments; ++iseg) {
				// set vertex position
				// ring control the y data and segment control xz data
				float sphere_theta = (iring + 1) / frings * YYC_PI,
					sphere_phi = (iseg / fsegments) * YYC_2PI;
				cache.Set(
					std::cos(sphere_phi) * radius * std::sin(sphere_theta),
					std::cos(sphere_theta) * radius,
					std::sin(sphere_phi) * radius * std::sin(sphere_theta)
				);
				mesh->SetVertexPosition(idx, &cache);

				// set normal
				cache.Normalize();
				mesh->SetVertexNormal(idx, &cache);

				// set uv and inc index
				mesh->SetVertexTextureCoordinates(idx, 0.0f, 0.0f);
				++idx;
			}
		}

		// resize face count.
		// first, top and bottom face is pure triangle and have `2 * segments` faces.
		// second, the amid between top and bottom face is the rectangle face created by rings and segments.
		// each rectangle face will be splited into 2 triangle faces.
		// and we have rectangle face `(rings - 1) * segments`
		// so we have triangle face double of it.
		int face_base = mesh->GetFaceCount();
		int top_face_count = segments,
			bottom_face_count = segments,
			other_face_count = ((rings - 1) * segments) * 2;
		mesh->SetFaceCount(
			face_base +
			top_face_count + bottom_face_count +
			other_face_count
		);
		int top_face_start = face_base,
			bottom_face_start = face_base + top_face_count,
			other_face_start = face_base + top_face_count + bottom_face_count;

		// create top bottom faces
		for (int iseg = 0; iseg < segments; ++iseg) {
			// compute line start
			int top_line_start = other_vtx_start,
				bottom_line_start = other_vtx_start + (rings - 1) * segments;

			// compute vertex index
			int top_line_this_vtx = top_line_start + iseg,
				top_line_next_vtx = top_line_start + NotRobustCircularIndex(iseg + 1, segments);
			int bottom_line_this_vtx = bottom_line_start + iseg,
				bottom_line_next_vtx = bottom_line_start + NotRobustCircularIndex(iseg + 1, segments);

			// create top face
			mesh->SetFaceVertexIndex(top_face_start + iseg, top_vtx_idx, top_line_next_vtx, top_line_this_vtx);
			mesh->SetFaceMaterial(top_face_start + iseg, mtl);

			// create bottom face
			mesh->SetFaceVertexIndex(bottom_face_start + iseg, bottom_vtx_idx, bottom_line_this_vtx, bottom_line_next_vtx);
			mesh->SetFaceMaterial(bottom_face_start + iseg, mtl);
		}

		// create other faces
		idx = other_face_start;
		for (int iring = 0; iring < rings - 1; ++iring) {
			// get ring start index
			int this_line_start = other_vtx_start + (iring * segments),
				next_line_start = other_vtx_start + ((iring + 1) * segments);

			// loop segments
			for (int iseg = 0; iseg < segments; ++iseg) {
				// compute index
				int this_line_this_vtx = this_line_start + iseg,
					this_line_next_vtx = this_line_start + NotRobustCircularIndex(iseg + 1, segments);
				int next_line_this_vtx = next_line_start + iseg,
					next_line_next_vtx = next_line_start + NotRobustCircularIndex(iseg + 1, segments);

				// create top-left triangle
				mesh->SetFaceVertexIndex(idx, this_line_this_vtx, this_line_next_vtx, next_line_this_vtx);
				mesh->SetFaceMaterial(idx, mtl);
				++idx;
				mesh->SetFaceVertexIndex(idx, this_line_next_vtx, next_line_next_vtx, next_line_this_vtx);
				mesh->SetFaceMaterial(idx, mtl);
				++idx;
			}
		}

		// notify changes
		OrderUpdateMesh(mesh);
	}

}