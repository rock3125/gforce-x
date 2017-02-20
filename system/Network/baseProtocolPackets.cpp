//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "system/network/baseProtocolPackets.h"

//////////////////////////////////////////////////////////////////////

PacketClientCanIJoin::PacketClientCanIJoin()
	: BasePacket(PID_CLIENT_CANIJOIN)
{
	version = PROTOCOL_VERSION;
}

PacketClientCanIJoin::PacketClientCanIJoin(const std::string& name)
	: BasePacket(PID_CLIENT_CANIJOIN)
{
	this->name = name;
	version = PROTOCOL_VERSION;
}

PacketClientCanIJoin* PacketClientCanIJoin::Read(byte* buffer)
{
	PacketClientCanIJoin* packet = new PacketClientCanIJoin();
	packet->CheckId(buffer);
	packet->name = packet->ReadString(buffer);
	packet->version = packet->ReadInt(buffer);
	return packet;
}

int PacketClientCanIJoin::Write(byte* buffer)
{
	WriteId(buffer);
	WriteString(buffer, name);
	WriteInt(buffer, version);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

//////////////////////////////////////////////////////////////////////

// 	PID_CLIENT_STARTED,
PacketClientStarted::PacketClientStarted()
	: BasePacket(PID_CLIENT_STARTED)
{
	playerId = 0;
}

PacketClientStarted::PacketClientStarted(byte playerId)
	: BasePacket(PID_CLIENT_STARTED)
{
	this->playerId = playerId;
}

PacketClientStarted* PacketClientStarted::Read(byte* buffer)
{
	PacketClientStarted* packet = new PacketClientStarted();
	packet->CheckId(buffer);
	packet->playerId = packet->ReadByte(buffer);
	return packet;
}

int PacketClientStarted::Write(byte* buffer)
{
	WriteId(buffer);
	WriteByte(buffer, playerId);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

//////////////////////////////////////////////////////////////////////

PacketClientIWillJoin::PacketClientIWillJoin()
	: BasePacket(PID_CLIENT_IWILLJOIN)
{
}

PacketClientIWillJoin::PacketClientIWillJoin(byte playerId, byte shipId, byte team, bool accept)
	: BasePacket(PID_CLIENT_IWILLJOIN)
{
	this->playerId = playerId;
	this->shipId = shipId;
	this->team = team;
	this->accept = accept;
}

PacketClientIWillJoin::PacketClientIWillJoin(byte playerId)
	: BasePacket(PID_CLIENT_IWILLJOIN)
{
	this->playerId = playerId;
	this->shipId = 0;
	this->team = 0;
	this->accept = true;
}

PacketClientIWillJoin* PacketClientIWillJoin::Read(byte* buffer)
{
	PacketClientIWillJoin* packet = new PacketClientIWillJoin();
	packet->CheckId(buffer);
	packet->playerId = packet->ReadByte(buffer);
	packet->shipId = packet->ReadByte(buffer);
	packet->team = packet->ReadByte(buffer);
	packet->accept = packet->ReadBool(buffer);
	return packet;
}

int PacketClientIWillJoin::Write(byte* buffer)
{
	WriteId(buffer);
	WriteByte(buffer, playerId);
	WriteByte(buffer, shipId);
	WriteByte(buffer, team);
	WriteBool(buffer, accept);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

//////////////////////////////////////////////////////////////////////

// 	PID_SERVER_YOUCAN,
PacketServerYouCan::PacketServerYouCan()
	: BasePacket(PID_SERVER_YOUCAN)
{
}

PacketServerYouCan::PacketServerYouCan(bool accept, const std::string& levelName, byte playerId, const std::string& reason)
	: BasePacket(PID_SERVER_YOUCAN)
{
	this->accept = accept;
	this->levelName = levelName;
	this->playerId = playerId;
	this->reason = reason;
}

PacketServerYouCan* PacketServerYouCan::Read(byte* buffer)
{
	PacketServerYouCan* packet = new PacketServerYouCan();
	packet->CheckId(buffer);
	packet->playerId = packet->ReadByte(buffer);
	packet->levelName = packet->ReadString(buffer);
	packet->accept = packet->ReadBool(buffer);
	packet->reason = packet->ReadString(buffer);
	return packet;
}

int PacketServerYouCan::Write(byte* buffer)
{
	WriteId(buffer);
	WriteByte(buffer, playerId);
	WriteString(buffer, levelName);
	WriteBool(buffer, accept);
	WriteString(buffer, reason);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

//////////////////////////////////////////////////////////////////////

PacketServerWelcome::PacketServerWelcome()
	: BasePacket(PID_SERVER_WELCOME)
{
	cntr = 0;
	gameType = 0;
	maxPoints = 0;
}

PacketServerWelcome::PacketServerWelcome(byte gameType, int maxPoints)
	: BasePacket(PID_SERVER_WELCOME)
{
	this->gameType = gameType;
	this->maxPoints = maxPoints;
	cntr = 0;
}

PacketServerWelcome* PacketServerWelcome::Read(byte* buffer)
{
	PacketServerWelcome* packet = new PacketServerWelcome();
	packet->CheckId(buffer);
	packet->gameType = packet->ReadByte(buffer);
	packet->maxPoints = packet->ReadInt(buffer);
	packet->cntr = packet->ReadInt(buffer);

	if (packet->cntr < 0 || packet->cntr >= MAX_NUMBER_OF_PLAYERS)
	{
		throw new Exception("min/max number of players exceeded");
	}

	// read the counter items
	for (int i = 0; i < packet->cntr; i++)
	{
		packet->playerId[i] = packet->ReadByte(buffer);
		packet->name[i] = packet->ReadString(buffer);
		packet->teamId[i] = packet->ReadByte(buffer);
		packet->shipId[i] = packet->ReadByte(buffer);
		packet->score[i] = packet->ReadInt(buffer);
	}
	return packet;
}

int PacketServerWelcome::Write(byte* buffer)
{
	WriteId(buffer);

	WriteByte(buffer, gameType);
	WriteInt(buffer, maxPoints);
	WriteInt(buffer, cntr);

	// read the counter items
	for (int i = 0; i < cntr; i++)
	{
		WriteByte(buffer, playerId[i]);
		WriteString(buffer, name[i]);
		WriteByte(buffer, teamId[i]);
		WriteByte(buffer, shipId[i]);
		WriteInt(buffer, score[i]);
	}
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}


void PacketServerWelcome::Add(byte playerId, const std::string& name, byte team, byte shipId, int score)
{
	// set vars
	this->playerId[cntr] = playerId;
	this->name[cntr] = name;
	this->teamId[cntr] = team;
	this->shipId[cntr] = shipId;
	this->score[cntr] = score;

	cntr++;
	if (cntr >= MAX_NUMBER_OF_PLAYERS)
	{
		throw new Exception("maximum number of players exceeded");
	}
}

//////////////////////////////////////////////////////////////////////

PacketServerOtherPlayerJoined::PacketServerOtherPlayerJoined()
	: BasePacket(PID_SERVER_OTHERPLAYERJOINED)
{
	name = "";
	playerId = 0;
	team = 0;
	shipId = 0;
}

PacketServerOtherPlayerJoined::PacketServerOtherPlayerJoined(byte playerId, const std::string& name, byte team, byte shipId)
	: BasePacket(PID_SERVER_OTHERPLAYERJOINED)
{
	this->name = name;
	this->playerId = playerId;
	this->team = team;
	this->shipId = shipId;
}

PacketServerOtherPlayerJoined* PacketServerOtherPlayerJoined::Read(byte* buffer)
{
	PacketServerOtherPlayerJoined* packet = new PacketServerOtherPlayerJoined();
	packet->CheckId(buffer);
	packet->playerId = packet->ReadByte(buffer);
	packet->name = packet->ReadString(buffer);
	packet->team = packet->ReadByte(buffer);
	packet->shipId = packet->ReadByte(buffer);
	return packet;
}

int PacketServerOtherPlayerJoined::Write(byte* buffer)
{
	WriteId(buffer);
	WriteByte(buffer, playerId);
	WriteString(buffer, name);
	WriteByte(buffer, team);
	WriteByte(buffer, shipId);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

//////////////////////////////////////////////////////////////////////

PacketServerOtherPlayerQuit::PacketServerOtherPlayerQuit()
	: BasePacket(PID_SERVER_OTHERPLAYERQUIT)
{
	playerId = 0;
}

PacketServerOtherPlayerQuit::PacketServerOtherPlayerQuit(byte playerId)
	: BasePacket(PID_SERVER_OTHERPLAYERQUIT)
{
	this->playerId = playerId;
}

PacketServerOtherPlayerQuit* PacketServerOtherPlayerQuit::Read(byte* buffer)
{
	PacketServerOtherPlayerQuit* packet = new PacketServerOtherPlayerQuit();
	packet->CheckId(buffer);
	packet->playerId = packet->ReadByte(buffer);
	return packet;
}

int PacketServerOtherPlayerQuit::Write(byte* buffer)
{
	WriteId(buffer);
	WriteByte(buffer, playerId);
	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

