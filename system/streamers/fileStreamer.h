#pragma once

#include "system/streamers/baseStreamer.h"

///////////////////////////////////////////////////////////////////////////////////

class FileStreamer : public BaseStreamer
{
	FileStreamer();
	FileStreamer(const FileStreamer&);
	const FileStreamer& operator=(const FileStreamer&);
public:
	enum
	{
		MAX_STRING_SIZE=1024
	};

	FileStreamer(BaseStreamer::BaseStreamerType type);
	virtual ~FileStreamer();

	// purely for ornamentation - returns this
	virtual BaseStreamer& NewChild(const std::string& tag,int version);
	virtual BaseStreamer& GetChild(const std::string& tag,int version);
	virtual BaseStreamer& GetChild(const std::string& tag);

	// version management system - throws Exception
	virtual void WriteNode(const std::string& name,int version);
	virtual int ReadNode(const std::string& name,int version);

	// read write to from file
	virtual void WriteData(const std::string& tag,const void* buf,int bytes);
	virtual void WriteData(const std::string& str);
	virtual bool ReadData(const std::string& tag,void* buf,int bytes);

	// open file
	bool Open(const std::string& filename);

	// close file
	void Close();

	// what is the size of this file
	int Size();

	// get error string
	const std::string& Error();

	virtual void Read(const std::string& tag,bool&);
	virtual void Read(const std::string& tag,int&);
	virtual void Read(const std::string& tag,float&);
	virtual void Read(const std::string& tag,double&);
	virtual void Read(const std::string& tag,short&);
	virtual void Read(const std::string& tag,unsigned char&);
	virtual void Read(const std::string& tag,std::string&);
	virtual void Read(const std::string& tag,D3DXVECTOR2&);
	virtual void Read(const std::string& tag,D3DXVECTOR3&);
	virtual void Read(const std::string& tag,D3DXQUATERNION&);
	virtual void Read(const std::string& tag,D3DXMATRIXA16&);
	virtual void Read(const std::string& tag,D3DXCOLOR&);
	virtual void Read(const std::string& tag,D3DPVERTEX&);
	virtual void Read(const std::string& tag,D3DPINDEX&);

	virtual void Write(const std::string& tag,bool);
	virtual void Write(const std::string& tag,int);
	virtual void Write(const std::string& tag,float);
	virtual void Write(const std::string& tag,double);
	virtual void Write(const std::string& tag,short);
	virtual void Write(const std::string& tag,unsigned char);
	virtual void Write(const std::string& tag,const std::string&);
	virtual void Write(const std::string& tag,const D3DXVECTOR2&);
	virtual void Write(const std::string& tag,const D3DXVECTOR3&);
	virtual void Write(const std::string& tag,const D3DXQUATERNION&);
	virtual void Write(const std::string& tag,const D3DXMATRIXA16&);
	virtual void Write(const std::string& tag,const D3DXCOLOR&);
	virtual void Write(const std::string& tag,const D3DPVERTEX&);
	virtual void Write(const std::string& tag,const D3DPINDEX&);

	// maker system - give the ability to rewind
	// to the current using two api calls
	long GetCurrentPosition() const;
	void SetCurrentPosition(long position);

private:
	FILE*							fh;
	std::string						filename;
	std::string						errStr;
	BaseStreamer::BaseStreamerType	type;
	int							size;

	// string buffer for reading strings
	static char strbuffer[MAX_STRING_SIZE];
};

