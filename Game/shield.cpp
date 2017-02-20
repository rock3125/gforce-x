//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "system/model/model.h"

#include "game/shield.h"
#include "game/commonModels.h"

///////////////////////////////////////////////////////////

Shield::Shield()
{
	xa = 0;
	ya = 0;
	za = 0;
}

Shield::~Shield()
{
}

void Shield::EventLogic(double time)
{
	xa += 0.5f;
	ya += 33;
}

void Shield::Draw(double time)
{
	Model* model = CommonModels::Get()->GetShield();
	if (model != NULL)
	{
		model->SetPosition(GetPosition());
		model->SetRotationEuler(D3DXVECTOR3(xa,ya,za));
		model->Draw(time);
	}
}

