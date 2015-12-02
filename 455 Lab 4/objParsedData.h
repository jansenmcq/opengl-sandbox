#ifndef OBJ_PARSED
#define OBJ_PARSED
using namespace std;
#include <vector>

class ObjVertex {
public:
	float x, y, z; 
	ObjVertex();
	ObjVertex(float nx, float ny, float nz);
};
class ObjUV {
public:
	float u, v;
	ObjUV();
	ObjUV(float nu, float nv);
};

class ObjTriFace{
public:
	int v1, v2, v3, u1, u2, u3, n1, n2, n3;
	ObjTriFace();
	/** Given the nature of these indices in the .obj file, the constructor will automatically subtract one from each index as it adds it*/
	ObjTriFace(int nv1, int nv2, int nv3, int nu1, int nu2, int nu3, int nn1, int nn2, int nn3);

};

class ObjQuadFace{
public:
	int v1, v2, v3, v4, u1, u2, u3, u4, n1, n2, n3, n4;
	ObjQuadFace(); 
	ObjQuadFace(int nv1, int nv2, int nv3, int nv4, int nu1, int nu2, int nu3, int nu4, int nn1, int nn2, int nn3, int nn4);
};

class ObjVertices {
public:
	vector<ObjVertex> vertices;
	ObjVertices();
	void addVertex(ObjVertex vert);
	ObjVertex at(int index);
};

class ObjUVs {
public:
	vector<ObjUV> uvs;
	ObjUVs();
	void addUV(ObjUV uv);
	ObjUV at(int index);
};

class ObjVertNormals {
public:
	vector<ObjVertex> normals;
	ObjVertNormals();
	void addNormal(ObjVertex norm);
	ObjVertex at(int index);
};

class ObjTriFaces {
public:
	vector<ObjTriFace> triFaces;
	ObjTriFaces(); 
	void addTFace(ObjTriFace face);
	ObjTriFace at(int index);
	int size();
};

class ObjQuadFaces{
public:
	vector<ObjQuadFace> quadFaces;
	ObjQuadFaces();
	void addQFace(ObjQuadFace face);
	ObjQuadFace at(int index);
	int size();
};

#endif