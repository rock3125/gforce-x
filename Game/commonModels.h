//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class Model;

///////////////////////////////////////////////////////////

class CommonModels
{
	CommonModels();
	~CommonModels();
	CommonModels(const CommonModels&);
	const CommonModels& operator=(const CommonModels&);
public:
	// access common models & create first time around
	static CommonModels* Get();

	// destroy common models when finished
	static void Destroy();

	// accessors for different models
	Model* GetMissile();
	Model* GetShield();

private:
	// load all common models
	void Load();

	// load missile common
	void LoadMissile();
	// load shield common
	void LoadShield();

	// helpers

	// fix cross over uvs
	int AlignUV(D3DPVERTEX* v1, D3DPVERTEX* v2, D3DPVERTEX* v3);

	// convert vec3 to uv in spherical space
	D3DXVECTOR2 Vec3toUV(const D3DXVECTOR3& pos);

private:
	static CommonModels*	singleton;

	Model*	missileModel;
	Model*	shieldModel;
};

