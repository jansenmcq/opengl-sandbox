#include "stdafx.h"
#include "objParsedData.h"


ObjVertex::ObjVertex(float nx, float ny, float nz) {
	x = nx;
	y = ny;
	z = nz;
}

ObjVertex::ObjVertex() {

}

ObjUV::ObjUV() {

}

ObjUV::ObjUV(float nu, float nv) {
	u = nu;
	v = nv;
}

ObjTriFace::ObjTriFace() {

}

ObjTriFace::ObjTriFace(int nv1, int nv2, int nv3, int nu1, int nu2, int nu3, int nn1, int nn2, int nn3) {
	v1 = nv1 - 1;
	v2 = nv2 - 1;
	v3 = nv3 - 1;
	u1 = nu1 - 1;
	u2 = nu2 - 1;
	u3 = nu3 - 1;
	n1 = nn1 - 1;
	n2 = nn2 - 1;
	n3 = nn3 - 1;
}

ObjQuadFace::ObjQuadFace() {

}

ObjQuadFace::ObjQuadFace(int nv1, int nv2, int nv3, int nv4, int nu1, int nu2, int nu3, int nu4, int nn1, int nn2, int nn3, int nn4) {
	v1 = nv1 - 1;
	v2 = nv2 - 1;
	v3 = nv3 - 1;
	v4 = nv4 - 1;
	u1 = nu1 - 1;
	u2 = nu2 - 1;
	u3 = nu3 - 1;
	u4 = nu4 - 1;
	n1 = nn1 - 1;
	n2 = nn2 - 1;
	n3 = nn3 - 1;
	n4 = nn4 - 1;
}

ObjVertices::ObjVertices() {
	vertices = vector<ObjVertex>();
}

void ObjVertices::addVertex(ObjVertex vert) {
	vertices.push_back(vert);
}

ObjVertex ObjVertices::at(int index) {
	return vertices[index];
}

ObjUVs::ObjUVs() {
	uvs = vector<ObjUV>();
}

void ObjUVs::addUV(ObjUV uv) {
	uvs.push_back(uv);
}

ObjUV ObjUVs::at(int index) {
	return uvs[index];
}

ObjVertNormals::ObjVertNormals() {
	normals = vector<ObjVertex>();
}

void ObjVertNormals::addNormal(ObjVertex norm) {
	normals.push_back(norm);
}

ObjVertex ObjVertNormals::at(int index) {
	return normals[index];
}

ObjTriFaces::ObjTriFaces() {
	triFaces = vector<ObjTriFace>();
}

void ObjTriFaces::addTFace(ObjTriFace face) {
	triFaces.push_back(face);
}

ObjTriFace ObjTriFaces::at(int index) {
	return triFaces[index];
}

int ObjTriFaces::size() {
	return triFaces.size();
}

ObjQuadFaces::ObjQuadFaces() {
	quadFaces = vector<ObjQuadFace>();
}

void ObjQuadFaces::addQFace(ObjQuadFace face) {
	quadFaces.push_back(face);
}

ObjQuadFace ObjQuadFaces::at(int index) {
	return quadFaces[index];
}

int ObjQuadFaces::size() {
	return quadFaces.size();
}

