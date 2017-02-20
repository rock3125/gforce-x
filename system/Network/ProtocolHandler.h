#pragma once

#include "system/network/networking.h"
#include "system/network/basePacket.h"
#include "system/network/baseProtocolPackets.h"

class NetworkId;
class NetworkServer;

//////////////////////////////////////////////////////////////////////

class ProtocolHandler
{
	ProtocolHandler(const ProtocolHandler&);
	const ProtocolHandler& operator = (const ProtocolHandler&);
public:
	ProtocolHandler();
	virtual ~ProtocolHandler();

	// does this packet need special dealings?
	static bool IsProtocolPacket(BasePacket* packet);

	// process a specific packet for protocol purposes
	static void ProcessPacket(NetworkServer* server, byte playerId, BasePacket* packet);
};

