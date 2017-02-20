#pragma once

#include "system/modelImporter.h"

class Model;
class LwoPolygon;

//////////////////////////////////////////////////////////

class LwoImport : public ModelImporter
{
public:
	LwoImport();
	~LwoImport();

	// binary loader for lwo
	WorldObject* LoadBinary(std::string filename);
	// not supported - but needs to be implemented
	WorldObject* ParseLoadedFile();

private:
	// read a set of vertices
	int ReadVertices(long numberOfBytes);

	// read a set of polygons
	int ReadPolygons(long numberOfBytes);

	// read a surface description (name, colour, etc)
	void ReadSurface(long numberOfBytes);

	// read surface names
	int ReadSurfaceNames(long numberOfBytes);

private:
	FILE* fh;

	long numberOfSurfaces;

};

class LwoPolygon
{
public:
	long numberOfPolygons;
	short* polygons;
	long* faceCount;
	long* lineCount;
};
