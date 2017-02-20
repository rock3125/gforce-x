#include "standardFirst.h"

#include "system/objectFactory.h"

#include "system/model/camera.h"
#include "system/model/light.h"
#include "system/model/model.h"
#include "system/model/mesh.h"
#include "system/skin/skin.h"
#include "system/skin/bone.h"
#include "system/xml/XmlParser.h"

#include "game/modelMap.h"
#include "game/ship.h"
#include "game/base.h"
#include "game/indestructableRegion.h"

#include "system/converters/vrml/vrmlParser.h"

#include "system/signals/objectLibraryObserver.h"

std::vector<WorldObject*> ObjectFactory::customObjects;
std::vector<std::string> ObjectFactory::customObjectFilenames;
std::vector<std::string> ObjectFactory::customObjectNames;

/////////////////////////////////////////////////////////////////////////////

void ObjectFactory::Initialise()
{
	WIN32_FIND_DATA findFileData;
	Log::GetLog() << "loading object-library objects" << System::CR;
	std::string path = System::GetDataDirectory()+"objectLibrary\\";
	std::string fileFilter = path + "*.wrl";
	HANDLE handle = ::FindFirstFile(fileFilter.c_str(),&findFileData);
	if (handle!=NULL)
	{
		int cntr = 0;
		do
		{
			WorldObject::WorldType type = (WorldObject::WorldType)((int)WorldObject::TYPE_CUSTOM + cntr);

			std::string partialName = System::ToLower(findFileData.cFileName);
			if (partialName.find('.')>0)
			{
				// only allow known extensions to be added for custom objects
				std::string extn = partialName.substr(partialName.find('.')+1);
				if (extn=="wrl" || extn=="xml" || extn=="bin")
				{
					// capitalize first letter
					char ch = partialName[0];
					if (ch>='a' && ch<='z')
						ch = 'A' + (ch-'a');
					partialName[0] = ch;

					// add full path and name of object to system
					std::string fname = path + partialName;
					std::string name = partialName.substr(0,partialName.find('.'));
					AddCustomObject(name,fname);
					cntr++;
				}
			}
		}
		while (::FindNextFile(handle,&findFileData)==TRUE);
	}
	Log::GetLog() << "loaded library" << System::CR << System::CR;
}

bool ObjectFactory::IsObjectInCache(WorldObject::WorldType worldType)
{
	if (worldType>=WorldObject::TYPE_CUSTOM)
	{
		int index = (int)worldType - (int)WorldObject::TYPE_CUSTOM;
		PreCond(index>=0 && index<customObjects.size());

		// lazy load if not yet loaded
		return (customObjects[index]!=NULL);
	}
	return true;
}

WorldObject* ObjectFactory::CreateObject(WorldObject::WorldType worldType)
{
	const float size = 50;
	switch (worldType)
	{
		case WorldObject::TYPE_CAMERA:
		{
			return new Camera();
		}
		case WorldObject::TYPE_LIGHT:
		{
			return new Light();
		}
		case WorldObject::TYPE_CONTAINER:
		{
			return new WorldObject();
		}
		case WorldObject::TYPE_MODEL:
		{
			return new Model();
		}
		case WorldObject::TYPE_SKIN:
		{
			return new Skin();
		}
		case WorldObject::TYPE_BONE:
		{
			return new Bone();
		}
		case WorldObject::TYPE_MODELMAP:
		{
			return new ModelMap();
		}
		case WorldObject::TYPE_SHIP:
		{
			return new Ship();
		}
		case WorldObject::TYPE_BASE:
		{
			return new Base();
		}
		case WorldObject::TYPE_INDESTRUCTABLEREGION:
		{
			return new IndestructableRegion();
		}
	}
	if (worldType>=WorldObject::TYPE_CUSTOM)
	{
		int index = (int)worldType - (int)WorldObject::TYPE_CUSTOM;
		PreCond(index>=0 && index<customObjects.size());

		// lazy load if not yet loaded
		WorldObject* obj = customObjects[index];
		if (obj==NULL)
		{
			VRMLParser v;
			std::string fname = customObjectFilenames[index];
			PreCond(!fname.empty());
			v.Load(fname);
			if (!v.HasErrors())
			{
				Model* model = dynamic_cast<Model*>(v.ParseLoadedFile());
				customObjects[index] = model;
			}
			else
			{
				throw new Exception(v.GetError());
			}
		}
		return DeepCopy(customObjects[index]);
	}
	PreCond("unknown WorldObject type id"==NULL);
	return NULL;
}

bool ObjectFactory::Isa(WorldObject* wo,WorldObject::WorldType worldType)
{
	switch (worldType)
	{
		case WorldObject::TYPE_CAMERA:
		{
			return dynamic_cast<Camera*>(wo)!=NULL;
		}
		case WorldObject::TYPE_LIGHT:
		{
			return dynamic_cast<Light*>(wo)!=NULL;
		}
		case WorldObject::TYPE_CONTAINER:
		{
			return dynamic_cast<WorldObject*>(wo)!=NULL;
		}
		case WorldObject::TYPE_MODEL:
		{
			return dynamic_cast<Model*>(wo)!=NULL;
		}
		case WorldObject::TYPE_SKIN:
		{
			return dynamic_cast<Skin*>(wo)!=NULL;
		}
		case WorldObject::TYPE_BONE:
		{
			return dynamic_cast<Bone*>(wo)!=NULL;
		}
		case WorldObject::TYPE_MODELMAP:
		{
			return dynamic_cast<ModelMap*>(wo)!=NULL;
		}
		case WorldObject::TYPE_SHIP:
		{
			return dynamic_cast<Ship*>(wo)!=NULL;
		}
		case WorldObject::TYPE_BASE:
		{
			return dynamic_cast<Base*>(wo)!=NULL;
		}
		case WorldObject::TYPE_INDESTRUCTABLEREGION:
		{
			return dynamic_cast<IndestructableRegion*>(wo)!=NULL;
		}
	}
	return false;
}

