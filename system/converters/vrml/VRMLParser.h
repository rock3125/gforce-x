#pragma once

class Model;
class WorldObject;

#include "system/modelImporter.h"

///////////////////////////////////////////////////////////////////////////////////
// vrml ->	'#' comment til eol			|
//			def
//
// defList ->	'DEF' ident 'Transform' '{'
//				'translation' f f f
//				['rotation' f f f f]
//				'children' [ shape* ] '}' defList |
//				e
//
// shape ->	['DEF' ident 'TimeSensor' '{' 'loop' 'TRUE' 'cycleInterval' f '}' ',']
//			'Shape' {
//			'appearance' ident {
//			'material' ident {
//			'diffuseColor' f f f
//          ['ambientIntensity' f]
//          ['specularColor' f f f]
//          ['shininess' f]
//          ['transparency' f]
//			} [ 'texture' 'ImageTexture' '{' 'url' "string" '}' ]
//			} geom }
//

// geom ->	'geometry' 'DEF' ident 'IndexedFaceSet' {
//			'ccw' 'TRUE'
//			'solid' 'TRUE'
//			optGeom }
//
// optGeom ->	coord optGeom			|
//				textCoord optGeom		|
//				texCoordIndex optGeom	|
//				coordIndex optGeom		|
//				normal optGeom			|
//				normalIndex optGeom		|
//				e
//
// coord -> 'coord' 'DEF' ident 'Coordinate' { 'point' [ f f f ,* ] }
//
// textCoord -> 'texCoord' 'DEF' ident 'TextureCoordinate' { 'point' [ f f,* ] }
//
// texCoordIndex -> 'texCoordIndex' '[' [i, i, i, -1,]* [i, i, i, -1] ']'
//
// coordIndex -> 'coordIndex' [ i, i, i, -1,* ]
//
// normal ->	'normal' 'Normal' '{' 'vector' '[' [f f f,]* [f f f] ']' '}'
//		        'normalPerVertex' 'TRUE'
//
// normalIndex -> 'normalIndex' '[' [i, i, i, -1,]* [i, i, i, -1] ']'

///////////////////////////////////////////////////////////////////////////////////

class VRMLParser : public ModelImporter
{
public:
	VRMLParser();
	virtual ~VRMLParser();
	const VRMLParser& operator=(const VRMLParser&);
	VRMLParser(const VRMLParser&);

	// VRML grammar - header
	WorldObject* ParseLoadedFile();

protected:
	// a vrml shape
	Model* Def();

	// a vrml shape
	Model* Shape(Model* model);

	// vrml mesh equivalent
	Mesh* Geom(Model* model);

	// vrml mesh equivalent
	Mesh* OptGeom(std::string& name,Model* model,bool& sharingVerts);

	// grammar constituents of geom
	std::vector<D3DXVECTOR3> Coordinate(std::string& name,Model* model,bool& sharingVerts);
	std::vector<D3DPINDEX> CoordIndex();
	std::vector<D3DXVECTOR2> TextureCoordinate();
	std::vector<D3DXVECTOR3> Normals();
	std::vector<D3DPINDEX> NormalIndex();
	std::vector<D3DPINDEX> TexCoordIndex();
};

