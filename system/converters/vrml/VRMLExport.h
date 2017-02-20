#pragma once

class Model;

#include "system/streamers/fileStreamer.h"

/*
#VRML V2.0 utf8

# Produced by 3D Studio MAX VRML97 exporter, Version 6, Revision 0.56
# Date: Sat Feb 05 13:33:29 2005

DEF name Transform 
{
  translation -0.7172 0 -9.309
  children [
    Shape 
	{
      appearance Appearance 
	  {
        material Material 
		{
          diffuseColor 0.4627 0.3569 0.1843
          ambientIntensity 0.2988
          specularColor 0.045 0.045 0.045
          shininess 0.2875
          transparency 0
        }
      }
      geometry DEF name-FACES IndexedFaceSet 
	  {
        ccw TRUE
        solid TRUE
        coord DEF Willow-COORD Coordinate { point [
          5.747 0 1.695, 7.848 0 -2.789, 4.122 0 -6.012, -0.2029 0 -7.426,
		  0 277.1 47.51]
        }
		texCoord DEF name-TEXCOORD TextureCoordinate { point [
          0 0, 0.1 0, 0.2 0, 0.3 0, 0.4 0, 0.5 0, 0.6 0, 0.7 0]
        }
        coordIndex [
          11, 10, 0, -1, 0, 1, 11, -1, 1, 2, 11, -1, 12, 11, 2, -1]
      }
    }
  ]
}
*/

class VRMLExport
{
	VRMLExport(const VRMLExport&);
	const VRMLExport& operator=(const VRMLExport&);
public:
	VRMLExport();
	~VRMLExport();

	// export a model to file
	static void Export(FileStreamer& fs,Model* model);

private:
	static std::string ModelToString(Model* model);
	static std::string Geometry(Model* model);
	static std::string Prolog(Model* model);
	static std::string Epilog();
};

