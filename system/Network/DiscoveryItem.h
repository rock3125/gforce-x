#pragma once

//////////////////////////////////////////////////////////////////////

class DiscoveryItem
{
public:
	DiscoveryItem();
	virtual ~DiscoveryItem();

	DiscoveryItem(const DiscoveryItem&);
	const DiscoveryItem& operator=(const DiscoveryItem&);

	std::string HostName()
	{
		return hostName;
	}

	void HostName(std::string hostName)
	{
		this->hostName = hostName;
	}

	std::string HostIP()
	{
		return hostIP;
	}

	void HostIP(std::string hostIP)
	{
		this->hostIP = hostIP;
	}

	std::string GameName()
	{
		return gameName;
	}

	void GameName(std::string gameName)
	{
		this->hostIP = gameName;
	}

	int HostPort()
	{
		return hostPort;
	}

	void HostPort(int hostPort)
	{
		this->hostPort = hostPort;
	}

	int NumPlayers()
	{
		return numPlayers;
	}

	void NumPlayers(int numPlayers)
	{
		this->numPlayers = numPlayers;
	}

private:
	std::string hostName;
	std::string hostIP;
	int			hostPort;
	int			numPlayers;
	std::string gameName;
};


