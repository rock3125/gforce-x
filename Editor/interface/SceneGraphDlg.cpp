#include "editor/stdafx.h"

#include "interface/sceneGraphDlg.h"
#include "interface/progressDlg.h"

#include "d3d9/interface.h"
#include "d3d9/texture.h"

#include "runtime/runtime.h"

#include "system/BaseApp.h"
#include "system/objectFactory.h"

#include "system/model/level.h"
#include "system/model/worldObject.h"
#include "system/model/model.h"
#include "system/model/mesh.h"
#include "system/skin/skin.h"

#include "system/signals/selectionObserver.h"
#include "system/signals/creationObserver.h"

#include "system/modelImporter.h"

#include "system/converters/vrml/VRMLExport.h"

#include "game/modelMap.h"

#pragma warning(disable:4355)

/////////////////////////////////////////////////////////////////////////////
// log dialog

BEGIN_MESSAGE_MAP(SceneGraphDlg,GenericPropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_BEGINDRAG,IDC_SCENEGRAPH_TREE,OnBeginDrag)
	ON_NOTIFY(TVN_SELCHANGED,IDC_SCENEGRAPH_TREE,OnSelectionChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


SceneGraphDlg::SceneGraphDlg(CWnd* pParent)
	: GenericPropertyPage(SceneGraphDlg::IDD,pParent)
	, tree(this)
{
	initialised=false;
	dragging=false;
	copyObj=NULL;
}

SceneGraphDlg::~SceneGraphDlg()
{
}

void SceneGraphDlg::SetData(WorldObject* obj)
{
}

void SceneGraphDlg::Activate()
{
	Invalidate();
	tree.Invalidate();
}

void SceneGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	GenericPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SceneGraphDlg)
	DDX_Control(pDX, IDC_SCENEGRAPH_TREE, tree);
	//}}AFX_DATA_MAP

//	bitmap1.LoadBitmap(IDB_DRAG1);
//	bitmap2.LoadBitmap(IDB_DRAG1MASK);
	bitmap3.LoadBitmap(IDB_FOLDER);
	imageList.Create(16,16,ILC_COLOR32,1,0);
//	imageList.Add(&bitmap1,&bitmap2);
	imageList.Add(&bitmap3,&bitmap3);
	tree.SetImageList(&imageList,LVSIL_NORMAL);

	// setup menu callback messages
	tree.AddMenuItem(SCENE_LOADNODE,"Load item...");
//	tree.AddMenuItem(SCENE_CREATEMODELMAP,"Load model map...");
//	tree.AddMenuItem(SCENE_EXPORTVRML,"Export as VRML...");
	tree.AddMenuItem(SCENE_APPLYTRANSFORMATION,"Apply transformation");
	tree.AddMenuItem(SCENE_OPTIMISE,"Optimise model");
	tree.AddMenuItem(SCENE_COPYNODE,"Copy");
	tree.AddMenuItem(SCENE_PASTENODE,"Paste");
	tree.AddMenuItem(SCENE_DELETENODE,"Delete");
	tree.AddMenuItem(SCENE_FOCUSNODE,"Focus");
	tree.AddMenuItem(SCENE_INFO,"Info...");

	// done
	initialised=true;
	RebuildTree();
	Invalidate();
}

void SceneGraphDlg::SelectionChanged(WorldObject* newSelection)
{
	HTREEITEM item = treeMap[newSelection];
	tree.SelectItem(item);
}

void SceneGraphDlg::FocusObject(WorldObject* obj)
{
}

void SceneGraphDlg::NewLevel(Level* level)
{
	if (initialised)
	{
		RebuildTree();
	}
}

void SceneGraphDlg::ObjectChanged(ObjectObserver::Operation operation,WorldObject* object)
{
	PreCond(object!=NULL);

	switch (operation)
	{
		case ObjectObserver::OP_RENAMED:
		{
			tree.SetItemText(treeMap[object],object->GetName().c_str());
			break;
		}
		default:
		{
			PreCond("change op not implemented"==NULL);
			break;
		}
	}
}

