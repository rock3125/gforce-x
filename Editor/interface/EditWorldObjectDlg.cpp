#include "editor/stdafx.h"

#include "interface/editWorldObjectDlg.h"
#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////
// world object edit dialog

BEGIN_MESSAGE_MAP(EditWorldObject,CDialog)
	ON_WM_SIZE()
	ON_EN_KILLFOCUS(IDC_WO_NAME,OnKillFocusName)
	ON_BN_CLICKED(IDC_WO_VISIBLE,OnClickButton)
	ON_BN_CLICKED(IDC_WO_SCRIPTABLE,OnClickButton)
	ON_BN_CLICKED(IDC_WO_UPDATEBUTTON,OnUpdateClicked)
END_MESSAGE_MAP()

EditWorldObject::EditWorldObject(CWnd* pParent)
	: GenericPropertyPage(EditWorldObject::IDD,pParent)
	, obj(NULL)
	, font(NULL)
{
	initialised=false;
}

EditWorldObject::~EditWorldObject()
{
	if (font!=NULL)
	{
		font->DeleteObject();
		safe_delete(font);
	}
	obj=NULL;
}

void EditWorldObject::Activate()
{
	name.SetFocus();
};

void EditWorldObject::ObjectChanged(ObjectObserver::Operation operation,WorldObject* object)
{
	if (object==obj && IsWindowVisible())
	{
		switch (operation)
		{
			case ObjectObserver::OP_RENAMED:
			{
				name.SetWindowText(object->GetName().c_str());
				break;
			}
			default:
			{
				PreCond("change op not implemented"==NULL);
				break;
			}
		};
	};
};

void EditWorldObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditWorldObject)
	DDX_Control(pDX,IDC_WO_NAME,name);
	DDX_Control(pDX,IDC_WO_VISIBLE,bVisible);
	DDX_Control(pDX,IDC_WO_SCRIPTABLE,bScriptable);
	DDX_Control(pDX,IDC_WO_CODEEDIT,code);
	DDX_Control(pDX,IDC_WO_UPDATEBUTTON,bUpdate);
	DDX_Control(pDX,IDC_WO_STATUS,status);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();

	font = new CFont();
	font->CreateFont(14,
					0,
					0,
					0,
					FW_NORMAL,
					FALSE,
					FALSE,
					0,
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"Courier New");

	code.SetFont(font);
	SetupData();
}

void EditWorldObject::SetData(WorldObject* _obj)
{
	obj=_obj;
	SetupData();
};

void EditWorldObject::SetupData()
{
	name.EnableWindow(obj!=NULL);
	code.EnableWindow(obj!=NULL && obj->GetScriptable());
	bVisible.EnableWindow(obj!=NULL);
	bScriptable.EnableWindow(obj!=NULL);
	bUpdate.EnableWindow(obj!=NULL);

	if (obj!=NULL)
	{
		name.SetWindowText(obj->GetName().c_str());
		code.SetWindowText(obj->GetCode().c_str());

		bVisible.SetCheck(obj->GetVisible()?BST_CHECKED:BST_UNCHECKED);
		bScriptable.SetCheck(obj->GetScriptable()?BST_CHECKED:BST_UNCHECKED);
	}
};

void EditWorldObject::GetData()
{
	if (obj!=NULL)
	{
		obj->SetVisible(bVisible.GetCheck()==BST_CHECKED);
		obj->SetScriptable(bScriptable.GetCheck()==BST_CHECKED);
		code.EnableWindow(obj->GetScriptable());

		static char buf[16384];
		name.GetWindowText(buf,255);
		std::string n=buf;
		if (!n.empty() && n!=obj->GetName())
		{
			obj->SetName(n);

			// notify everyone it has changed (including myself)
			ObjectObserver::NotifyObservers(ObjectObserver::OP_RENAMED,obj);
		}
		else
		{
			name.SetWindowText(obj->GetName().c_str());
		}

		if (obj->GetScriptable())
		{
			code.GetWindowText(buf,16383);
			n=buf;
			if (!n.empty() && n!=obj->GetCode())
			{
				obj->SetCode(n);
			}
			else
			{
				code.SetWindowText(obj->GetCode().c_str());
			}
		}
	};
};

/////////////////////////////////////////////////////////////////////////////
// EditWorldObject message handlers

void EditWorldObject::OnOK()
{
};

void EditWorldObject::OnCancel()
{
	ShowWindow(SW_HIDE);
};

void EditWorldObject::OnSize(UINT,int,int)
{
	Resize();
};

void EditWorldObject::Resize()
{
	if (initialised)
	{
		CRect r,r1,r2;
		code.GetWindowRect(&r2);
		bUpdate.GetWindowRect(&r1);
		GetClientRect(&r);
		code.SetWindowPos(NULL,0,0,
						  r.Width() - (r.left+25),
						  r.Height() - 150,
						  SWP_NOZORDER|SWP_NOMOVE);
		bUpdate.SetWindowPos(NULL,
							 r.Width() - (r1.Width()+10),
							 r.Height() - 25,0,0,
						     SWP_NOZORDER|SWP_NOSIZE);

	};
};

void EditWorldObject::OnKillFocusName()
{
	GetData();
};

void EditWorldObject::OnClickButton()
{
	GetData();
};

void EditWorldObject::OnUpdateClicked()
{
	GetData();

	if (obj!=NULL)
	{
		try
		{
			obj->UpdateCode();
			status.SetWindowText("enabled");
			AfxMessageBox("Parse Successful",MB_ICONINFORMATION|MB_OK,0);
		}
		catch (Exception* e)
		{
			std::string str = e->Message();
			str = str + "\n(script disabled from running)";
			AfxMessageBox(str.c_str(),MB_ICONERROR|MB_OK,0);
			obj->SetScriptEnabled(false);

			status.SetWindowText("disabled");
		}
	}
}
