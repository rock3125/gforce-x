#include "standardFirst.h"

#include "system/modelImporter.h"

#include "system/converters/x/x.h"
#include "system/converters/vrml/VRMLParser.h"
#include "system/converters/obj/objImport.h"
#include "system/converters/bvh/bvh.h"

/////////////////////////////////////////////////////////////////////////////

WorldObject* ModelImporter::Import(const std::string& fname)
{
	std::string lfname = System::ToLower(fname);

	ModelImporter* import = NULL;
	if ((int)lfname.find(".x") > 0)
	{
		import = new X();
	}
	else if ((int)lfname.find(".wrl") > 0)
	{
		import = new VRMLParser();
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
	import->Load(fname);
	if (import->HasErrors())
	{
		errStr = import->GetError();
		error = true;
		safe_delete(import);
		return NULL;
	}
	WorldObject* obj = import->ParseLoadedFile();
	if (import->HasErrors())
	{
		errStr = import->GetError();
		error = true;
		safe_delete(import);
		return NULL;
	}
	safe_delete(import);
	return obj;
}

WorldObject* ModelImporter::ParseLoadedFile()
{
	errStr = "call import, not parse loaded file";
	error = true;
	return NULL;
}

