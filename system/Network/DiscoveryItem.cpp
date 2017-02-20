//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "system/network/discoveryItem.h"

//////////////////////////////////////////////////////////////////////

DiscoveryItem::DiscoveryItem()
{
	hostName = "";
	hostIP = "";
	hostPort = 0;
	numPlayers = 0;
	gameName = "";
}

DiscoveryItem::~DiscoveryItem()
{
}

DiscoveryItem::DiscoveryItem(const DiscoveryItem& item)
{
	operator=(item);
}

const DiscoveryItem& DiscoveryItem::operator=(const DiscoveryItem& item)
{
	hostName = item.hostName;
	hostIP = item.hostIP;
	hostPort = item.hostPort;
	numPlayers = item.numPlayers;
	gameName = item.gameName;
	return *this;
}

