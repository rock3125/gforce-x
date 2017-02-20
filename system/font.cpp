#include "standardFirst.h"

#include "system/font.h"
#include "d3d9/texture.h"

std::string Font::fontSignature = "PDVFNT02";
int Font::fontVersion = 2;

///////////////////////////////////////////////////////////////////////////

Font::Font(void)
	: font(NULL),
	  texture(NULL)
{
	width=0;
	height=0;
	for (int i=0; i < 256; i++)
		widths[i]=0;

	byteWidth=0;
	byteHeight=0;
}

Font::Font(const Font& f)
	: font(NULL),
	  texture(NULL)
{
	operator=(f);
}

const Font& Font::operator=(const Font& f)
{
	width = f.width;
	height = f.height;
	for (int i=0; i<256; i++)
		widths[i]=f.widths[i];

	byteWidth = f.byteWidth;
	byteHeight = f.byteHeight;

	safe_delete(font);
	texture = f.texture;

	int size = byteWidth * byteHeight;
	if (size > 0 && f.font != NULL)
	{
		font = new byte[size];
		memcpy(font,f.font,size);
		SetupD3DFont();
	}
	return *this;
}

Font::~Font(void)
{
	texture = NULL;
	safe_delete(font);
}

void Font::Write(float x, float y, const std::string& str, D3DXCOLOR colour)
{
	Device* dev = Interface::GetDevice();

	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	dev->SetRenderState(D3DRS_TEXTUREFACTOR,colour);

	dev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	dev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);

	dev->SetTexture(0,texture);
	dev->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	dev->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	dev->SetRenderState(D3DRS_ALPHAREF,254);

	dev->SetFVF(D3DPFVF4);

	float xpos = (float)(int)x;
	float ypos = (float)(int)y;
	for (int i=0; i<str.length(); i++)
	{
		int letterId = int(str[i]);
		WriteLetter(xpos, ypos, str[i]);
		xpos = xpos + (float)widths[letterId];
	}
	dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
}

void Font::WriteLetter(float x, float y, char ch)
{
	D3DPVERTEX4 vertices[4]; // define 4 vertices (a quad)
	Device* dev = Interface::GetDevice();

	if (texture!=NULL)
	{
		float left = x;
		float top = y;
		float right = left + (float)widths[ch];
		float bottom = top + (float)height;

		// convert ch to a (u1,v1) - (u2,v2) for a uv map
		int cx = ch % 16;
		int cy = ch / 16;

		float u1 = (float)cx * width;
		float v1 = (float)cy * height;
		float u2 = u1 + widths[ch] + 1;
		float v2 = v1 + height + 1;

		// fixed sizes for 256 character alphabets
		float cwdth = 1 / ((float)width * 16);
		float chght = 1 / ((float)height * 16);
		u1 = u1 * cwdth;
		v1 = v1 * chght;
		u2 = u2 * cwdth;
		v2 = v2 * chght;

		vertices[0].position = D3DXVECTOR4(left,top,0,1);
		vertices[0].texture = D3DXVECTOR2(u1,v1);

		vertices[1].position = D3DXVECTOR4(right,top,0,1);
		vertices[1].texture = D3DXVECTOR2(u2,v1);

		vertices[2].position = D3DXVECTOR4(right,bottom,0,1);
		vertices[2].texture = D3DXVECTOR2(u2,v2);

		vertices[3].position = D3DXVECTOR4(left,bottom,0,1);
		vertices[3].texture = D3DXVECTOR2(u1,v2);

		WORD indices[] = {0,1,2,2,3,0};

		dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,2,indices,D3DFMT_INDEX16,vertices,sizeof(D3DPVERTEX4));
	}
}

bool Font::Load(const std::string& _fname)
{
	FileStreamer file(BaseStreamer::STREAM_READ);

	std::string fname = System::GetDataDirectory() + _fname;
	if (!file.Open(fname))
		return false;

	char buf[256];
	file.ReadData("",buf,fontSignature.length()+1);
	if (buf!=fontSignature)
	{
		file.Close();
		return false;
	}

	int version;
	file.ReadData("",&version,sizeof(int));
	if (version > fontVersion)
	{
		file.Close();
		return false;
	}

	// max size of cell width
	file.ReadData("",&width,sizeof(int));

	// height of cell
	file.ReadData("",&height,sizeof(int));

	// write inidividual widths
	file.ReadData("",widths,sizeof(int)*256);

	// byte width and total height in rows
	file.ReadData("",&byteWidth,sizeof(int));
	file.ReadData("",&byteHeight,sizeof(int));

	safe_delete(font);

	// the map itself
	int size = byteWidth * byteHeight;
	font = new byte[size];

	file.ReadData("", font, size);
	file.Close();

	return SetupD3DFont();
}

bool Font::SetupD3DFont(void)
{
	// create a texture from this data
	Device* dev = Interface::GetDevice();
	int size = byteWidth * byteHeight;

	texture = new Texture(dev, byteWidth/4, byteHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);
	D3DLOCKED_RECT lockRect = texture->LockReadOnly();
	byte* pBytes=(byte*)lockRect.pBits;
	memcpy(pBytes, font, size);

	texture->Unlock();

	return true;
}

float Font::GetWidth(const std::string& str)
{
	float fsize = 0;
	int ssize = str.length();
	for (int i=0; i<ssize; i++)
	{
		int ch = (int)(str[i]);

		if ( ch=='\t' ) 
		{
			fsize += float(widths[32]*4);
		}
		else if (ch!=10 && ch!=13)
		{
			fsize += float(widths[ch]);
		}
	}
	return fsize;
}

float Font::GetCharWidth(char ch)
{
	if (ch==13 || ch==10)
		return 0;

	if (ch=='\t') 
	{
		return float(widths[32]*4);
	}
	else
	{
		return float(widths[int(ch)]);
	}
}

void Font::RestoreDeviceObjects()
{
	SetupD3DFont();
}

void Font::InvalidateDeviceObjects()
{
	texture->Invalidate();
}

