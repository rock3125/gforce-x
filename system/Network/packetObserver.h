#pragma once

/////////////////////////////////////////////////////////////////////////////

class BasePacket;

class PacketObserver
{
public:
	PacketObserver()
	{
		packetObservers.push_back(this);
	};

	virtual ~PacketObserver()
	{
		std::vector<PacketObserver*>::iterator p=std::find(packetObservers.begin(),
														   packetObservers.end(),this);
		if (p!=packetObservers.end())
		{
			packetObservers.erase(p);
		};
	};

	virtual void PacketReceived(BasePacket* packet) = 0;

	// tell everyone its happened
	static void NotifyObservers(BasePacket* packet);

private:
	// anyone who wants to know about selections inherits from
	// PacketObserver
	static std::vector<PacketObserver*> packetObservers;
};

