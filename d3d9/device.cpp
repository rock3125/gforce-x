//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/device.h"
#include "d3d9/interface.h"
#include "d3d9/texture.h"
#include "system/model/mesh.h"

/////////////////////////////////////////////////////////////////////////////

D3DXMATRIXA16 Device::identityMatrix;

Device::Device(IDirect3DDevice9* _pd3dDevice)
	: pd3dDevice(_pd3dDevice)
	, pVertexDeclaration(NULL)
	, pColourVertexDeclaration(NULL)
	, font(NULL)
{
	deviceLost = false;

	// initial values only
	width=800;
	height=600;
	windowed=true;

	// initialize our identity matrix;
	D3DXMatrixIdentity(&identityMatrix);

	font = new Font();
}

Device::~Device()
{
	safe_delete(font);
	pd3dDevice=NULL;
}

void Device::SetDevice(IDirect3DDevice9* _pd3dDevice)
{
	pd3dDevice=_pd3dDevice;
}

IDirect3DDevice9* Device::GetDevice()
{
	return pd3dDevice;
}

Font* Device::GetFont()
{
	return font;
}

void Device::SetWidthHeightWindowed(int _width,int _height,bool _windowed)
{
	width=_width;
	height=_height;
	windowed=_windowed;
}

int Device::GetWidth() const
{
	return width;
}

int Device::GetHeight() const
{
	return height;
}

bool Device::IsWindowed() const
{
	return windowed;
}

bool Device::SetTexture(DWORD index,Texture* texture)
{
	if (texture!=NULL)
		return (SUCCEEDED(pd3dDevice->SetTexture(index,texture->GetTexture()))==TRUE);
	else
		return (SUCCEEDED(pd3dDevice->SetTexture(index,NULL))==TRUE);
}

bool Device::BeginScene()
{
   HRESULT hr;

    if (deviceLost)
    {
		pVertexDeclaration = NULL;
		pColourVertexDeclaration = NULL;

        // Test the cooperative level to see if it's okay to render
        if (FAILED(hr=pd3dDevice->TestCooperativeLevel()))
        {
			// mark the device as unavailable
			deviceLost = true;

            // If the device was lost, do not render until we get it back
            if (hr == D3DERR_DEVICELOST)
			{
                return false;
			}

            // Check if the device needs to be reset.
            if (hr==D3DERR_DEVICENOTRESET)
			{
				if (FAILED(hr=Interface::GetI()->ResetDevice()))
				{
                    return false;
				}
            }
            return false;
        }
        deviceLost = false;
    }

	// else begin the scene
	return SUCCEEDED(pd3dDevice->BeginScene())==TRUE;
}

bool Device::EndScene()
{
	return SUCCEEDED(pd3dDevice->EndScene())==TRUE;
}

bool Device::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color, float Z,DWORD Stencil)
{
	return SUCCEEDED(pd3dDevice->Clear(Count,pRects,Flags,Color,Z,Stencil))==TRUE;
}

bool Device::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE primitiveType,UINT minVertexIndex,UINT numVertices,UINT primitiveCount,const void *pIndexData,D3DFORMAT indexDataFormat,
									CONST void* pVertexStreamZeroData,UINT vertexStreamZeroStride)
{
	return SUCCEEDED(pd3dDevice->DrawIndexedPrimitiveUP(primitiveType,minVertexIndex,numVertices,primitiveCount,
					 pIndexData,indexDataFormat,pVertexStreamZeroData,vertexStreamZeroStride)) == TRUE;
}

bool Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	return SUCCEEDED(pd3dDevice->DrawPrimitiveUP(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride)==TRUE);
}

bool Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	return SUCCEEDED(pd3dDevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount))==TRUE;
}

bool Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	return SUCCEEDED(pd3dDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount))==TRUE;
}

void Device::Present()
{
    HRESULT hr=pd3dDevice->Present(NULL,NULL,NULL,NULL);
    if (hr==D3DERR_DEVICELOST)
        deviceLost=true;
}

void Device::Present(RECT* source, RECT* dest)
{
    HRESULT hr=pd3dDevice->Present(source,dest,NULL,NULL);
    if (hr==D3DERR_DEVICELOST)
        deviceLost=true;
}

