#pragma once

class Model;
class WorldObject;

///////////////////////////////////////////////////////////////////////////////////

class ImportCompoundObject
{
public:
	ImportCompoundObject();
	~ImportCompoundObject();

	// load a wgck compound object
	WorldObject* Load(const std::string& fname);

private:
	// read a string from file
	std::string GetString(FileStreamer& f);

	// load binary object
	std::vector<Model*> LoadBinary(FileStreamer& f);

	struct Material
	{
		D3DCOLOR	diffuse;
		std::string	texture;
	};

	// load material
	Material LoadMaterial(FileStreamer& f);

	// get a material by index
	Material GetMaterial(int index);

	// load mesh
	std::vector<Model*> LoadMesh(FileStreamer& f);

	struct Processed
	{
		int		index;
		int		newindex;
	};

private:
	// list of materials used in the object
	std::vector<Material>	materials;

	// load path
	std::string				path;
};

