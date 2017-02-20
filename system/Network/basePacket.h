#pragma once

#include "system/network/networking.h"

//////////////////////////////////////////////////////////////////////

// These are the common message types/packet ids between client and server
enum PacketId
{ 
	PID_INVALID_PACKET = 0,
	PID_CLIENT_CANIJOIN,
	PID_CLIENT_IWILLJOIN,
	PID_CLIENT_STARTED,

	PID_SERVER_YOUCAN,
	PID_SERVER_WELCOME,
	PID_SERVER_OTHERPLAYERJOINED,
	PID_SERVER_OTHERPLAYERQUIT,

	PID_LAST_PACKET
};

//////////////////////////////////////////////////////////////////////

class BasePacket
{
public:
	BasePacket(byte id);
	virtual ~BasePacket();
	BasePacket(const BasePacket&);
	const BasePacket& operator=(const BasePacket&);

	// write the packet to a buffer, return the number of bytes written
	virtual int Write(byte* buffer) = 0;

	// verify the id in the buffer is for this packet
	void CheckId(byte* buffer);

	// create a packet of the required type
	static BasePacket* Factory(byte* buffer);

	// write the packet id into the buffer
	void WriteId(byte* buffer);

	// after the packet has been filled, write the size to it
	void UpdateSize(byte* buffer, int size);

	// Get the id of the data packet (warning - must be valid!)
	static byte GetPacketId(byte* buffer);

	// get the size of the packet
	static int GetPacketSize(byte* buffer);

	// is this a valid packet?
	static bool IsValidPacket(byte* buffer);

	// return the size of the packet
	int PacketSize()
	{
		return writeIndex;
	}

	void WriteUint(byte* buffer, size_t s);
	void WriteInt(byte* buffer, int s);
	void WriteString(byte* buffer, const std::string& s);
	void WriteByte(byte* buffer, byte s);
	void WriteBool(byte* buffer, bool s);
	void WriteFloat(byte* buffer, float s);
	void WriteVec3(byte* buffer, const D3DXVECTOR3& s);
	void WriteQuat(byte* buffer, const D3DXQUATERNION& s);

	size_t ReadUint(byte* buffer);
	int ReadInt(byte* buffer);
	const char* ReadString(byte* buffer);
	byte ReadByte(byte* buffer);
	bool ReadBool(byte* buffer);
	float ReadFloat(byte* buffer);
	D3DXVECTOR3 ReadVec3(byte* buffer);
	D3DXQUATERNION ReadQuat(byte* buffer);

	// packet id
	byte id;
	// writing index
	int writeIndex;
	// reading index
	int readIndex;
};