std::string SceneGraphDlg::GetObjectDescription(Model* obj)
{
	std::string str;
	str = str + "name : " + obj->GetName() + "\n";
	str = str + "oid : " + System::Int2Str(obj->GetOid()) + "\n";

	Mesh* m = obj->GetMesh();
	if (m!=NULL)
	{
		str = str + "num vertices : " + System::Int2Str(m->GetNumVertices()) + "\n";
		str = str + "num triangles : " + System::Int2Str(m->GetNumTriangles()) + "\n";
	}
	else
	{
		str = str + "num vertices : n/a\n";
		str = str + "num triangles : n/a\n";
	}

	Texture* t = obj->GetTexture();
	if (t!=NULL && !t->GetFilename().empty())
	{
		str = str + "texture : " + t->GetFilename() + "\n";
	}
	else
	{
		str = str + "texture : n/a\n";
	}
	return str;
}

void SceneGraphDlg::TreeMessage(GenericTree::TreeMessage msg,HTREEITEM item,int id)
{
	switch (msg)
	{
		case GenericTree::TMSG_TEXTCHANGED:
		{
			CString _str=tree.GetItemText(item);
			std::string str=_str;
			WorldObject* obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
			PreCond(obj!=NULL);
			obj->SetName(str);

			// notify everyone it has changed (including myself)
			ObjectObserver::NotifyObservers(ObjectObserver::OP_RENAMED,obj);
			break;
		}

		case GenericTree::TMSG_MENUCOMMAND:
		{
			switch (id)
			{
				case SCENE_FOCUSNODE:
				{
					WorldObject* obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					if (obj!=NULL)
					{
						LevelObserver::NotifyObservers(obj);
					}
					break;
				}
				case SCENE_COPYNODE:
				{
					WorldObject* obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					copyObj = obj;
					break;
				}
				case SCENE_PASTENODE:
				{
					if (copyObj != NULL)
					{
						WorldObject* obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
						if (obj!=NULL)
						{
							if (copyObj->GetOid()==obj->GetOid() || copyObj->HasChild(obj))
							{
								AfxMessageBox("you can't copy an object onto itself",MB_OK|MB_ICONERROR);
							}
							else
							{
								// copy object and 'create' it
								WorldObject* copy1 = ObjectFactory::DeepCopy(copyObj);
								CreationObserver::NotifyObservers(copy1);
							}
						}
					}
					break;
				}
				case SCENE_APPLYTRANSFORMATION:
				{
					WorldObject* _obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(_obj != NULL);
					Model* obj = dynamic_cast<Model*>(_obj);
					if (obj!=NULL)
					{
						obj->ApplyTransformation();
					}
					else
					{
						AfxMessageBox("the selected object is not a model",MB_OK|MB_ICONERROR);
					}
					break;
				}
				case SCENE_INFO:
				{
					WorldObject* _obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(_obj != NULL);
					Model* obj = dynamic_cast<Model*>(_obj);
					if (obj != NULL)
					{
						std::string str = GetObjectDescription(obj);
						MessageBox(str.c_str(),"info",MB_OK|MB_ICONINFORMATION);
					}
					else
					{
						AfxMessageBox("the selected object is not a model",MB_OK|MB_ICONERROR);
					}
					break;
				}
				case SCENE_OPTIMISE:
				{
					WorldObject* _obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(_obj != NULL);
					Model* model = dynamic_cast<Model*>(_obj);
					if (model != NULL)
					{
						std::string str = "before:\n";
						str += GetObjectDescription(model);
						model->OptimiseMeshes();
						str += "\nafter:\n";
						str += GetObjectDescription(model);
						MessageBox(str.c_str(),"info",MB_OK|MB_ICONINFORMATION);
					}
					else
					{
						AfxMessageBox("the selected object is not a model",MB_OK|MB_ICONERROR);
					}
					break;
				}
				case SCENE_EXPORTVRML:
				{
					WorldObject* _obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(_obj != NULL);
					Model* parent = dynamic_cast<Model*>(_obj);
					if (parent != NULL)
					{
						std::string fname = BaseApp::Get()->GetFilenameForSave("VRML model (.wrl)|*.wrl|all files|*.*||","wrl","");
						if (!fname.empty())
						{
							FileStreamer fs(BaseStreamer::STREAM_WRITE);
							if (fs.Open(fname))
							{
								VRMLExport::Export(fs,parent);
								fs.Close();
							}
						}
					}
					else
					{
						AfxMessageBox("the selected object is not a model",MB_OK|MB_ICONERROR);
					}
					break;
				}
				case SCENE_LOADNODE:
				{
					WorldObject* parent = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(parent!=NULL);

					std::string fname = BaseApp::Get()->GetFilenameForLoad("model files (.wrl,.x,.obj)|*.x;*.wrl;*.obj;*.bvh|all files|*.*||","wrl","");
					if (!fname.empty())
					{
						ModelImporter importer;
						std::string partial = System::RemoveDataDirectory(fname);
						ProgressDlg* dlg = ProgressDlg::Start(this,"Importing " + partial);
						WorldObject* obj = importer.Import(fname);
						dlg->End();
						if (!importer.HasErrors())
						{
							CreationObserver::NotifyObservers(obj);
						}
						else
						{
							AfxMessageBox(importer.GetError().c_str(),MB_ICONERROR|MB_OK);
						}
					}
					break;
				}
				case SCENE_DELETENODE:
				{
					copyObj = NULL;

					WorldObject* obj = reinterpret_cast<WorldObject*>(tree.GetItemData(item));
					PreCond(obj!=NULL);

					Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
					Level* level = rt->GetCurrentLevel();
					PreCond(level != NULL);
					WorldObject* root = level->GetSceneRoot();
					PreCond(root != NULL);

					if (obj->GetOid()==root->GetOid())
					{
						AfxMessageBox("you can't delete the scene root",MB_OK|MB_ICONERROR);
					}
					else
					{
						level->RemoveObjectFromScene(obj->GetOid());
						tree.DeleteItem(item);

						// remove the item from the tree-map too
						stdext::hash_map<WorldObject*, HTREEITEM >::iterator pos=treeMap.find(obj);
						if (pos!=treeMap.end())
						{
							treeMap.erase(pos);
						}

						// make it cear there are no selections at present
						HTREEITEM selItem = tree.GetSelectedItem();
						WorldObject* selObj = reinterpret_cast<WorldObject*>(tree.GetItemData(selItem));
						SelectionObserver::NotifyObservers(selObj);
					}

					break;
				}
			}
		}

	}
}

