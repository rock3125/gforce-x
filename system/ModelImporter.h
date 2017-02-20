#pragma once

#include "system/tokeniser.h"

class WorldObject;

/////////////////////////////////////////////////////////////////////////

// define an interface for all importers
class ModelImporter : public Tokeniser
{
public:
	// load a file (import it)
	WorldObject* Import(const std::string& fname);

protected:
	// parse loaded file
	virtual WorldObject* ParseLoadedFile();
	// load binary file
	virtual WorldObject* LoadBinary(std::string filename);

	bool isBinary;
};

