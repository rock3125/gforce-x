#include "standardFirst.h"

#include "system/streamers/fileStreamer.h"

char FileStreamer::strbuffer[1024];

///////////////////////////////////////////////////////////////////////////////////

FileStreamer::FileStreamer(BaseStreamer::BaseStreamerType _type)
	: fh(NULL)
{
	type=_type;
	size=0;
}

FileStreamer::~FileStreamer()
{
	Close();
}

BaseStreamer& FileStreamer::NewChild(const std::string& tag,int version)
{
	Write("tag",tag);
	Write("version",version);
	return *this;
}

BaseStreamer& FileStreamer::GetChild(const std::string& tag,int version)
{
	std::string t;
	int v;
	Read("tag",t);
	if (t!=tag)
		throw new Exception("wrong object type");

	Read("version",v);
	if (v>version)
		throw new Exception("object file too new");

	return *this;
}

BaseStreamer& FileStreamer::GetChild(const std::string& tag)
{
	std::string t;
	int v;
	Read("tag",t);
	if (t!=tag)
		throw new Exception("wrong object type");

	// don't care
	Read("version",v);

	return *this;
}

void FileStreamer::WriteNode(const std::string& name,int version)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_WRITE);

	Write("name",name);
	Write("version",version);
}

int FileStreamer::ReadNode(const std::string& name,int version)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_READ);

	std::string signature;
	int ver;

	Read("name",signature);
	if (signature!=name)
	{
		throw new Exception("signature mismatch");
	}

	Read("version",ver);
	if (ver>version)
	{
		throw new Exception("version too new");
	}

	return version;
}

void FileStreamer::WriteData(const std::string& tag,const void* buf,int bytes)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_WRITE);
	fwrite(buf,bytes,1,fh);
	size+=bytes;
}

void FileStreamer::WriteData(const std::string& str)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_WRITE);
	fwrite(str.c_str(),str.length(),1,fh);
	size+=str.length();
}

bool FileStreamer::ReadData(const std::string& tag,void* buf,int bytes)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_READ);
	return (fread(buf,bytes,1,fh)==1);
}

bool FileStreamer::Open(const std::string& _filename)
{
	Close();

	size=0;
	errStr.clear();

	filename=_filename;

	std::string mode;
	if (type==BaseStreamer::STREAM_READ)
	{
		mode="rb";
	}
	else
	{
		mode="wb";
	}

	fopen_s(&fh, filename.c_str(),mode.c_str());
	if (fh==NULL)
	{
		if (type==BaseStreamer::STREAM_READ)
		{
			errStr="could not open "+filename+" for reading";
		}
		else
		{
			errStr="could not open "+filename+" for writing";
		}
		return false;
	}

	// get file size
	if (type==BaseStreamer::STREAM_READ)
	{
		fseek(fh,0,SEEK_END);
		size=(int)ftell(fh);
		fseek(fh,0,SEEK_SET);
	}

	return true;
}

long FileStreamer::GetCurrentPosition() const
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_READ);
	return ftell(fh);
}

void FileStreamer::SetCurrentPosition(long position)
{
	PreCond(fh!=NULL);
	PreCond(type==BaseStreamer::STREAM_READ);
	fseek(fh,position,SEEK_SET);
}

void FileStreamer::Close()
{
	if (fh!=NULL)
	{
		fclose(fh);
		fh=NULL;
	}
}

int FileStreamer::Size()
{
	return size;
}

const std::string& FileStreamer::Error()
{
	return errStr;
}

void FileStreamer::Read(const std::string& tag,bool& data)
{
	ReadData(tag,&data,sizeof(bool));
}

void FileStreamer::Read(const std::string& tag,int& data)
{
	ReadData(tag,&data,sizeof(int));
}

void FileStreamer::Read(const std::string& tag,float& data)
{
	ReadData(tag,&data,sizeof(float));
}

