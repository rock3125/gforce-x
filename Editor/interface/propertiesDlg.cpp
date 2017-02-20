#include "editor/stdafx.h"

#include "interface/propertiesDlg.h"

#include "interface/editPRSDlg.h"
#include "interface/editLightDlg.h"
#include "interface/editWorldObjectDlg.h"
#include "interface/editModelDlg.h"
#include "interface/editBoundingBoxDlg.h"
#include "interface/logDlg.h"
#include "interface/sceneGraphDlg.h"
#include "interface/editModelMapDlg.h"
#include "interface/editShipDlg.h"
#include "interface/editBaseDlg.h"
#include "interface/editAmmoDlg.h"
#include "interface/editIndestructableRegionDlg.h"
#include "interface/editTurretDlg.h"
#include "interface/editWaterDlg.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

PropertiesDlg::PropertiesDlg(CWnd* pParent)
	: CDialog(PropertiesDlg::IDD,pParent)
	, prsDlg(NULL)
	, lightDlg(NULL)
	, worldObjectDlg(NULL)
	, modelDlg(NULL)
	, aabbDlg(NULL)
	, logDlg(NULL)
	, sceneDlg(NULL)
	, modelMapDlg(NULL)
	, shipDlg(NULL)
	, baseDlg(NULL)
	, ammoDlg(NULL)
	, irDlg(NULL)
	, turretDlg(NULL)
	, waterDlg(NULL)
{
	selectedItem=NULL;
	initialised=false;
}

PropertiesDlg::~PropertiesDlg()
{
	selectedItem=NULL;

	safe_delete(prsDlg);
	safe_delete(lightDlg);
	safe_delete(worldObjectDlg);
	safe_delete(modelDlg);
	safe_delete(aabbDlg);
	safe_delete(logDlg);
	safe_delete(sceneDlg);
	safe_delete(modelMapDlg);
	safe_delete(shipDlg);
	safe_delete(baseDlg);
	safe_delete(ammoDlg);
	safe_delete(irDlg);
	safe_delete(turretDlg);
	safe_delete(waterDlg);
}

void PropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropertiesDlg)
	DDX_Control(pDX, IDC_PROPERTIES_TAB, tab);
	//}}AFX_DATA_MAP

	// create the dialogs to be used
	prsDlg=new EditPRSDlg(&tab);
	prsDlg->Create(EditPRSDlg::IDD,&tab);
	prsDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	prsDlg->ShowWindow(SW_HIDE);

	lightDlg=new EditLightDlg(&tab);
	lightDlg->Create(EditLightDlg::IDD,&tab);
	lightDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	lightDlg->ShowWindow(SW_HIDE);

	worldObjectDlg=new EditWorldObject(&tab);
	worldObjectDlg->Create(EditWorldObject::IDD,&tab);
	worldObjectDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	worldObjectDlg->ShowWindow(SW_HIDE);

	modelDlg=new EditModelDlg(&tab);
	modelDlg->Create(EditModelDlg::IDD,&tab);
	modelDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	modelDlg->ShowWindow(SW_HIDE);

	modelMapDlg = new EditModelMapDlg(&tab);
	modelMapDlg->Create(EditModelMapDlg::IDD,&tab);
	modelMapDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	modelMapDlg->ShowWindow(SW_HIDE);

	shipDlg = new EditShipDlg(&tab);
	shipDlg->Create(EditShipDlg::IDD,&tab);
	shipDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	shipDlg->ShowWindow(SW_HIDE);

	baseDlg = new EditBaseDlg(&tab);
	baseDlg->Create(EditBaseDlg::IDD,&tab);
	baseDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	baseDlg->ShowWindow(SW_HIDE);

	aabbDlg=new EditBoundingBoxDlg(&tab);
	aabbDlg->Create(EditBoundingBoxDlg::IDD,&tab);
	aabbDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	aabbDlg->ShowWindow(SW_HIDE);

	logDlg=new LogDlg(&tab);
	logDlg->Create(LogDlg::IDD,&tab);
	logDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	logDlg->ShowWindow(SW_HIDE);

	sceneDlg=new SceneGraphDlg(&tab);
	sceneDlg->Create(SceneGraphDlg::IDD,&tab);
	sceneDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	sceneDlg->ShowWindow(SW_HIDE);

	ammoDlg=new EditAmmoDlg(&tab);
	ammoDlg->Create(EditAmmoDlg::IDD,&tab);
	ammoDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	ammoDlg->ShowWindow(SW_HIDE);

	irDlg=new EditIndestructableRegionDlg(&tab);
	irDlg->Create(EditIndestructableRegionDlg::IDD,&tab);
	irDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	irDlg->ShowWindow(SW_HIDE);

	turretDlg=new EditTurretDlg(&tab);
	turretDlg->Create(EditTurretDlg::IDD,&tab);
	turretDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	turretDlg->ShowWindow(SW_HIDE);

	waterDlg = new EditWaterDlg(&tab);
	waterDlg->Create(EditWaterDlg::IDD,&tab);
	waterDlg->SetWindowPos(0,4,25,0,0,SWP_NOZORDER|SWP_NOSIZE);
	waterDlg->ShowWindow(SW_HIDE);

	initialised=true;
	Resize();
}

