#include "standardFirst.h"

#include "system/objectFactory.h"

#include "system/model/level.h"
#include "system/model/worldObject.h"
#include "system/model/camera.h"

#include "d3d9/interface.h"

std::string Level::levelSignature="lev";
int Level::levelVersion=1;

/////////////////////////////////////////////////////////////////////////////

Level::Level()
	: sceneRoot(NULL)
{
	// both always exist
	sceneRoot=new WorldObject();
	sceneRoot->SetType(OT_SCENEROOT);
	sceneRoot->SetName("Scene root");

	objects.push_back(sceneRoot);
	objectMap[sceneRoot->GetOid()]=sceneRoot;
}

Level::~Level()
{
	ClearLevel();
}

void Level::ClearLevel()
{
	objectMap.clear();

	// remove scene root
	if (!objects.empty())
	{
		safe_delete(objects[0]);
	}

	objects.clear();
}

Level::Level(const Level& l)
{
	operator=(l);
}

const Level& Level::operator=(const Level& l)
{
	ClearLevel();

	// copy objects
	for (int i=0; i<l.objects.size(); i++)
	{
		WorldObject::WorldType type=l.objects[i]->GetWorldType();
		WorldObject* object=ObjectFactory::CreateObject(type);
		*object=*l.objects[i];
		objects.push_back(object);
	}

	RebuildObjectMap();

	// add objects to their resepective parents
	// needs a valid object map!!!
	ResetParents();

	return *this;
}

WorldObject* Level::GetSceneRoot()
{
	return sceneRoot;
}

WorldObject* Level::GetObject(int oid)
{
	if (objectMap.find(oid)!=objectMap.end())
		return objectMap[oid];
	return NULL;
}

std::vector<WorldObject*> Level::GetObjects()
{
	return objects;
}

void Level::ResetParents()
{
	for (int i=0; i<objects.size(); i++)
	{
		WorldObject* obj = objects[i];
		if (System::ValidOid(obj->GetParentOid()))
		{
			WorldObject* parent = GetObject(obj->GetParentOid());
			PreCond(parent!=NULL);
			parent->AttachChild(obj);
		}
	}
}

void Level::AddObjectToScene(WorldObject* parent,WorldObject* obj)
{
	// detach children from parent - temporarily
	parent->AttachChild(obj);
	RecursiveAddChildrenToObjects(obj);
}

void Level::RecursiveAddChildrenToObjects(WorldObject* obj)
{
	objects.push_back(obj);
	objectMap[obj->GetOid()]=obj;

	std::vector<PRS*> children = obj->GetChildren();
	for (int i=0; i<children.size(); i++)
	{
		RecursiveAddChildrenToObjects(dynamic_cast<WorldObject*>(children[i]));
	}
}

void Level::RemoveObjectFromScene(int oid)
{
	stdext::hash_map<int, WorldObject* >::iterator p2 = objectMap.find(oid);
	if (p2 != objectMap.end())
	{
		WorldObject* obj = p2->second;
		objectMap.erase(p2);

		std::vector<WorldObject*>::iterator p1 = std::find(objects.begin(),objects.end(),obj);
		if (p1!=objects.end())
		{
			objects.erase(p1);
		}

		// remove it from its parent
		obj->GetParent()->DetachChild(obj);

		// remove all its children too
		std::vector<PRS*> children = obj->GetChildren();
		for (int i=0; i<children.size(); i++)
		{
			RemoveObjectFromScene(children[i]->GetOid());
		}
	}
}

int Level::ObjectTypeCount(WorldObject::WorldType type)
{
	int count=0;
	for (int i=0; i<objects.size(); i++)
	{
		if (ObjectFactory::Isa(objects[i],type))
			count++;
	}
	return count;
}

void Level::RebuildObjectMap()
{
	objectMap.clear();
	for (int i=0; i<objects.size(); i++)
	{
		objectMap[objects[i]->GetOid()]=objects[i];
	}
}

void Level::Write(BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(levelSignature,levelVersion);

	// save oid of scene root
	sceneRootOid=sceneRoot->GetOid();
	f.Write("sceneRootOid",sceneRootOid);

	// save all objects
	int count=objects.size();
	f.Write("objectCount",count);
	for (int i=0; i<count; i++)
	{
		int type=objects[i]->GetWorldType();

		std::string str = "obj" + System::Int2Str(i+1);
		BaseStreamer& f2 = f.NewChild(str,1);
		f2.Write("type",type);

		objects[i]->Write(f2);
	}
}

