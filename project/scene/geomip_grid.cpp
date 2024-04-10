#include "geomip_grid.h"

#include <stdio.h>
#include <vector>

#include <glm/glm.hpp>
using namespace glm;

#include "geomip_grid.h"
#include "terrain.h"
#include "perf.h"
#include <iostream>
#include <omp.h>
/***
*** Based on the Triangle List Class
***/

GeomipGrid::GeomipGrid()
{
}

GeomipGrid::~GeomipGrid()
{
	Destroy();
}


void GeomipGrid::Destroy()
{
	if (m_vao > 0) {
		glDeleteVertexArrays(1, &m_vao);
	}

	if (m_vb > 0) {
		glDeleteBuffers(1, &m_vb);
	}

	if (m_ib > 0) {
		glDeleteBuffers(1, &m_ib);
	}
}

void GeomipGrid::CreateGeomipGrid(int Width, int Depth, int PatchSize, const BaseTerrain* pTerrain) {


	m_width = Width;
	m_depth = Depth;

	m_patchSize = PatchSize;
	m_numPatchesX = (m_width - 1) / (m_patchSize - 1);
	m_numPatchesZ = (m_depth - 1) / (m_patchSize - 1);

	m_maxLOD = m_lodManager.InitLodManager(PatchSize, m_numPatchesX, m_numPatchesZ, pTerrain->GetWorldScale());
	printf("GeomipGrid setting max lod to: %d\n", m_maxLOD);
	m_lodInfo.resize(m_maxLOD + 1);

	CreateGLState();

	PopulateBuffers(pTerrain);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeomipGrid::UpdateVertices(const BaseTerrain* pTerrain) {
	// Vertices
	std::vector<Vertex> Vertices;
	Vertices.resize(m_width * m_depth);
	InitVertices(pTerrain, Vertices);

	CalcNormals(Vertices, Indices);

	// Bind the vertex buffer and update the data
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * Vertices.size(), &Vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GeomipGrid::CreateGLState()
{
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vb);

	glBindBuffer(GL_ARRAY_BUFFER, m_vb);

	glGenBuffers(1, &m_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);

	int POS_LOC = 0;
	int TEX_LOC = 1;
	int NORMAL_LOC = 2;

	size_t NumFloats = 0;

	glEnableVertexAttribArray(POS_LOC);
	glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
	NumFloats += 3;

	glEnableVertexAttribArray(TEX_LOC);
	glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
	NumFloats += 2;

	glEnableVertexAttribArray(NORMAL_LOC);
	glVertexAttribPointer(NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
	NumFloats += 3;
}

void GeomipGrid::PopulateBuffers(const BaseTerrain* pTerrain)
{
	// Vertices
	std::vector<Vertex> Vertices;
	Vertices.resize(m_width * m_depth);
	InitVertices(pTerrain, Vertices);

	// Indices
	int NumIndices = CalcNumIndices(); // Will be slightly larger than what the actual size will be. Only used for the vector size.
	Indices.resize(NumIndices);
	NumIndices = InitIndices(Indices); // The actual size calculated dynamically.
	printf("Final number of indices: %d\n", NumIndices);

	CalcNormals(Vertices, Indices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

int GeomipGrid::CalcNumIndices() {
	int NumQuads = (m_patchSize - 1) * (m_patchSize - 1);
	int NumIndices = 0;
	int MaxPermuationsPerLevel = 16; // true/false for each of the four sides.
	const int IndicesPerQuad = 6; // Two triangles forms a quad 2*3
	for (int lod = 0; lod <= m_maxLOD; lod++) {
		printf("LOD %d: num quads %d\n", lod, NumQuads);
		NumIndices += NumQuads * IndicesPerQuad * MaxPermuationsPerLevel;
		NumQuads /= 4;
	}
	printf("Initial number of indices %d\n", NumIndices);
	return NumIndices;
}

void GeomipGrid::InitVertices(const BaseTerrain* pTerrain, std::vector<Vertex>& Vertices)
{
	// Start timing
#if TIMING_ENABLED
	auto start = std::chrono::high_resolution_clock::now();
#endif
#pragma omp parallel for default(none) shared(pTerrain, Vertices)
	for (int z = 0; z < m_depth; z++) {
		for (int x = 0; x < m_width; x++) {
			int Index = z * m_width + x;
			assert(Index < Vertices.size());
			Vertices[Index].InitVertex(pTerrain, x, z);
		}
	}
	// End timing and output
#if TIMING_ENABLED
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Time taken for initializing vertices in Geomip Grid: " << duration.count() << " seconds" << std::endl;
#endif
}

void GeomipGrid::Vertex::InitVertex(const BaseTerrain* pTerrain, int x, int z)
{
	float y = pTerrain->GetHeight(x, z);

	float WorldScale = pTerrain->GetWorldScale();
	Pos = vec3(x * WorldScale, y, z * WorldScale);

	float Size = (float)pTerrain->GetSize();
	float TextureScale = pTerrain->GetTextureScale();
	Tex = vec2(TextureScale * (float)x / Size, TextureScale * (float)z / Size);
}

int GeomipGrid::InitIndices(std::vector<uint>& Indices)
{
	int Index = 0;

	for (int lod = 0; lod <= m_maxLOD; lod++) {
		printf("*** Init indices lod %d ***\n", lod);
		Index = InitIndicesLOD(Index, Indices, lod);
		printf("\n");
	}

	return Index;
}

int GeomipGrid::InitIndicesLOD(int Index, std::vector<uint>& Indices, int lod)
{
	int TotalIndicesForLOD = 0;

	for (int l = 0; l < LEFT; l++) {
		for (int r = 0; r < RIGHT; r++) {
			for (int t = 0; t < TOP; t++) {
				for (int b = 0; b < BOTTOM; b++) {
					m_lodInfo[lod].info[l][r][t][b].Start = Index;
					Index = InitIndicesLODSingle(Index, Indices, lod, lod + l, lod + r, lod + t, lod + b);

					m_lodInfo[lod].info[l][r][t][b].Count = Index - m_lodInfo[lod].info[l][r][t][b].Start;
					TotalIndicesForLOD += m_lodInfo[lod].info[l][r][t][b].Count;
				}
			}
		}
	}

	printf("Total indices for LOD: %d\n", TotalIndicesForLOD);

	return Index;
}

int GeomipGrid::InitIndicesLODSingle(int Index, std::vector<uint>& Indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom)
{
	int FanStep = pow(2, lodCore + 1);   // lod = 0 --> 2, lod = 1 --> 4, lod = 2 --> 8, etc
	int EndPos = m_patchSize - 1 - FanStep;  // patch size 5, fan step 2 --> EndPos = 2; patch size 9, fan step 2 --> EndPos = 6

	for (int z = 0; z <= EndPos; z += FanStep) {
		for (int x = 0; x <= EndPos; x += FanStep) {
			int lLeft = x == 0 ? lodLeft : lodCore;
			int lRight = x == EndPos ? lodRight : lodCore;
			int lBottom = z == 0 ? lodBottom : lodCore;
			int lTop = z == EndPos ? lodTop : lodCore;

			Index = CreateTriangleFan(Index, Indices, lodCore, lLeft, lRight, lTop, lBottom, x, z);
		}
	}

	return Index;
}


uint GeomipGrid::CreateTriangleFan(int Index, std::vector<uint>& Indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z)
{
	int StepLeft = pow(2, lodLeft); // because LOD starts at zero...
	int StepRight = pow(2, lodRight);
	int StepTop = pow(2, lodTop);
	int StepBottom = pow(2, lodBottom);
	int StepCenter = pow(2, lodCore);

	uint IndexCenter = (z + StepCenter) * m_width + x + StepCenter;

	// first up
	uint IndexTemp1 = z * m_width + x;
	uint IndexTemp2 = (z + StepLeft) * m_width + x;

	Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

	// second up
	if (lodLeft == lodCore) {
		IndexTemp1 = IndexTemp2;
		IndexTemp2 += StepLeft * m_width;

		Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
	}

	// first right
	IndexTemp1 = IndexTemp2;
	IndexTemp2 += StepTop;

	Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

	// second right
	if (lodTop == lodCore) {
		IndexTemp1 = IndexTemp2;
		IndexTemp2 += StepTop;

		Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
	}

	// first down
	IndexTemp1 = IndexTemp2;
	IndexTemp2 -= StepRight * m_width;

	Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

	// second down
	if (lodRight == lodCore) {
		IndexTemp1 = IndexTemp2;
		IndexTemp2 -= StepRight * m_width;

		Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
	}

	// first left
	IndexTemp1 = IndexTemp2;
	IndexTemp2 -= StepBottom;

	Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

	// second left
	if (lodBottom == lodCore) {
		IndexTemp1 = IndexTemp2;
		IndexTemp2 -= StepBottom;

		Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
	}

	return Index;
}

uint GeomipGrid::AddTriangle(uint Index, std::vector<uint>& Indices, uint v1, uint v2, uint v3)
{
	//printf("Add triangle %d %d %d\n", v1, v2, v3);
	assert(Index < Indices.size());
	Indices[Index++] = v1;
	assert(Index < Indices.size());
	Indices[Index++] = v2;
	assert(Index < Indices.size());
	Indices[Index++] = v3;

	return Index;
}



void GeomipGrid::CalcNormals(std::vector<Vertex>& Vertices, const std::vector<uint>& Indices)
{
	// Timing variables
#if TIMING_ENABLED
	auto start = std::chrono::high_resolution_clock::now(); // Start timing
#endif

	int NumIndices = m_lodInfo[0].info[0][0][0][0].Count;
#pragma omp parallel for default(none) shared(Vertices, Indices, NumIndices)
	for (int z = 0; z < m_depth - 1; z += (m_patchSize - 1)) {
		for (int x = 0; x < m_width - 1; x += (m_patchSize - 1)) {
			int BaseVertex = z * m_width + x;
			for (int i = 0; i < NumIndices; i += 3) {
				unsigned int Index0 = BaseVertex + Indices[i];
				unsigned int Index1 = BaseVertex + Indices[i + 1];
				unsigned int Index2 = BaseVertex + Indices[i + 2];
				vec3 v1 = Vertices[Index1].Pos - Vertices[Index0].Pos;
				vec3 v2 = Vertices[Index2].Pos - Vertices[Index0].Pos;
				vec3 Normal = normalize(cross(v1, v2));
				// NOT VERY CLEAN, but like 3x faster (with 12 threads)
#pragma omp atomic
				Vertices[Index0].Normal.x += Normal.x;
#pragma omp atomic
				Vertices[Index0].Normal.y += Normal.y;
#pragma omp atomic
				Vertices[Index0].Normal.z += Normal.z;
#pragma omp atomic
				Vertices[Index1].Normal.x += Normal.x;
#pragma omp atomic
				Vertices[Index1].Normal.y += Normal.y;
#pragma omp atomic
				Vertices[Index1].Normal.z += Normal.z;
#pragma omp atomic
				Vertices[Index2].Normal.x += Normal.x;
#pragma omp atomic
				Vertices[Index2].Normal.y += Normal.y;
#pragma omp atomic
				Vertices[Index2].Normal.z += Normal.z;
			}
		}
	}

	// Normalize all the vertex normals
#pragma omp parallel for default(none) shared(Vertices)
	for (int i = 0; i < Vertices.size(); i++) {
		Vertices[i].Normal = normalize(Vertices[i].Normal);
	}
	// End timing and output
#if TIMING_ENABLED
	auto end = std::chrono::high_resolution_clock::now(); // End timing
	std::chrono::duration<double> duration = end - start;
	std::cout << "Time taken for calculating normals for all vertices: " << duration.count() << " seconds" << std::endl;
#endif
}

void GeomipGrid::Render(const vec3& CameraPos)
{
	m_lodManager.Update(CameraPos);

	{
		labhelper::perf::Scope s("GeomipGrid: Render Patches");
		glBindVertexArray(m_vao);

		for (int PatchZ = 0; PatchZ < m_numPatchesZ; PatchZ++) {
			for (int PatchX = 0; PatchX < m_numPatchesX; PatchX++) {
				const LodManager::PatchLod& plod = m_lodManager.GetPatchLod(PatchX, PatchZ);
				int C = plod.Core;
				int L = plod.Left;
				int R = plod.Right;
				int T = plod.Top;
				int B = plod.Bottom;

				size_t BaseIndex = sizeof(unsigned int) * m_lodInfo[C].info[L][R][T][B].Start;

				int z = PatchZ * (m_patchSize - 1);
				int x = PatchX * (m_patchSize - 1);
				int BaseVertex = z * m_width + x;
				//            printf("%d\n", BaseVertex);

				glDrawElementsBaseVertex(GL_TRIANGLES, m_lodInfo[C].info[L][R][T][B].Count,
					GL_UNSIGNED_INT, (void*)BaseIndex, BaseVertex);
			}
		}

		glBindVertexArray(0);
	}
}