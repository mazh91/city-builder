//////////////////////////////////////////////////////////////////////////////////////////
//	TerrainGrid.h
//	Class declaration for grid of cubes for "marching cubes" algorithm
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

// Modified by T. McInerney Sep. 2014
//////////////////////////////////////////////////////////////////////////////////////////	
#include <stdlib.h>
#include <vector>
#include <utility>
struct TerrainGridVertex
{
	VECTOR3D	position;
	VECTOR3D    normal;
	float		height;	//the height of the scalar field at this point
};

struct TerrainGridQuad
{
	// pointers to vertices of each quad
	TerrainGridVertex *vertices[4];	
};

class TerrainGrid
{
private:
	
	int maxGridSize;
	int minGridSize;
	float gridDim;

	int numVertices;
	TerrainGridVertex *vertices;

	int numQuads;
	TerrainGridQuad *quads;

	int numFacesDrawn;
	
	GLfloat mat_ambient[4];
    GLfloat mat_specular[4];
    GLfloat mat_diffuse[4];
	GLfloat mat_shininess[1];

	
private:
	bool CreateMemory();
	void FreeMemory();

public:

	typedef std::pair<int, int> TMaxGridDim;

	TerrainGrid(int maxGridSize = 40, float gridDim = 1.0f);
	
	~TerrainGrid()
	{
		FreeMemory();
	}

	TMaxGridDim GetMaxGridDimentions()
	{
		return TMaxGridDim(minGridSize, maxGridSize);
	}
	
	bool InitGrid(int gridSize, VECTOR3D origin, double gridLength, double gridWidth);
	void DrawGrid(int gridSize);
	void ComputeNormals();
	
};

