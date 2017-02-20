#pragma once

#include "system/network/networking.h"

//////////////////////////////////////////////////////////////////////

class NetworkId
{
private:
	NetworkId(const NetworkId&);
	const NetworkId& operator = (const NetworkId&);
public:
	NetworkId();
	virtual ~NetworkId();

private:
	// initialise the queue
	void ClearQueue();

public:
	std::string		name;
	byte			teamId;
	byte			shipId;
	bool			inUse;
	bool			isActive;
	int				score;
	SOCKET			socket;
};