/////////////////////////////////////////////////////////////////////////////
// SceneGraphDlg message handlers

void SceneGraphDlg::OnOK()
{
}

void SceneGraphDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void SceneGraphDlg::OnSize(UINT,int,int)
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
		int border=2;
		tree.SetWindowPos(0,border,border,r.Width()-(border*2),r.Height()-(border*2),SWP_NOZORDER);
	}
}

void SceneGraphDlg::RebuildTree()
{
	if (initialised)
	{
		tree.DeleteAllItems();
		treeMap.clear();

		Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
		Level* level = rt->GetCurrentLevel();
		PreCond(level != NULL);
		WorldObject* root = level->GetSceneRoot();
		PreCond(root != NULL);

		RebuildTree(root,TVI_ROOT);
		tree.Expand(tree.GetRootItem(),TVE_EXPAND);

		// map root
		treeMap[root]=tree.GetRootItem();

		// make it cear there are no selections at present
		SelectionObserver::NotifyObservers(NULL);
	}
}

HTREEITEM SceneGraphDlg::RebuildTree(WorldObject* root,HTREEITEM parent)
{
	HTREEITEM treeroot=tree.InsertItem(root->GetName().c_str(),parent);
	tree.SetItemData(treeroot,DWORD_PTR(root));
	tree.SetItemImage(treeroot,0,0);

	// map obj to tree item
	treeMap[root]=treeroot;

	std::vector<PRS*> children=root->GetChildren();
	for (int i=0; i<children.size(); i++)
	{
		RebuildTree(dynamic_cast<WorldObject*>(children[i]),treeroot);
	}

	return treeroot;
}