void Device::SetRenderState(D3DRENDERSTATETYPE renderState,DWORD value)
{
	pd3dDevice->SetRenderState(renderState,value);
}

bool Device::SetViewport(int x,int y,int w,int h,float minz,float maxz)
{
	D3DVIEWPORT9 vp;
	vp.X      = DWORD(x);
	vp.Y      = DWORD(y);
	vp.Width  = DWORD(w);
	vp.Height = DWORD(h);
	vp.MinZ   = minz;
	vp.MaxZ   = maxz;

	HRESULT hr=pd3dDevice->SetViewport(&vp);
	return SUCCEEDED(hr);
}

void Device::SetupViewportTransformation(int width,int height,int zoom)
{
	D3DXMATRIXA16 projection;
	D3DXMatrixOrthoLH(&projection,float(width/zoom),float(height/zoom),-100000,100000);

	SetProjectionTransform(&projection);
	SetViewTransform(&Device::identityMatrix);
	SetViewport(0,0,width,height);
}

void Device::SetWorldTransform(const D3DXMATRIX* m)
{
	if (m!=NULL)
	{
		pd3dDevice->SetTransform(D3DTS_WORLD,m);
	}
	else
	{
		pd3dDevice->SetTransform(D3DTS_WORLD,&identityMatrix);
	}
}

void Device::SetViewTransform(const D3DXMATRIXA16* m)
{
	if (m!=NULL)
	{
		pd3dDevice->SetTransform(D3DTS_VIEW,m);
	}
	else
	{
		pd3dDevice->SetTransform(D3DTS_VIEW,&identityMatrix);
	}
}

void Device::SetIdentityTransform()
{
	pd3dDevice->SetTransform(D3DTS_WORLD,&identityMatrix);
}

void Device::SetProjectionTransform(const D3DXMATRIXA16* p)
{
	if (p!=NULL)
	{
		pd3dDevice->SetTransform(D3DTS_PROJECTION,p);
	}
	else
	{
		pd3dDevice->SetTransform(D3DTS_PROJECTION,&identityMatrix);
	}
}

void Device::SetSamplerState(DWORD stage,D3DSAMPLERSTATETYPE type,DWORD value)
{
	pd3dDevice->SetSamplerState(stage, type, value);
}

void Device::SetTextureStageState(DWORD stage,D3DTEXTURESTAGESTATETYPE type,DWORD value)
{
	pd3dDevice->SetTextureStageState(stage,type,value);
}

void Device::SetFVF(DWORD fvf)
{
	pd3dDevice->SetPixelShader(0);
	pd3dDevice->SetVertexShader(0);
	pd3dDevice->SetFVF(fvf);
}

bool Device::CreateTextureFromFile(const std::string& fname,int width,int height,int mipLevels,
								   DWORD usage,D3DFORMAT format,D3DPOOL pool,DWORD filter,DWORD mipFilter,
								   D3DCOLOR colourKey,D3DXIMAGE_INFO* srcInfo,PALETTEENTRY* palette,
								   LPDIRECT3DTEXTURE9* texture)
{
	return (SUCCEEDED(D3DXCreateTextureFromFileEx(pd3dDevice,fname.c_str(),width,height,mipLevels,
												  usage,format,pool,filter,mipFilter,colourKey,
												  srcInfo,palette,texture))==TRUE);
}

IDirect3DVertexBuffer9* Device::CreateVertexBuffer(int length,DWORD usage,DWORD fvf,D3DPOOL pool)
{
	IDirect3DVertexBuffer9* vb;
	HRESULT hr = pd3dDevice->CreateVertexBuffer(length,usage,fvf,pool,&vb,NULL);
	if (SUCCEEDED(hr))
	{
		return vb;
	}
	return NULL;
}

IDirect3DIndexBuffer9* Device::CreateIndexBuffer(UINT length,DWORD usage,D3DFORMAT format,D3DPOOL pool)
{
	IDirect3DIndexBuffer9* ib;
	if (SUCCEEDED(pd3dDevice->CreateIndexBuffer(length,usage,format,pool,&ib,NULL))==TRUE)
	{
		return ib;
	}
	return NULL;
}

