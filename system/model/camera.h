#pragma once

#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////

class Camera : public WorldObject
{
public:
	enum CameraType
	{
		CAM_PERSPECTIVE,
		CAM_ORTHOGRAPHIC
	};

	Camera(CameraType cameraType = CAM_PERSPECTIVE);
	virtual ~Camera();
	Camera(const Camera&);
	const Camera& operator=(const Camera&);

	// activate and set camera
	void Activate();
	void Activate(float width,float height);

	// get/set zoom
	void SetZoom(float zoom);
	float GetZoom();

	// set camera orientation
	void SetAngles(float xangle,float yangle);
	void SetPositionAndAngle(const D3DXVECTOR3& _pos,float _rotx,float _roty);

	// get/set radius
	void  SetRadius(float radius);
	float GetRadius();

	float GetInitialXAngle();
	void  SetInitialXAngle(float n);

	float GetInitialYAngle();
	void  SetInitialYAngle(float n);

	float GetInitialZoom();
	void  SetInitialZoom(float n);

	float GetViewingDistance();
	void  SetViewingDistance(float n);

	float GetViewingAngle();
	void  SetViewingAngle(float n);

	// return camera type
	CameraType GetCameraType();
	void SetCameraType(CameraType cameraType);

	// get transformations
	D3DXVECTOR3 GetTransformedPosition();
	D3DXMATRIXA16* GetProjectionMatrix();
	D3DXMATRIXA16* GetViewMatrix();
	void CalculateMatrices();
	void CalculateMatrices(float width,float height);

	// save and load camera
	void Read(XmlNode*);
	XmlNode* Write();

	// return the signature of this object
	virtual std::string Signature()
	{
		return cameraSignature;
	}

protected:
	float			zoom;
	float			initialXAngle;
	float			initialYAngle;
	float			initialZoom;
	float			viewingDistance;
	float			viewingAngle;
	float			radius;

	D3DXMATRIXA16 	projection;
	D3DXMATRIXA16 	view;

	CameraType		cameraType;

	static std::string cameraSignature;
	static int cameraVersion;
};