void SceneGraphDlg::NotifyCreation(WorldObject* parent,WorldObject* obj)
{
	PreCond(treeMap.find(parent)!=treeMap.end());
	HTREEITEM parentTree = treeMap[parent];
	HTREEITEM newItem=RebuildTree(obj,parentTree);
	tree.SetItemData(newItem,DWORD_PTR(obj));
	tree.SetItemImage(newItem,0,0);
	tree.Expand(parentTree,TVE_EXPAND);
}

void SceneGraphDlg::OnBeginDrag(NMHDR* hdr,LRESULT* res)
{
	dragging=false;
	LPNMTREEVIEW tvi=reinterpret_cast<LPNMTREEVIEW>(hdr);
	CPoint dragPoint=tvi->ptDrag;
	dragItem=tvi->itemNew.hItem;
	if (dragItem!=NULL)
	{
		WorldObject* w=reinterpret_cast<WorldObject*>(tree.GetItemData(dragItem));
		if (w!=NULL && !w->Isa(Object::OT_SCENEROOT))
		{
			tree.SelectItem(tree.GetRootItem());
			imageList.BeginDrag(0,CPoint(0,0));
			imageList.DragEnter(&tree,dragPoint);
			SetCapture();
			dragging=true;
		}
    }
    *res=0;
}

void SceneGraphDlg::OnSelectionChanged(NMHDR*,LRESULT* res)
{
	HTREEITEM item=tree.GetSelectedItem();
	if (item!=NULL)
	{
		SelectionObserver::NotifyObservers(reinterpret_cast<WorldObject*>(tree.GetItemData(item)));
	}
	*res=0;
}

void SceneGraphDlg::OnMouseMove(UINT nFlags,CPoint point)
{
    if (dragging)
    {
        CPoint ptTree(point);
        MapWindowPoints(&tree,&ptTree,1);
        CImageList::DragMove(ptTree);
        UINT uHitTest=TVHT_ONITEM;
        dragTarget=tree.HitTest(ptTree,&uHitTest);
		if (dragTarget!=dragItem)
			tree.SelectItem(dragTarget);
    }
    GenericPropertyPage::OnMouseMove(nFlags,point);
}

void SceneGraphDlg::CopyBranch(HTREEITEM h,HTREEITEM parent)
{
	if (h==NULL)
		return;

    CString szLabel=tree.GetItemText(h);
	WorldObject* w=reinterpret_cast<WorldObject*>(tree.GetItemData(h));

	HTREEITEM newItem=tree.InsertItem(szLabel,parent);
	tree.SetItemData(newItem,DWORD_PTR(w));
	tree.SetItemImage(newItem,0,0);

	// update map
	treeMap[w]=newItem;

	// go through all its children
	HTREEITEM child=tree.GetNextItem(h,TVGN_CHILD);
	while (child!=NULL)
	{
		CopyBranch(child,newItem);
		child=tree.GetNextItem(child,TVGN_NEXT);
	}
}

void SceneGraphDlg::OnLButtonUp(UINT nFlags,CPoint point)
{
    if (dragging)
    {
        CImageList::DragLeave(&tree);
        CImageList::EndDrag();
        ReleaseCapture();
        dragging=false;
        if (dragTarget!=NULL)
        {
            // get parent info
			WorldObject* parentObj=reinterpret_cast<WorldObject*>(tree.GetItemData(dragTarget));

            CString szLabel=tree.GetItemText(dragItem);
			WorldObject* childObj=reinterpret_cast<WorldObject*>(tree.GetItemData(dragItem));

            if (parentObj!=NULL && parentObj->GetOid()!=childObj->GetOid())
			{
				CopyBranch(dragItem,dragTarget);
				tree.DeleteItem(dragItem);

				// and set the record straight for the level
				childObj->DetachFromParent();
				parentObj->AttachChild(childObj);

				// and expand the new item to show all it worked
				tree.Expand(dragTarget,TVE_EXPAND);
			}
        }
    }
    else
	{
        GenericPropertyPage::OnLButtonUp(nFlags, point);
	}
}
