#include "standardFirst.h"

#include "system/modelImporter.h"

#include "system/converters/x/x.h"
#include "system/converters/vrml/VRMLParser.h"
#include "system/converters/obj/objImport.h"
#include "system/converters/bvh/bvh.h"
#include "system/converters/lwo/lwoImport.h"

/////////////////////////////////////////////////////////////////////////////

WorldObject* ModelImporter::Import(const std::string& fname)
{
	std::string lfname = System::ToLower(fname);

	isBinary = false;
	ModelImporter* import = NULL;
	if ((int)lfname.find(".x") > 0)
	{
		import = new X();
	}
	else if ((int)lfname.find(".wrl") > 0)
	{
		import = new VRMLParser();
	}
	else if ((int)lfname.find(".lwo") > 0)
	{
		isBinary = true;
		import = new LwoImport();
	}
	else if ((int)lfname.find(".obj") > 0)
	{
		import = new ObjImport();
	}
	else if ((int)lfname.find(".bvh") > 0)
	{
		import = new BVH();
	}
	else
	{
		errStr = "Unknown file extension " + fname;
		error = true;
		return NULL;
	}

	// obj file
	WorldObject* obj = NULL;
	if (isBinary)
	{
		obj = import->LoadBinary(lfname);
	}
	else
	{
		import->Load(fname);
		if (import->HasErrors())
		{
			errStr = import->GetError();
			error = true;
			safe_delete(import);
			return NULL;
		}
		obj = import->ParseLoadedFile();
		if (import->HasErrors())
		{
			errStr = import->GetError();
			error = true;
			safe_delete(import);
			return NULL;
		}
	}

	// remove importer after use
	safe_delete(import);

	return obj;
}

WorldObject* ModelImporter::ParseLoadedFile()
{
	throw new Exception("can't call parser loaded file directly");
}

WorldObject* ModelImporter::LoadBinary(std::string filename)
{
	throw new Exception("can't call load binary file directly");
}

