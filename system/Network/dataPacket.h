#pragma once

#include "system/network/basePacket.h"

//////////////////////////////////////////////////////////////////////

// These are the common message types/packet ids between client and server
enum DataPacketId
{ 
	DP_INVALID_PACKET = PID_LAST_PACKET,

	DP_POSITION,

	PACKETS_LAST_ENTRY
};

//////////////////////////////////////////////////////////////////////

class DataPosition : public BasePacket
{
	DataPosition(const DataPosition&);
	const DataPosition& operator = (const DataPosition&);
protected:
	DataPosition();
public:
	DataPosition(byte playerId);

	// read the packet from a buffer, return a new instance of this packet
	static DataPosition* Read(byte* buffer);
	// write the packet to a buffer, return the number of bytes written
	int Write(byte* buffer);

	void Init();

	// id of this player
	byte playerId;

	// the position of this player
	D3DXVECTOR3 pos;
	D3DXQUATERNION rot;
	float angle;

	// sounds and thrust
	bool hasThrust;
	bool doFire;
	bool doWaterSound;
	float speed;
	float baseOffset;
	bool doFireMissile;
	int missileTargetId;
	int shieldHitCount;

	// explosion
	bool startExplosion;

	// shields & other indicators for display
	float shield;
	float fuel;
	int numBullets;
	int numMissiles;
	int myScore;
};