bool Device::SetStreamSource(int stream,IDirect3DVertexBuffer9* vb,int offset,int stride)
{
	return (SUCCEEDED(pd3dDevice->SetStreamSource(stream,vb,offset,stride))==TRUE);
}

bool Device::SetIndices(IDirect3DIndexBuffer9* i)
{
	return (SUCCEEDED(pd3dDevice->SetIndices(i))==TRUE);
}

void Device::SetColour(const D3DXCOLOR& colour)
{
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,colour);
}

void Device::DrawAxes(const D3DXMATRIXA16* matrix,const float scale)
{
	static float lines[] = { 0,0,0, scale,0,0, 0,0,0, 0,scale,0, 0,0,0, 0,0,scale };
	static WORD  faces[] = { 0, 1, 2, 3, 4, 5 };

	SetWorldTransform(matrix);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZ);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DXCOLOR(1,0,0,1));
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,6,1,&faces[0],D3DFMT_INDEX16,lines,sizeof(D3DXVECTOR3));
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DXCOLOR(0,1,0,1));
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,6,1,&faces[2],D3DFMT_INDEX16,lines,sizeof(D3DXVECTOR3));
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DXCOLOR(0,0,1,1));
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,6,1,&faces[4],D3DFMT_INDEX16,lines,sizeof(D3DXVECTOR3));
}

void Device::DrawLines(int numVertices,int numFaces,D3DXVECTOR3* vertices,WORD* faces,const D3DXCOLOR& colour)
{
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,colour);

	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZ);
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,numVertices,numFaces,faces,
									   D3DFMT_INDEX16,vertices,sizeof(D3DXVECTOR3));
}

void Device::DrawLine(const D3DXVECTOR2& p1,const D3DXVECTOR2& p2,const D3DXCOLOR& col)
{
	WORD indices[]={0,1};
	D3DXVECTOR4 vert[2];

	vert[0]=D3DXVECTOR4(p1.x,p1.y,0,1);
	vert[1]=D3DXVECTOR4(p2.x,p2.y,0,1);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,col);

	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZRHW);

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,2,1,indices,D3DFMT_INDEX16,vert,sizeof(D3DXVECTOR4));
}

void Device::DrawLine(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,const D3DXCOLOR& col)
{
	WORD indices[]={0,1};
	D3DXVECTOR3 vert[2];

	vert[0]=p1;
	vert[1]=p2;

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,col);

	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZ);

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,2,1,indices,D3DFMT_INDEX16,vert,sizeof(D3DXVECTOR3));
}