void Level::Read(BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(levelSignature,levelVersion);

	// get oid of scene root
	f.Read("sceneRootOid",sceneRootOid);

	// remove all existing objects
	// don't delete them since screen root is object[0]
	objects.clear();

	// get all objects
	int maxOid=0;
	int count;
	f.Read("objectCount",count);
	for (int i=0; i<count; i++)
	{
		std::string str = "obj" + System::Int2Str(i+1);
		BaseStreamer& f2 = f.GetChild(str,1);

		// get factory to create this object
		int t;
		f2.Read("type",t);
		WorldObject::WorldType type=WorldObject::WorldType(t);
		WorldObject* object=ObjectFactory::CreateObject(type);

		object->Read(f2);

		// keep oids clean
		if (object->GetOid()>maxOid)
			maxOid=object->GetOid();

		// any parent oids = 0 are auto-parented to the scene root

		// add object
		if (System::ValidOid(object->GetParentOid()) || object->GetOid() == sceneRootOid)
		{
			// set scene root if possible
			if (object->GetOid() == sceneRootOid)
			{
				sceneRoot->SetPosition(object->GetPosition());
				sceneRoot->SetRotationQuat(object->GetRotationQuat());
				sceneRoot->SetScale(object->GetScale());
				sceneRoot->SetOid(object->GetOid());
				objects.push_back(sceneRoot);
				safe_delete(object);
			}
			else
			{
				objects.push_back(object);
			}
		}
		else
		{
			// object not used - remove it
			Log::GetLog() << "WARNING: orphaned object, Oid " << object->GetOid() << System::CR;
			safe_delete(object);
		}
	}

	// set oid counter for next creations
	System::SetOidCounter(maxOid+1);

	RebuildObjectMap();

	// add objects to their resepective parents
	// needs a valid object map!!!
	ResetParents();

	// re-compile object scripts
	count = objects.size();
	for (int i=0; i<count; i++)
	{
		if (objects[i]->GetScriptable())
		{
			objects[i]->UpdateCode();
		}
	}

}

WorldObject* Level::Pick(Camera* camera,const D3DXVECTOR2& mousePos,bool getClosest)
{
	D3DXMATRIXA16* matProj = camera->GetProjectionMatrix();
	D3DXMATRIXA16* matView = camera->GetViewMatrix();

	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ((mousePos.x / Interface::GetDevice()->GetWidth()) - 0.5f) * 2;
	v.y = ((1 - (mousePos.y / Interface::GetDevice()->GetHeight())) - 0.5f) * 2;
	v.z =  1.0f;

//	D3DXVec3TransformCoord(&v, &v, matView);
//	D3DXVec3TransformCoord(&v, &v, matProj);

	D3DXVECTOR3 minv, maxv;
	for (int i=0; i<objects.size(); i++)
	{
		WorldObject* w = objects[i];
		BoundingBox* bb = w->GetBoundingBox();
		minv = bb->GetTransformedMin();
		maxv = bb->GetTransformedMax();

		D3DXVec3TransformCoord(&minv, &minv, matView);
		D3DXVec3TransformCoord(&minv, &minv, matProj);

		D3DXVec3TransformCoord(&maxv, &maxv, matView);
		D3DXVec3TransformCoord(&maxv, &maxv, matProj);

		if (!ObjectFactory::Isa(w, WorldObject::TYPE_MODELMAP))
		{
			if ((minv.x <= v.x && v.x <= maxv.x &&
				minv.y <= v.y && v.y <= maxv.y) ||
				(maxv.x <= v.x && v.x <= minv.x &&
				maxv.y <= v.y && v.y <= minv.y))
			{
				return w;
			}
		}
	}
	return NULL;
/*
	WorldObject* result = NULL;

	// picking system
	// collision checking for picking
	D3DXMATRIXA16* matProj = camera->GetProjectionMatrix();
	D3DXMATRIXA16* matView = camera->GetViewMatrix();

	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * mousePos.x ) / Interface::GetDevice()->GetWidth()) - 1 ) / matProj->_11;
	v.y = -( ( ( 2.0f * mousePos.y ) / Interface::GetDevice()->GetHeight() ) - 1 ) / matProj->_22;
	v.z =  1.0f;

	D3DXMATRIXA16 m;
	D3DXVECTOR3 vPickRayDir,vPickRayOrig;

	float closestFound = 0;
	for (int i=0; i<objects.size(); i++)
	{
		WorldObject* w=objects[i];
		D3DXMATRIXA16 world = w->GetWorldTransform();
		m = world * (*matView);
		D3DXMatrixInverse(&m,NULL,&m);

		// Transform the screen space pick ray into 3D space
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		vPickRayOrig.x = m._41;
		vPickRayOrig.y = m._42;
		vPickRayOrig.z = m._43;

		float distance;
		if (w->GetBoundingBox()->IntersectRay(vPickRayOrig,vPickRayDir,&distance))
		{
			if (distance < closestFound || result == NULL)
			{
				result=w;
				closestFound=distance;
			}

			if (!getClosest)
				return result;
		}
	}
	return result;
*/
}

