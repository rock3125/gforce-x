#pragma once

#include "system/modelImporter.h"

class Model;

/*

obj	->			'#' eol						|
				model*

model	->		['v' f f f]*
				['vt' f f]*
				['vn' f f f]*

				faces

faces	->		['mtllib' ident ['.' ident]]
				['g' ident]*

				[
					['usemtl' ident]
					['f'  i '/' i '/' i     i '/' i '/' i    i '/' i '/' i    [i '/' i '/' i]]*
				]*
				faces						|
				e

material ->		newmtl ident
				Ns f
				Ka f f f
				Kd f f f
				Ks f f f
				map_Kd ident+ ['.' ident]

*/

//////////////////////////////////////////////////////////

class ObjImport : public ModelImporter
{
public:
	ObjImport();
	~ObjImport();

	// parse an object after it has been loaded
	// by Load() in the base class
	WorldObject* ParseLoadedFile();

private:
	// parser a material (and set my local material list)
	void ParseMaterialLib();

	void ParseObj();
	void ParseModel();
	void ParseFaces();

	bool IsModel(Token token);
	bool IsFace(Token token);

	class ObjModel;
	class ObjMaterial;
	ObjModel* GetModelByName(const std::string& name);
	void GetIndices(int& i1, int& i2, int& i3);
	ObjMaterial* GetMaterial(const std::string& name);

private:
	class ObjModel
	{
	public:
		std::string				name;
		std::vector<D3DPVERTEX> vertices;
	};

	class ObjMaterial
	{
	public:
		std::string				name;
		std::string				mapName;
		D3DXCOLOR				diffuse;
	};

	std::vector<ObjModel*> models;
	std::vector<D3DXVECTOR3>	vertices;
	std::vector<D3DXVECTOR3>	normals;
	std::vector<D3DXVECTOR2>	textures;
	std::vector<ObjMaterial>	materials;
};