void Device::FillRect(const D3DXVECTOR2& leftTop,const D3DXVECTOR2& rightBottom,const D3DXCOLOR& col)
{
	WORD indices[]={0,1,2,2,3,0};
	D3DXVECTOR4 vert[4];

	vert[0]=D3DXVECTOR4(leftTop.x,leftTop.y,0,1);
	vert[1]=D3DXVECTOR4(leftTop.x,rightBottom.y,0,1);
	vert[2]=D3DXVECTOR4(rightBottom.x,rightBottom.y,0,1);
	vert[3]=D3DXVECTOR4(rightBottom.x,leftTop.y,0,1);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,col);

	if (col.a < 1)
	{
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	}

	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZRHW);

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,2,indices,D3DFMT_INDEX16,vert,sizeof(D3DXVECTOR4));

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Device::FillRect(const D3DXVECTOR3& leftTop,const D3DXVECTOR3& rightBottom,const D3DXCOLOR& col)
{
	WORD indices[]={0,1,2,2,3,0};
	D3DXVECTOR3 vert[4];

	vert[0] = D3DXVECTOR3(leftTop.x,leftTop.y,leftTop.z);
	vert[1] = D3DXVECTOR3(leftTop.x,rightBottom.y,leftTop.z);
	vert[2] = D3DXVECTOR3(rightBottom.x,rightBottom.y,leftTop.z);
	vert[3] = D3DXVECTOR3(rightBottom.x,leftTop.y,leftTop.z);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,col);

	if (col.a < 1)
	{
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	}

	pd3dDevice->SetTexture(0,0);
	pd3dDevice->SetFVF(D3DFVF_XYZ);

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,2,indices,D3DFMT_INDEX16,vert,sizeof(D3DXVECTOR3));

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Device::FillRect(const D3DXVECTOR2& leftTop, const D3DXVECTOR2& rightBottom, const D3DXCOLOR& col, Texture* texture)
{
	WORD indices[]={0,1,2,2,3,0};
	D3DPVERTEX4 vert[4];

	vert[0].position = D3DXVECTOR4(leftTop.x,rightBottom.y,0,1);
	vert[0].texture  = D3DXVECTOR2(0,1);
	vert[1].position = D3DXVECTOR4(leftTop.x,leftTop.y,0,1);
	vert[1].texture  = D3DXVECTOR2(0,0);
	vert[2].position = D3DXVECTOR4(rightBottom.x,leftTop.y,0,1);
	vert[2].texture  = D3DXVECTOR2(1,0);
	vert[3].position = D3DXVECTOR4(rightBottom.x,rightBottom.y,0,1);
	vert[3].texture  = D3DXVECTOR2(1,1);

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,col);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pd3dDevice->SetTexture(0,texture->GetTexture());
	pd3dDevice->SetFVF(D3DPFVF4);

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,2,indices,D3DFMT_INDEX16,vert,sizeof(D3DPVERTEX4));

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Device::DrawDisplayBar(float x, float y, float amount, float maxAmount,
							D3DXCOLOR colour1, D3DXCOLOR colour2, float barLen, float barHeight)
{
	float perc1 = amount / maxAmount;
	float perc2 = 1 - perc1;

	D3DXVECTOR2 leftTop, rightBottom;

	if (perc1 > 0)
	{
		leftTop = D3DXVECTOR2(x, y + barHeight);
		rightBottom = D3DXVECTOR2(x + perc1 * barLen, y);
		FillRect(leftTop, rightBottom, colour1);
	}
	if (perc1 < 1)
	{
		leftTop = D3DXVECTOR2(x + perc1 * barLen, y + barHeight);
		rightBottom = D3DXVECTOR2(x + barLen, y);
		FillRect(leftTop, rightBottom, colour2);
	}
}

LPDIRECT3DVERTEXDECLARATION9 Device::GetVertexDeclaration()
{
	if (pVertexDeclaration==NULL)
	{
		static D3DVERTEXELEMENT9 declD3D[] =
		{
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0},
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
			D3DDECL_END()
		};

		HRESULT hr = pd3dDevice->CreateVertexDeclaration(declD3D,&pVertexDeclaration);
		PreCond(SUCCEEDED(hr));
	}
	return pVertexDeclaration;
}

LPDIRECT3DVERTEXDECLARATION9 Device::GetColourVertexDeclaration()
{
	if (pColourVertexDeclaration==NULL)
	{
		static D3DVERTEXELEMENT9 declColourD3D[] =
		{
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0},
			{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR, 0},
			D3DDECL_END()
		};

		HRESULT hr = pd3dDevice->CreateVertexDeclaration(declColourD3D,&pColourVertexDeclaration);
		PreCond(SUCCEEDED(hr));
	}
	return pColourVertexDeclaration;
}

void Device::RenderMesh(Mesh* mesh,const D3DXMATRIX& transform)
{
	if (mesh!=NULL && mesh->GetNumTriangles()>0)
	{
		SetWorldTransform(&transform);
		pd3dDevice->SetVertexDeclaration(GetVertexDeclaration());
		pd3dDevice->SetIndices(mesh->GetIndexBuffer());
		pd3dDevice->SetStreamSource(0,mesh->GetVertexBuffer(),0,sizeof(D3DPVERTEX));
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,
										 mesh->GetNumVertices(),0,
										 mesh->GetNumTriangles());
	}
}

void Device::SetLight(int index, const D3DLIGHT9& pLight)
{
	pd3dDevice->SetLight(index,&pLight);
}

void Device::LightEnable(int index, bool enable)
{
	pd3dDevice->LightEnable(index,enable?TRUE:FALSE);
}

