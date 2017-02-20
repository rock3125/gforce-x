//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

// fwd
class Texture;
class Mesh;
class Font;

/////////////////////////////////////////////////////////////////////////////
// d3d device function wrapper

class Device
{
	Device(const Device&);
	const Device& operator=(const Device&);
public:
	Device(IDirect3DDevice9* pd3dDevice);
	virtual ~Device();

	// access backbuffer width and height
	int GetWidth() const;

	// access backbuffer width and height
	int GetHeight() const;

	// is the system in windowed mode?
	bool IsWindowed() const;

	// interface will call this to set my device
	void SetDevice(IDirect3DDevice9* pd3dDevice);
	IDirect3DDevice9* GetDevice();

	// get main app font
	virtual Font* GetFont();

	// lighting
	void SetLight(int index, const D3DLIGHT9& pLight);
	void LightEnable(int index, bool enable);

	// get vertex decl.
	LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration();
	LPDIRECT3DVERTEXDECLARATION9 GetColourVertexDeclaration();

	// misc api wrappers
	bool SetTexture(DWORD sampler,Texture* texture);
	bool BeginScene();
	bool EndScene();
	bool Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color, float Z,DWORD Stencil);
	bool DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	bool DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride);
	bool DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount);
	bool DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	void Present();
	void Present(RECT* source, RECT* dest);
	void SetRenderState(D3DRENDERSTATETYPE renderState,DWORD value);
	bool SetViewport(int x,int y,int w,int h,float minz=0,float maxz=1);
	void SetWorldTransform(const D3DXMATRIX* m);
	void SetViewTransform(const D3DXMATRIXA16* m);
	void SetIdentityTransform();
	void SetProjectionTransform(const D3DXMATRIXA16* p);
	void SetSamplerState(DWORD stage,D3DSAMPLERSTATETYPE type,DWORD value);
	void SetTextureStageState(DWORD stage,D3DTEXTURESTAGESTATETYPE type,DWORD value);
	void SetFVF(DWORD fvf);
	bool CreateTextureFromFile(const std::string& fname,int width,int height,int mipLevels,
							   DWORD usage,D3DFORMAT format,D3DPOOL pool,DWORD filter,DWORD mipFilter,
							   D3DCOLOR colourKey,D3DXIMAGE_INFO* srcInfo,PALETTEENTRY* palette,
							   LPDIRECT3DTEXTURE9* texture);
	IDirect3DVertexBuffer9* CreateVertexBuffer(int length,DWORD usage,DWORD fvf,D3DPOOL pool);
	IDirect3DIndexBuffer9* CreateIndexBuffer(UINT length,DWORD usage,D3DFORMAT format,D3DPOOL pool);
	bool SetStreamSource(int stream,IDirect3DVertexBuffer9* vb,int offset,int stride);
	bool SetIndices(IDirect3DIndexBuffer9* i);
	void SetColour(const D3DXCOLOR& colour);

	// setup view port to width height, zoom level
	void SetupViewportTransformation(int width,int height,int zoom=1);

	// draw a 2d line
	void DrawLine(const D3DXVECTOR2& p1,const D3DXVECTOR2& p2,const D3DXCOLOR& col);

	// draw a line in 3d
	void DrawLine(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,const D3DXCOLOR& col);

	// draw a rect in 2d
	void FillRect(const D3DXVECTOR2& leftTop,const D3DXVECTOR2& rightBottom,const D3DXCOLOR& col);
	void FillRect(const D3DXVECTOR2& leftTop, const D3DXVECTOR2& rightBottom, const D3DXCOLOR& col, 
				  Texture* texture);

	// draw flat in 3d
	void FillRect(const D3DXVECTOR3& leftTop,const D3DXVECTOR3& rightBottom,const D3DXCOLOR& col);

	// display a 2d 2-coloured progress bar at (x,y)
	void DrawDisplayBar(float x, float y, float amount, float maxAmount,
						D3DXCOLOR colour1, D3DXCOLOR colour2, float barLen, float barHeight);

	// draw a set of axes
	void DrawAxes(const D3DXMATRIXA16* matrix,const float scale = 25.0f);

	// draw a series of lines using index primitive up
	void DrawLines(int numVertices,int numFaces,D3DXVECTOR3* vertices,WORD* faces,const D3DXCOLOR& colour);

	// add a mesh to the list to be rendered
	void RenderMesh(Mesh* mesh,const D3DXMATRIX& transform);

private:
	friend class Interface;

	// accessed by interface to set width height
	void SetWidthHeightWindowed(int width,int height,bool windowed);

public:
	// setup identity for efficient use
	static D3DXMATRIXA16 identityMatrix;

protected:
	// device management
	bool			deviceLost;

	// accessors (really set by interface)
	int				width;
	int				height;
	bool			windowed;

	// system main font
	Font*			font;

	// device (owned by interface)
	IDirect3DDevice9*				pd3dDevice;
    LPDIRECT3DVERTEXDECLARATION9	pVertexDeclaration;
	LPDIRECT3DVERTEXDECLARATION9	pColourVertexDeclaration;
};