void FileStreamer::Read(const std::string& tag,double& data)
{
	ReadData(tag,&data,sizeof(double));
}

void FileStreamer::Read(const std::string& tag,short& data)
{
	ReadData(tag,&data,sizeof(short));
}

void FileStreamer::Read(const std::string& tag,unsigned char& data)
{
	ReadData(tag,&data,sizeof(unsigned char));
}

void FileStreamer::Read(const std::string& tag,std::string& data)
{
	int s;
	ReadData(tag,&s,sizeof(int));
	PreCond(s<MAX_STRING_SIZE);
	ReadData(tag,strbuffer,s);
	data=strbuffer;
}

void FileStreamer::Read(const std::string& tag,D3DXVECTOR2& data)
{
	ReadData(tag,&data,sizeof(D3DXVECTOR2));
}

void FileStreamer::Read(const std::string& tag,D3DXVECTOR3& data)
{
	ReadData(tag,&data,sizeof(D3DXVECTOR3));
}

void FileStreamer::Read(const std::string& tag,D3DXQUATERNION& data)
{
	ReadData(tag,&data,sizeof(D3DXQUATERNION));
}

void FileStreamer::Read(const std::string& tag,D3DXMATRIXA16& data)
{
	ReadData(tag,&data,sizeof(D3DXMATRIXA16));
}

void FileStreamer::Read(const std::string& tag,D3DXCOLOR& data)
{
	ReadData(tag,&data,sizeof(D3DXCOLOR));
}

void FileStreamer::Read(const std::string& tag,D3DPVERTEX& v)
{
	ReadData(tag,&v,sizeof(D3DPVERTEX));
}

void FileStreamer::Read(const std::string& tag,D3DPINDEX& i)
{
	ReadData(tag,&i,sizeof(D3DPINDEX));
}

void FileStreamer::Write(const std::string& tag,bool data)
{
	WriteData(tag,&data,sizeof(bool));
}

void FileStreamer::Write(const std::string& tag,int data)
{
	WriteData(tag,&data,sizeof(int));
}

void FileStreamer::Write(const std::string& tag,float data)
{
	WriteData(tag,&data,sizeof(float));
}

void FileStreamer::Write(const std::string& tag,double data)
{
	WriteData(tag,&data,sizeof(double));
}

void FileStreamer::Write(const std::string& tag,short data)
{
	WriteData(tag,&data,sizeof(short));
}

void FileStreamer::Write(const std::string& tag,unsigned char data)
{
	WriteData(tag,&data,sizeof(unsigned char));
}

void FileStreamer::Write(const std::string& tag,const std::string& data)
{
	int s=data.size()+1;
	WriteData(tag,&s,sizeof(int));
	WriteData(tag,data.c_str(),s);
}

void FileStreamer::Write(const std::string& tag,const D3DXVECTOR2& data)
{
	WriteData(tag,&data,sizeof(D3DXVECTOR2));
}

void FileStreamer::Write(const std::string& tag,const D3DXVECTOR3& data)
{
	WriteData(tag,&data,sizeof(D3DXVECTOR3));
}

void FileStreamer::Write(const std::string& tag,const D3DXQUATERNION& data)
{
	WriteData(tag,&data,sizeof(D3DXQUATERNION));
}

void FileStreamer::Write(const std::string& tag,const D3DXMATRIXA16& data)
{
	WriteData(tag,&data,sizeof(D3DXMATRIXA16));
}

void FileStreamer::Write(const std::string& tag,const D3DXCOLOR& data)
{
	WriteData(tag,&data,sizeof(D3DXCOLOR));
}

void FileStreamer::Write(const std::string& tag,const D3DPVERTEX& v)
{
	WriteData(tag,&v,sizeof(D3DPVERTEX));
}

void FileStreamer::Write(const std::string& tag,const D3DPINDEX& i)
{
	WriteData(tag,&i,sizeof(D3DPINDEX));
}

