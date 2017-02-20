//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "system/network/dataPacket.h"

//////////////////////////////////////////////////////////////////////

DataPosition::DataPosition(byte playerId)
	: BasePacket(DP_POSITION)
{
	this->playerId = playerId;
	Init();
}

DataPosition::DataPosition()
	: BasePacket(DP_POSITION)
{
	playerId = 0;
	Init();
}

void DataPosition::Init()
{
	hasThrust = false;
	doFire = false;
	doWaterSound = false;
	startExplosion = false;
	shield = 0.0f;
	fuel = 0.0f;
	angle = 0.0f;
	speed = 0.0f;
	baseOffset = 0.0f;
	numMissiles = 0;
	numBullets = 0;
	doFireMissile = false;
	missileTargetId = 0;
	shieldHitCount = 0;
	myScore = 0;
}

DataPosition* DataPosition::Read(byte* buffer)
{
	DataPosition* packet = new DataPosition();
	packet->CheckId(buffer);

	packet->playerId = packet->ReadByte(buffer);

	packet->pos = packet->ReadVec3(buffer);
	packet->rot = packet->ReadQuat(buffer);
	packet->angle = packet->ReadFloat(buffer);

	packet->hasThrust = packet->ReadBool(buffer);
	packet->doFire = packet->ReadBool(buffer);
	packet->doWaterSound = packet->ReadBool(buffer);
	packet->startExplosion = packet->ReadBool(buffer);
	packet->doFireMissile = packet->ReadBool(buffer);
	packet->missileTargetId = packet->ReadInt(buffer);
	packet->shieldHitCount = packet->ReadInt(buffer);

	// status display
	packet->shield = packet->ReadFloat(buffer);
	packet->fuel = packet->ReadFloat(buffer);
	packet->numBullets = packet->ReadInt(buffer);
	packet->numMissiles = packet->ReadInt(buffer);
	packet->speed = packet->ReadFloat(buffer);
	packet->baseOffset = packet->ReadFloat(buffer);
	packet->myScore = packet->ReadInt(buffer);

	return packet;
}

int DataPosition::Write(byte* buffer)
{
	WriteId(buffer);

	WriteByte(buffer, playerId);

	WriteVec3(buffer, pos);
	WriteQuat(buffer, rot);
	WriteFloat(buffer, angle);

	WriteBool(buffer, hasThrust);
	WriteBool(buffer, doFire);
	WriteBool(buffer, doWaterSound);
	WriteBool(buffer, startExplosion);
	WriteBool(buffer, doFireMissile);
	WriteInt(buffer, missileTargetId);
	WriteInt(buffer, shieldHitCount);

	WriteFloat(buffer, shield);
	WriteFloat(buffer, fuel);
	WriteInt(buffer, numBullets);
	WriteInt(buffer, numMissiles);
	WriteFloat(buffer, speed);
	WriteFloat(buffer, baseOffset);
	WriteInt(buffer, myScore);

	UpdateSize(buffer, writeIndex);
	return writeIndex;
}

