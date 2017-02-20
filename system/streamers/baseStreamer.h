#pragma once

///////////////////////////////////////////////////////////////////////////////////

class BaseStreamer
{
public:
	enum BaseStreamerType
	{
		STREAM_READ,
		STREAM_WRITE
	};

	// for xml types we need this
	virtual BaseStreamer& NewChild(const std::string& tag,int version) = 0;
	virtual BaseStreamer& GetChild(const std::string& tag,int version) = 0;
	virtual BaseStreamer& GetChild(const std::string& tag) = 0;

	virtual void WriteNode(const std::string& name,int version) = 0;
	virtual int ReadNode(const std::string& name,int version) = 0;

	virtual void WriteData(const std::string& tag,const void* buf,int bytes) = 0;
	virtual bool ReadData(const std::string& tag,void* buf,int bytes) = 0;

	virtual void Read(const std::string& tag,bool&) = 0;
	virtual void Read(const std::string& tag,int&) = 0;
	virtual void Read(const std::string& tag,float&) = 0;
	virtual void Read(const std::string& tag,double&) = 0;
	virtual void Read(const std::string& tag,short&) = 0;
	virtual void Read(const std::string& tag,unsigned char&) = 0;
	virtual void Read(const std::string& tag,std::string&) = 0;
	virtual void Read(const std::string& tag,D3DXVECTOR2&) = 0;
	virtual void Read(const std::string& tag,D3DXVECTOR3&) = 0;
	virtual void Read(const std::string& tag,D3DXQUATERNION&) = 0;
	virtual void Read(const std::string& tag,D3DXMATRIXA16&) = 0;
	virtual void Read(const std::string& tag,D3DXCOLOR&) = 0;
	virtual void Read(const std::string& tag,D3DPVERTEX&) = 0;
	virtual void Read(const std::string& tag,D3DPINDEX&) = 0;

	virtual void Write(const std::string& tag,bool) = 0;
	virtual void Write(const std::string& tag,int) = 0;
	virtual void Write(const std::string& tag,float) = 0;
	virtual void Write(const std::string& tag,double) = 0;
	virtual void Write(const std::string& tag,short) = 0;
	virtual void Write(const std::string& tag,unsigned char) = 0;
	virtual void Write(const std::string& tag,const std::string&) = 0;
	virtual void Write(const std::string& tag,const D3DXVECTOR2&) = 0;
	virtual void Write(const std::string& tag,const D3DXVECTOR3&) = 0;
	virtual void Write(const std::string& tag,const D3DXQUATERNION&) = 0;
	virtual void Write(const std::string& tag,const D3DXMATRIXA16&) = 0;
	virtual void Write(const std::string& tag,const D3DXCOLOR&) = 0;
	virtual void Write(const std::string& tag,const D3DPVERTEX&) = 0;
	virtual void Write(const std::string& tag,const D3DPINDEX&) = 0;
};

