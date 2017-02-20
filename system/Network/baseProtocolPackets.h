#pragma once

#include "system/network/basePacket.h"

//////////////////////////////////////////////////////////////////////
// a list of client server specific protocol packets
// used for joining clients to servers

// PID_CLIENT_CANIJOIN
class PacketClientCanIJoin : public BasePacket
{
	PacketClientCanIJoin(const PacketClientCanIJoin&);
	const PacketClientCanIJoin& operator = (const PacketClientCanIJoin&);
public:
	PacketClientCanIJoin();
	PacketClientCanIJoin(const std::string& name);

	// read the packet from a buffer, return a new instance of this packet
	static PacketClientCanIJoin* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	// client network version
	int	version;
	// client's name
	std::string name;
};

// 	PID_CLIENT_STARTED,
class PacketClientStarted : public BasePacket
{
	PacketClientStarted(const PacketClientStarted&);
	const PacketClientStarted& operator = (const PacketClientStarted&);
public:
	PacketClientStarted();
	PacketClientStarted(byte playerId);

	// read the packet from a buffer, return a new instance of this packet
	static PacketClientStarted* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	// id of the player
	byte playerId;
};

// 	PID_CLIENT_IWILLJOIN,
class PacketClientIWillJoin : public BasePacket
{
	PacketClientIWillJoin(const PacketClientIWillJoin&);
	const PacketClientIWillJoin& operator = (const PacketClientIWillJoin&);
public:
	PacketClientIWillJoin();
	PacketClientIWillJoin(byte playerId, byte shipId, byte team, bool accept);
	PacketClientIWillJoin(byte playerId);

	// read the packet from a buffer, return a new instance of this packet
	static PacketClientIWillJoin* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	// my id
	byte playerId;
	// selected ship
	byte shipId;
	// selected team
	byte team;
	// accepted?
	bool accept;
};

// 	PID_SERVER_YOUCAN,
class PacketServerYouCan : public BasePacket
{
	PacketServerYouCan( const PacketServerYouCan& );
	const PacketServerYouCan& operator=( const PacketServerYouCan& );
public:
	PacketServerYouCan();
	PacketServerYouCan(bool accept, const std::string& levelName, byte playerId, const std::string& reason);

	// read the packet from a buffer, return a new instance of this packet
	static PacketServerYouCan* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	// your assigned id
	byte playerId;
	// the level you're joining/loading
	std::string levelName;
	// did we get accepted?
	bool accept;
	// if not - what was the reason?
	std::string	reason;
};

// 	PID_SERVER_WELCOME,
class PacketServerWelcome : public BasePacket
{
	PacketServerWelcome(const PacketServerWelcome&);
	const PacketServerWelcome& operator = (const PacketServerWelcome&);
public:
	PacketServerWelcome();
	PacketServerWelcome(byte gameType, int maxPoints);

	// read the packet from a buffer, return a new instance of this packet
	static PacketServerWelcome* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	// announce a new player(s)
	void Add(byte playerId, const std::string& name, byte team, byte shipId, int score);

	// type of game we're playing
	byte			gameType;
	int				maxPoints;

	// list of people ready to join in
	size_t			cntr;
	byte			playerId[MAX_NUMBER_OF_PLAYERS];
	std::string		name[MAX_NUMBER_OF_PLAYERS];
	byte			teamId[MAX_NUMBER_OF_PLAYERS];
	byte			shipId[MAX_NUMBER_OF_PLAYERS];
	int				score[MAX_NUMBER_OF_PLAYERS];
};

// 	PID_SERVER_OTHERPLAYERJOINED,
class PacketServerOtherPlayerJoined : public BasePacket
{
	PacketServerOtherPlayerJoined(const PacketServerOtherPlayerJoined&);
	const PacketServerOtherPlayerJoined& operator=(const PacketServerOtherPlayerJoined&);
public:
	PacketServerOtherPlayerJoined();
	PacketServerOtherPlayerJoined(byte playerId, const std::string& name, byte team, byte shipId);

	// read the packet from a buffer, return a new instance of this packet
	static PacketServerOtherPlayerJoined* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	byte				team;
	byte				playerId;
	byte				shipId;
	std::string			name;
};

// PID_SERVER_OTHERPLAYERQUIT
class PacketServerOtherPlayerQuit : public BasePacket
{
	PacketServerOtherPlayerQuit(const PacketServerOtherPlayerQuit&);
	const PacketServerOtherPlayerQuit& operator = (const PacketServerOtherPlayerQuit&);
public:
	PacketServerOtherPlayerQuit();
	PacketServerOtherPlayerQuit(byte playerId);

	// read the packet from a buffer, return a new instance of this packet
	static PacketServerOtherPlayerQuit* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	byte	playerId;
};

