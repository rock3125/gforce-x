//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <winsock.h>

#include "system/network/NetworkId.h"

//////////////////////////////////////////////////////////////////////

NetworkId::NetworkId()
{
	ClearQueue();
}

NetworkId::~NetworkId()
{
	ClearQueue();
}

void NetworkId::ClearQueue()
{
	name = "";
	teamId = 0;
	shipId = 0;
	inUse = false;
	isActive = false;
	score = 0;
	socket = INVALID_SOCKET;
}