WorldObject* ObjectFactory::DeepCopy(WorldObject* wo)
{
	if (wo==NULL)
		return NULL;

	WorldObject* w = CreateObject(wo->GetWorldType());

	// cast it to the right thing
	if (Isa(w,WorldObject::TYPE_MODEL))
	{
		Model* model = dynamic_cast<Model*>(w);
		*model = *dynamic_cast<Model*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_CAMERA))
	{
		Camera* o = dynamic_cast<Camera*>(w);
		*o = *dynamic_cast<Camera*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_LIGHT))
	{
		Light* o = dynamic_cast<Light*>(w);
		*o = *dynamic_cast<Light*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_SKIN))
	{
		Skin* o = dynamic_cast<Skin*>(w);
		*o = *dynamic_cast<Skin*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_BONE))
	{
		Bone* o = dynamic_cast<Bone*>(w);
		*o = *dynamic_cast<Bone*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_MODELMAP))
	{
		ModelMap* o = dynamic_cast<ModelMap*>(w);
		*o = *dynamic_cast<ModelMap*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_SHIP))
	{
		Ship* o = dynamic_cast<Ship*>(w);
		*o = *dynamic_cast<Ship*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_BASE))
	{
		Base* o = dynamic_cast<Base*>(w);
		*o = *dynamic_cast<Base*>(wo);
	}
	else if (Isa(w,WorldObject::TYPE_INDESTRUCTABLEREGION))
	{
		IndestructableRegion* o = dynamic_cast<IndestructableRegion*>(w);
		*o = *dynamic_cast<IndestructableRegion*>(wo);
	}
	else
	{
		w->operator =(*wo);
	}

	std::vector<PRS*> children = wo->GetChildren();
	for (int i=0; i<children.size(); i++)
	{
		w->AttachChild(DeepCopy(dynamic_cast<WorldObject*>(children[i])));
	}
	return w;
}

std::string ObjectFactory::GetWorldTypeName(WorldObject::WorldType worldType)
{
	switch (worldType)
	{
		case WorldObject::TYPE_CAMERA:
		{
			return "Camera";
		}
		case WorldObject::TYPE_LIGHT:
		{
			return "Light";
		}
		case WorldObject::TYPE_CONTAINER:
		{
			return "Container";
		}
		case WorldObject::TYPE_MODEL:
		{
			return "Model";
		}
		case WorldObject::TYPE_SKIN:
		{
			return "Skin";
		}
		case WorldObject::TYPE_BONE:
		{
			return "Bone";
		}
		case WorldObject::TYPE_MODELMAP:
		{
			return "ModelMap";
		}
		case WorldObject::TYPE_SHIP:
		{
			return "Ship";
		}
		case WorldObject::TYPE_BASE:
		{
			return "Base";
		}
		case WorldObject::TYPE_INDESTRUCTABLEREGION:
		{
			return "IndestructableRegion";
		}
	}

	if (worldType>=WorldObject::TYPE_CUSTOM)
	{
		int index = (int)worldType - (int)WorldObject::TYPE_CUSTOM;
		PreCond(index>=0 && index<customObjects.size());
		WorldObject* obj = customObjects[index];
		if (obj==NULL)
		{
			return customObjectNames[index];
		}
		else
		{
			return customObjects[index]->GetName();
		}
	}
	PreCond("unknown WorldObject type id"==NULL);
	return "";
}

std::vector<WorldObject::WorldType> ObjectFactory::GetCreatableObjectList()
{
	std::vector<WorldObject::WorldType> creatableObjects;

	creatableObjects.push_back(WorldObject::TYPE_CONTAINER);
//	creatableObjects.push_back(WorldObject::TYPE_MODEL);
//	creatableObjects.push_back(WorldObject::TYPE_LIGHT);
//	creatableObjects.push_back(WorldObject::TYPE_SKIN);
	creatableObjects.push_back(WorldObject::TYPE_MODELMAP);
	creatableObjects.push_back(WorldObject::TYPE_SHIP);
	creatableObjects.push_back(WorldObject::TYPE_BASE);
	creatableObjects.push_back(WorldObject::TYPE_INDESTRUCTABLEREGION);

//	for (int i=0; i<customObjects.size(); i++)
//	{
//		creatableObjects.push_back((WorldObject::WorldType)(WorldObject::TYPE_CUSTOM+i));
//	}
	return creatableObjects;
}

void ObjectFactory::AddCustomObject(WorldObject* obj)
{
	PreCond(obj!=NULL);
	customObjects.push_back(obj);
	customObjectFilenames.push_back("");
	customObjectNames.push_back("");
	ObjectLibraryObserver::NotifyObservers(obj);
}

void ObjectFactory::AddCustomObject(const std::string& name,const std::string& fileName)
{
	customObjects.push_back(NULL);
	customObjectFilenames.push_back(fileName);
	customObjectNames.push_back(name);
	ObjectLibraryObserver::NotifyObservers(NULL);
}

void ObjectFactory::RemoveCustomObject(WorldObject::WorldType type)
{
	int index = int(type) - int(WorldObject::TYPE_CUSTOM);
	PreCond(index>=0 && index<customObjects.size());

	std::vector<WorldObject*>::iterator pos = customObjects.begin();
	while (pos != customObjects.end() && index>0)
	{
		index--;
		pos++;
	}
	PreCond(pos != customObjects.end());
	customObjects.erase(pos);
	ObjectLibraryObserver::NotifyObservers(NULL);
}

int ObjectFactory::NumCustomObjects()
{
	return customObjects.size();
}