BEGIN_MESSAGE_MAP(PropertiesDlg,CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_PROPERTIES_TAB, OnTabSelchange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropertiesDlg message handlers

void PropertiesDlg::OnOK()
{
}

void PropertiesDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void PropertiesDlg::OnSize(UINT,int,int)
{
	Resize();
}

void PropertiesDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);

		tab.SetWindowPos(NULL,0,0,r.Width()-12,r.Height()-12,SWP_NOZORDER|SWP_NOMOVE);

		// set all tab windows' sizes
		tab.GetClientRect(&r);
		prsDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		lightDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		worldObjectDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		modelDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		aabbDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		logDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		sceneDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		modelMapDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		shipDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		baseDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		ammoDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		irDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		turretDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
		waterDlg->SetWindowPos(0,4,25,r.Width()-8,r.Height()-30,SWP_NOZORDER);
	}
}

void PropertiesDlg::OnTabSelchange(NMHDR*,LRESULT* pResult)
{
	int currentTab=tab.GetCurSel();
	for (int i=0; i<pages.size(); i++)
	{
		pages[i]->ShowWindow((currentTab==i)?SW_SHOW:SW_HIDE);
	}
	if (currentTab>=0 && currentTab<pages.size())
	{
		pages[currentTab]->Activate();
		pages[currentTab]->SetData(selectedItem);
	}
}

void PropertiesDlg::SelectionChanged(WorldObject* newSelection)
{
	int oldSelectedTab = tab.GetCurSel();
	if (oldSelectedTab>=0 && oldSelectedTab<pages.size())
	{
		pages[oldSelectedTab]->ShowWindow(SW_HIDE);
	}

	tab.DeleteAllItems();
	pages.clear();

	// start count at 0
	int cntr=0;

	// always add
	tab.InsertItem(cntr++,"Log",0);
	pages.push_back(logDlg);
	logDlg->SetData(selectedItem);

	tab.InsertItem(cntr++,"Scene",0);
	pages.push_back(sceneDlg);
	sceneDlg->SetData(selectedItem);

	selectedItem=newSelection;
	if (selectedItem!=NULL && selectedItem->GetParent()!=NULL)
	{
		// setup the tabs
		if (newSelection->Isa(Object::OT_WORLDOBJECT))
		{
			tab.InsertItem(cntr++,"World Object",0);
			pages.push_back(worldObjectDlg);
			worldObjectDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_PRS))
		{
			tab.InsertItem(cntr++,"PRS",0);
			pages.push_back(prsDlg);
			prsDlg->SetData(selectedItem);

			//tab.InsertItem(cntr++,"Bounding Box",0);
			//pages.push_back(aabbDlg);
			//aabbDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_MODEL))
		{
			tab.InsertItem(cntr++,"Model",0);
			pages.push_back(modelDlg);
			modelDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_LIGHT))
		{
			tab.InsertItem(cntr++,"Light",0);
			pages.push_back(lightDlg);
			lightDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_MODELMAP))
		{
			tab.InsertItem(cntr++,"ModelMap",0);
			pages.push_back(modelMapDlg);
			modelMapDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_SHIP))
		{
			tab.InsertItem(cntr++,"Ship",0);
			pages.push_back(shipDlg);
			shipDlg->SetData(selectedItem);

			tab.InsertItem(cntr++,"Ammo",0);
			pages.push_back(ammoDlg);
			ammoDlg->SetData(selectedItem);
		}

		if (newSelection->Isa(Object::OT_BASE))
		{
			tab.InsertItem(cntr++,"Base",0);
			pages.push_back(baseDlg);
			shipDlg->SetData(selectedItem);
		}
		if (newSelection->Isa(Object::OT_INDESTRUCTABLEREGION))
		{
			tab.InsertItem(cntr++,"Ind.Region",0);
			pages.push_back(irDlg);
			irDlg->SetData(selectedItem);
		}
		if (newSelection->Isa(Object::OT_TURRET))
		{
			tab.InsertItem(cntr++,"Turret",0);
			pages.push_back(turretDlg);
			turretDlg->SetData(selectedItem);
		}
		if (newSelection->Isa(Object::OT_WATER))
		{
			tab.InsertItem(cntr++,"Water",0);
			pages.push_back(waterDlg);
			waterDlg->SetData(selectedItem);
		}

	}

	// show first page
	if (pages.size()>0)
	{
		if (oldSelectedTab<0 || oldSelectedTab >= pages.size())
		{
			oldSelectedTab = 0;
		}
		tab.SetCurSel(oldSelectedTab);
		pages[oldSelectedTab]->ShowWindow(SW_SHOW);
		pages[oldSelectedTab]->Activate();
	}
}


