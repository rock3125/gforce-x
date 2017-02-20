//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////

class NetworkPlayer
{
public:
	NetworkPlayer()
	{
	}

	virtual ~NetworkPlayer()
	{
	}

	NetworkPlayer(const NetworkPlayer& np)
	{
		operator=(np);
	}

	const NetworkPlayer& operator=(const NetworkPlayer& np)
	{
		name = np.name;
		playerId = np.playerId;
		teamId = np.teamId;
		shipId = np.shipId;
		score = np.score;
		return *this;
	}

	std::string name;
	int playerId;
	int teamId;
	int shipId;
	int score;
};

