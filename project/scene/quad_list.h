#ifndef QUAD_LIST_H
#define QUAD_LIST_H

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
using namespace glm;


// this header is included by terrain.h so we have a forward 
// declaration for BaseTerrain.
class BaseTerrain;

class QuadList {
public:
	QuadList();

	~QuadList();

	void Destroy();

	void CreateQuadList(int Width, int Depth, const BaseTerrain* pTerrain);

	void Render();

private:

	struct Vertex {
		vec3 Pos;
		vec2 Tex;

		void InitVertex(const BaseTerrain* pTerrain, int width, int depth, int x, int z);
	};

	void CreateGLState();

	void PopulateBuffers(const BaseTerrain* pTerrain);
	void InitVertices(const BaseTerrain* pTerrain, std::vector<Vertex>& Vertices);
	void InitIndices(std::vector<uint>& Indices);

	int m_width = 0;
	int m_depth = 0;
	GLuint m_vao;
	GLuint m_vb;
	GLuint m_ib;
};
#endif