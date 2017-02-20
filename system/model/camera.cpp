#include "standardFirst.h"

#include "system/model/camera.h"
#include "d3d9/interface.h"

std::string Camera::cameraSignature = "camera";
int Camera::cameraVersion = 1;

/////////////////////////////////////////////////////////////////////////////

Camera::Camera(CameraType _cameraType)
{
	SetType(OT_CAMERA);
	SetWorldType(TYPE_CAMERA);

	cameraType = _cameraType;

	zoom = 1;
	viewingDistance = 8000;
	viewingAngle = 60;

	initialXAngle = 0;
	initialYAngle = 0;
	initialZoom = 1;
	radius = 125;
}

Camera::~Camera()
{
}

Camera::Camera(const Camera& c)
{
	operator=(c);
}

const Camera& Camera::operator=(const Camera& c)
{
	WorldObject::operator=(c);

	zoom = c.zoom;
	radius = c.radius;
	viewingDistance = c.viewingDistance;
	viewingAngle = c.viewingAngle;
	initialXAngle = c.initialXAngle;
	initialYAngle = c.initialYAngle;
	initialZoom = c.initialZoom;

	CalculateMatrices();

	return *this;
}

void Camera::CalculateMatrices()
{
	Device* dev=Interface::GetDevice();
	CalculateMatrices((float)dev->GetWidth(), (float)dev->GetHeight());
}

void Camera::CalculateMatrices(float width, float height)
{
	float fovy = zoom;
	float aspect = width / height;

	switch (cameraType)
	{
		case CAM_PERSPECTIVE:
		{
			D3DXMatrixPerspectiveFovLH(&projection, fovy, aspect, 0.10f, viewingDistance);
			break;
		}
		case CAM_ORTHOGRAPHIC:
		{
			// make sure depth fits in 32 bits (even though its floats)
			D3DXMatrixOrthoLH(&projection, width, height, -10000, 10000);
			break;
		}
	}

	if (radius!=0.0f && cameraType!=CAM_ORTHOGRAPHIC)
	{
		D3DXMATRIXA16 trans, rot;
		D3DXMatrixTranslation(&trans, -GetWorldPosition().x, -GetWorldPosition().y, -GetWorldPosition().z);
		D3DXMatrixRotationQuaternion(&rot, &GetRotationQuat());

		// translate out by radius and then rotate
		D3DXMATRIXA16 radTrans;
		D3DXMatrixTranslation(&radTrans, 0.0f, 0.0f, radius);
		D3DXMatrixMultiply(&view, &rot, &radTrans);

		// translate to cam position
		D3DXMatrixMultiply(&view, &trans, &view);
	}
	else
	{
		D3DXMATRIXA16 trans, rot;
		D3DXMatrixTranslation(&trans, -GetWorldPosition().x, -GetWorldPosition().y, -GetWorldPosition().z);
		D3DXMatrixRotationQuaternion(&rot, &GetRotationQuat());
		D3DXMatrixMultiply(&view, &rot, &trans);
	}
}

void Camera::Activate()
{
	CalculateMatrices();

	Device* dev=Interface::GetDevice();

	dev->SetViewTransform(&view);
	dev->SetProjectionTransform(&projection);
}

void Camera::Activate(float width,float height)
{
	CalculateMatrices(width,height);

	Device* dev=Interface::GetDevice();

	dev->SetViewTransform(&view);
	dev->SetProjectionTransform(&projection);
}

Camera::CameraType Camera::GetCameraType()
{
	return cameraType;
}

void Camera::SetCameraType(CameraType _cameraType)
{
	cameraType = _cameraType;
}

void Camera::SetZoom(float _zoom)
{
	zoom = _zoom;
}

float Camera::GetZoom()
{
	return zoom;
}

void Camera::SetRadius(float _radius)
{
	radius = _radius;
}

float Camera::GetRadius()
{
	return radius;
}

void Camera::SetPositionAndAngle(const D3DXVECTOR3& _pos,float _rotx,float _roty)
{
	PRS::SetPosition(_pos);
	PRS::SetRotationEuler(D3DXVECTOR3(_rotx, _roty, 0.0f));
}

void Camera::SetAngles(float _rotx,float _roty)
{
	PRS::SetRotationEuler(D3DXVECTOR3(_rotx, _roty, 0.0f));
}

D3DXMATRIXA16* Camera::GetProjectionMatrix()
{
	return &projection;
}

D3DXMATRIXA16* Camera::GetViewMatrix()
{
	return &view;
}

D3DXVECTOR3 Camera::GetTransformedPosition()
{
	D3DXMATRIXA16 inverseView;
	D3DXMatrixInverse(&inverseView, NULL, &view);

	D3DXVECTOR3 transPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&transPos, &transPos, &inverseView);

	return transPos;
}

float Camera::GetInitialXAngle()
{
	return initialXAngle;
}

void  Camera::SetInitialXAngle(float _initialXAngle)
{
	initialXAngle = _initialXAngle;
}

float Camera::GetInitialYAngle()
{
	return initialYAngle;
}

void  Camera::SetInitialYAngle(float _initialYAngle)
{
	initialYAngle = _initialYAngle;
}

float Camera::GetInitialZoom()
{
	return initialZoom;
}

void  Camera::SetInitialZoom(float _initialZoom)
{
	initialZoom = _initialZoom;
}

float Camera::GetViewingDistance()
{
	return viewingDistance;
}

void  Camera::SetViewingDistance(float _viewingDistance)
{
	viewingDistance = _viewingDistance;
}

float Camera::GetViewingAngle()
{
	return viewingAngle;
}

void  Camera::SetViewingAngle(float _viewingAngle)
{
	viewingAngle = _viewingAngle;
}

XmlNode* Camera::Write()
{
	XmlNode* node = XmlNode::NewChild(cameraSignature, cameraVersion);

	node->Add(WorldObject::Write());

	node->Write("zoom", zoom);
	node->Write("initialXAngle", initialXAngle);
	node->Write("initialYAngle", initialYAngle);
	node->Write("initialZoom", initialZoom);
	node->Write("viewingDistance", viewingDistance);
	node->Write("viewingAngle", viewingAngle);
	node->Write("radius", radius);

	int type = (int)cameraType;
	node->Write("type",type);
	return node;
}

void Camera::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, cameraSignature, cameraVersion);

	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	node->Read("zoom",zoom);
	node->Read("initialXAngle", initialXAngle);
	node->Read("initialYAngle", initialYAngle);
	node->Read("initialZoom", initialZoom);
	node->Read("viewingDistance", viewingDistance);
	node->Read("viewingAngle", viewingAngle);
	node->Read("radius", radius);

	int type;
	node->Read("type",type);
	cameraType = (CameraType)type;

	Device* dev = Interface::GetDevice();
	if (dev!=NULL)
	{
		Activate();
	}
}

