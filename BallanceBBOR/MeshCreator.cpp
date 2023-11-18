#include "MeshCreator.h"

namespace NSBallanceBBOR::MeshCreator {

	constexpr float YYC_PI = PI;
	constexpr float YYC_2PI = YYC_PI * 2.0f;

	void CreateSphere(CKMesh* mesh, CKMaterial* mtl, CKDWORD segments, CKDWORD rings, float radius) {
		VxVector cache;

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
		float frings = static_cast<float>(rings),
			fsegments = static_cast<float>(segments);
		int idx = other_vtx_start;
		for (int iring = 0; iring < rings; ++iring) {
			for (int iseg = 0; iseg < segments; ++iseg) {
				// set vertex position
				// ring control the y data and segment control xz data
				cache.Set(
					std::cos((iseg / fsegments) * YYC_2PI) * radius,
					std::cos((iring / frings) * YYC_PI) * radius,
					std::sin((iseg / fsegments) * YYC_2PI) * radius
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
		// each rectangle created by rings and segments will cause 2 face.
		// we have "rings - 1" horizontal belt and "segments" vertical belt.
		// so we have this face count:
		int face_base = mesh->GetFaceCount();
		mesh->SetFaceCount(face_base + ((rings - 1) * segments) * 2);

		// todo: create face


		// notify changes
		mesh->NormalChanged();
		mesh->UVChanged();
	}

}