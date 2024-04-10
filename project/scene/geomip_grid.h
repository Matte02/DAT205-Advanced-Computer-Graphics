#ifndef GEOMIP_GRID_H
#define GEOMIP_GRID_H

#include <GL/glew.h>
#include <vector>

#include <glm/glm.hpp>
#include "lod_manager.h"

using namespace glm;


// this header is included by terrain.h so we have a forward 
// declaration for BaseTerrain.
class BaseTerrain;

class GeomipGrid {
public:
	GeomipGrid();

	~GeomipGrid();

	void CreateGeomipGrid(int Width, int Depth, int PatchSize, const BaseTerrain* pTerrain);
	void UpdateVertices(const BaseTerrain* pTerrain);

	void Destroy();

	void Render(const vec3& CameraPos);

private:

	struct Vertex {
		vec3 Pos;
		vec2 Tex;
		vec3 Normal = vec3(0.0f, 0.0f, 0.0f);

		void InitVertex(const BaseTerrain* pTerrain, int x, int z);
	};

	void CreateGLState();

	void PopulateBuffers(const BaseTerrain* pTerrain);

	void InitVertices(const BaseTerrain* pTerrain, std::vector<Vertex>& Vertices);

	int InitIndices(std::vector<uint>& Indices);
	int InitIndicesLOD(int Index, std::vector<uint>& Indices, int lod);
	int InitIndicesLODSingle(int Index, std::vector<uint>& Indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom);
	int CalcNumIndices();

	void CalcNormals(std::vector<Vertex>& Vertices, const std::vector<uint>& Indices);

	uint AddTriangle(uint Index, std::vector<uint>& Indices, uint v1, uint v2, uint v3);
	uint CreateTriangleFan(int Index, std::vector<uint>& Indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int y);


	int m_width = 0;
	int m_depth = 0;

	int m_patchSize = 0;
	int m_numPatchesX = 0;
	int m_numPatchesZ = 0;
	int m_maxLOD = 0;

	struct SingleLodInfo {
		int Start = 0;
		int Count = 0;
	};

#define LEFT 2
#define RIGHT 2
#define TOP 2
#define BOTTOM 2

	struct LodInfo {
		SingleLodInfo info[LEFT][RIGHT][TOP][BOTTOM];
	};

	std::vector<LodInfo> m_lodInfo;
	LodManager m_lodManager;


	GLuint m_vao = 0;
	GLuint m_vb = 0;
	GLuint m_ib = 0;
	std::vector<uint> Indices;

};
#endif