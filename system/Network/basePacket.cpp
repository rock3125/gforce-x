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
#include "system/network/dataPacket.h"

//////////////////////////////////////////////////////////////////////

BasePacket::BasePacket(byte id)
{
	this->id = id;
	// set indexes to zero
	writeIndex = 0;
	readIndex = 0;
}

BasePacket::~BasePacket()
{
}

BasePacket::BasePacket(const BasePacket& basePacket)
{
	operator=(basePacket);
}

const BasePacket& BasePacket::operator=(const BasePacket& basePacket)
{
	id = basePacket.id;
	writeIndex = basePacket.writeIndex;
	readIndex = basePacket.readIndex;
	return *this;
}

BasePacket* BasePacket::Factory(byte* buffer)
{
	if (!BasePacket::IsValidPacket(buffer))
	{
		throw new Exception("not a valid packet");
	}
	int id = BasePacket::GetPacketId(buffer);
	switch (id)
	{
	case PID_CLIENT_CANIJOIN:
		{
			return PacketClientCanIJoin::Read(buffer);
		}
	case PID_CLIENT_IWILLJOIN:
		{
			return PacketClientIWillJoin::Read(buffer);
		}
	case PID_CLIENT_STARTED:
		{
			return PacketClientStarted::Read(buffer);
		}
	case PID_SERVER_YOUCAN:
		{
			return PacketServerYouCan::Read(buffer);
		}
	case PID_SERVER_WELCOME:
		{
			return PacketServerWelcome::Read(buffer);
		}
	case PID_SERVER_OTHERPLAYERJOINED:
		{
			return PacketServerOtherPlayerJoined::Read(buffer);
		}
	case PID_SERVER_OTHERPLAYERQUIT:
		{
			return PacketServerOtherPlayerQuit::Read(buffer);
		}
	case DP_POSITION:
		{
			return DataPosition::Read(buffer);
		}
	default:
		{
			throw new Exception("unknown/unhandled packet type");
		}
	}
	return NULL;
}

void BasePacket::CheckId(byte* buffer)
{
	writeIndex = BasePacket::GetPacketSize(buffer);
	byte packetId = ReadByte(buffer);
	if (packetId != id)
	{
		throw new Exception("packet id incorrect");
	}
	// where was the writing up to (i.e. sizeof packet)
	ReadInt(buffer);
}

void BasePacket::WriteId(byte* buffer)
{
	//
	WriteByte(buffer, id);
	// write prelim size (reserve space, see UpdateSize() below)
	WriteInt(buffer, 0);
}

void BasePacket::UpdateSize(byte* buffer, int size)
{
	int* p = (int*)&buffer[sizeof(byte)];
	*p = size;
}

bool BasePacket::IsValidPacket(byte* buffer)
{
	byte* pid = (byte*)buffer;
	int* psize = (int*)&buffer[sizeof(byte)];
	if (*pid > PID_INVALID_PACKET && *pid < PACKETS_LAST_ENTRY)
	{
		if (*psize > 0 && *psize < NETWORK_MAX_BUFFER_SIZE)
		{
			return true;
		}
	}
	return false;
}

byte BasePacket::GetPacketId(byte* buffer)
{
	if (!BasePacket::IsValidPacket(buffer))
	{
		throw new Exception("this is not a valid packet");
	}
	byte* pid = (byte*)buffer;
	return *pid;
}

int BasePacket::GetPacketSize(byte* buffer)
{
	if (!BasePacket::IsValidPacket(buffer))
	{
		throw new Exception("this is not a valid packet");
	}
	byte* pid = (byte*)buffer;
	int* psize = (int*)&buffer[sizeof(byte)];
	return *psize;
}

void BasePacket::WriteUint(byte* buffer, size_t s)
{
	size_t* p = (size_t*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(size_t);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteInt(byte* buffer, int s)
{
	int* p = (int*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(int);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteString(byte* buffer, const std::string& s)
{
	if ((writeIndex + s.size() + sizeof(int)) >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
	if (s.size() > MAX_NETWORK_STRING_SIZE)
	{
		throw new Exception("string size exceeds maximum allowed network string size");
	}

	// write size first
	WriteInt(buffer, s.size());

	// then the actual string
	char* str = (char*)&buffer[writeIndex];
	strcpy_s(str, s.size() + 2, s.c_str());
	writeIndex += (s.size() + 1);
}

void BasePacket::WriteByte(byte* buffer, byte s)
{
	byte* p = (byte*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(byte);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteBool(byte* buffer, bool s)
{
	bool* p = (bool*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(bool);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteFloat(byte* buffer, float s)
{
	float* p = (float*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(float);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteVec3(byte* buffer, const D3DXVECTOR3& s)
{
	D3DXVECTOR3* p = (D3DXVECTOR3*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(D3DXVECTOR3);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

void BasePacket::WriteQuat(byte* buffer, const D3DXQUATERNION& s)
{
	D3DXQUATERNION* p = (D3DXQUATERNION*)&buffer[writeIndex];
	*p = s;
	writeIndex += sizeof(D3DXQUATERNION);
	if (writeIndex >= NETWORK_MAX_BUFFER_SIZE)
	{
		throw new Exception("packet size exceeded");
	}
}

size_t BasePacket::ReadUint(byte* buffer)
{
	size_t ret;
	size_t* p = (size_t*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(size_t);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

int BasePacket::ReadInt(byte* buffer)
{
	int ret;
	int* p = (int*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(int);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

const char* BasePacket::ReadString(byte* buffer)
{
	int strSize = ReadInt(buffer);
	if (strSize > MAX_NETWORK_STRING_SIZE)
	{
		throw new Exception("maximum network string size exceeded");
	}

	char* p = (char*)&buffer[readIndex];
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	if (strSize != strlen(p))
	{
		throw new Exception("string size discrepancy");
	}
	readIndex += (strSize + 1);
	return p;
}

byte BasePacket::ReadByte(byte* buffer)
{
	byte ret;
	byte* p = (byte*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(byte);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

bool BasePacket::ReadBool(byte* buffer)
{
	bool ret;
	bool* p = (bool*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(bool);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

float BasePacket::ReadFloat(byte* buffer)
{
	float ret;
	float* p = (float*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(float);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

D3DXVECTOR3 BasePacket::ReadVec3(byte* buffer)
{
	D3DXVECTOR3 ret;
	D3DXVECTOR3* p = (D3DXVECTOR3*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(D3DXVECTOR3);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

D3DXQUATERNION BasePacket::ReadQuat(byte* buffer)
{
	D3DXQUATERNION ret;
	D3DXQUATERNION* p = (D3DXQUATERNION*)&buffer[readIndex];
	ret = *p;
	readIndex += sizeof(D3DXQUATERNION);
	if (readIndex > writeIndex)
	{
		throw new Exception("packet read exceeds packet size");
	}
	return ret;
}

