#include "standardFirst.h"

#include "system/network/packetObserver.h"
#include "system/network/basePacket.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<PacketObserver*> PacketObserver::packetObservers;

void PacketObserver::NotifyObservers(BasePacket* packet)
{
	std::vector<PacketObserver*>::iterator p = packetObservers.begin();
	while (p != packetObservers.end())
	{
		(*p)->PacketReceived(packet);
		p++;
	};
};

