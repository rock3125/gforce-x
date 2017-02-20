#include "stdafx.h"

//==========================================================================

#include "CreateFont/fontDialog.h"

//==========================================================================
//
//	System global vars
//

HBRUSH		blackBrush = NULL;
HPEN		whitePen = NULL;
HFONT		arialFont = NULL;

std::string		_dir;
char		fontName[256];

typedef unsigned char byte;

const size_t lw = 32;
const size_t lh = 32;

byte   letters[256*lw*lh]; // 256 letters x 16 heigh each x max 40 wide each
size_t widths[256];

void WriteString(const char* str)
{
	printf("%s", str);
}

//==========================================================================

LRESULT CALLBACK WndProc(   HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam	);

//==========================================================================
//
// Entry point of all Windows programs
//

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	FontDialog dlg( hInst );

	if ( dlg.Execute() )
	{
		LOGFONT lf   = dlg.Font();
		int		size = dlg.Size();

		arialFont = ::CreateFont( -size, 0, lf.lfEscapement,
								  lf.lfOrientation, lf.lfWeight, lf.lfItalic,
								  lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
								  lf.lfOutPrecision, lf.lfClipPrecision,
								  lf.lfQuality, lf.lfPitchAndFamily,
								  lf.lfFaceName );

		sprintf( fontName, "%s%d.fnt", lf.lfFaceName, size );

		// back brush to clear background
		blackBrush = HBRUSH(::GetStockObject(WHITE_BRUSH));
		whitePen = HPEN(::GetStockObject(BLACK_PEN));

		//
		// Register the window class
		//
		WNDCLASSEX wc;
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc		= (WNDPROC)WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInst;
		wc.hIcon			= NULL;
		wc.hCursor			= ::LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= "CreateFont";
		wc.hIconSm			= NULL;

		if(::RegisterClassEx(&wc) == 0)
		{
			DWORD errNum = ::GetLastError();
			std::string errStr = "Failed to register windows class (" + System::Int2Str( errNum ) + ")";
			WriteString( errStr.c_str() );
			return -1;
		}

		size_t windowStyle = WS_CAPTION | WS_SYSMENU |
							 WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
							 WS_VISIBLE | WS_OVERLAPPED | 
							 WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		HWND hwnd = ::CreateWindow( "CreateFont",
									"Bin Font Creator",
									windowStyle,
									0,0,
									640,480,
									NULL,
									NULL,
									hInst,
									NULL );

		MSG  msg;
		WriteString( "entering main system loop\n\n" );
		do
		{
			BOOL bGotMsg = ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
			if( bGotMsg )
			{
				// Translate and dispatch the message
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
			else
			{
				::WaitMessage();
			}
		}
		while ( msg.message!=WM_QUIT );

		// delete GDI objects
		if ( arialFont!=NULL )
			::DeleteObject(arialFont);
		arialFont = NULL;

		if ( blackBrush!=NULL )
			::DeleteObject(blackBrush);
		blackBrush = NULL;

		if ( whitePen!=NULL )
			::DeleteObject(whitePen);
		whitePen = NULL;

		return msg.wParam;
	}
	return 0;
}

//==========================================================================

void StoreLetter( size_t index, HDC dc, size_t xp, size_t yp, size_t w, size_t h )
{
	PreCond( w < lw );
	PreCond( h == lh );
	PreCond( index < 256 );

	byte* buf = &letters[index*lw*lh];

	w++;
	widths[index] = w;

	// zero the buffer letter first
	for ( size_t i=0; i<(lw*lh); i++ )
	{
		buf[i] = 0;
	}

	size_t white = RGB(255,255,255);
	for ( size_t y=0; y<h; y++ )
	for ( size_t x=0; x<w; x++ )
	{
		size_t col = ::GetPixel( dc, xp+x, yp+y );
		if ( col==white )
		{
			buf[x+y*lw] = 1;
		}
		else
		{
			buf[x+y*lw] = 0;
		}
	}
};

void WriteFont( const std::string& fname, size_t maxWidth )
{
	size_t cellSize = 16;
	if ( maxWidth >= 16 )
	{
		cellSize = 32;
	}

	// 256/16 = 16
	// so 16 letters across, 16 rows down
	size_t mapWidth = cellSize * 16 * 4;
	size_t mapHeight = lh * 16;

	byte* rgbaMap = new byte[mapWidth*mapHeight];

	// zero rgbaMap data
	for ( size_t i=0; i<(mapWidth*mapHeight); i++ )
	{
		rgbaMap[i] = 0;
	}

	// copy the letters onto the rgba map
	for ( size_t counter=0; counter<256; counter++ )
	{
		for ( size_t y=0; y<lh; y++ )
		for ( size_t x=0; x<widths[counter]; x++ )
		{
			size_t cellAddress = (counter%16) * cellSize * 4 + 
								 (counter/16) * lh * mapWidth;
			byte* cell = &rgbaMap[cellAddress];

			if ( letters[ counter*lw*lh + x+y*lw]==1 )
			{
				cell[ x*4 + y*mapWidth + 0] = 255;
				cell[ x*4 + y*mapWidth + 1] = 255;
				cell[ x*4 + y*mapWidth + 2] = 255;
				cell[ x*4 + y*mapWidth + 3] = 255;
			}
			else
			{
				cell[ x*4 + y*mapWidth + 0] = 0;
				cell[ x*4 + y*mapWidth + 1] = 0;
				cell[ x*4 + y*mapWidth + 2] = 0;
				cell[ x*4 + y*mapWidth + 3] = 0;
			}
		}
	}


	FILE* fh = fopen(fname.c_str(), "wb");
	if (fh != NULL)
	{
		std::string signature = "PDVFNT02";
		fwrite(signature.c_str(), signature.length()+1, 1, fh);

		size_t version = 2;
		fwrite(&version, sizeof(size_t), 1, fh);

		// max size of cell width
		fwrite(&cellSize, sizeof(size_t), 1, fh);

		// size of cell height
		fwrite(&lh, sizeof(size_t), 1, fh);

		// write inidividual widths
		fwrite(widths,sizeof(size_t), 256, fh);

		// byte width and total height in rows
		fwrite(&mapWidth,sizeof(size_t), 1, fh);
		fwrite(&mapHeight,sizeof(size_t), 1, fh);

		// the map itself
		fwrite(rgbaMap,mapWidth*mapHeight, 1, fh);
		fclose(fh);
	}
};

//==========================================================================
//
// Window procedure, handles all messages for this program
//
LRESULT CALLBACK WndProc(   HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam	)
{
	switch (message)
	{
		case WM_CREATE:
			{
				break;
			}

		case WM_CLOSE:
			{
				::PostQuitMessage(0);
				return 0;
			}

        case WM_PAINT:
			{
				PAINTSTRUCT ps;
				::BeginPaint(hWnd,&ps);

				size_t maxWidth = 0;
				for ( size_t counter=0; counter<256; counter++ )
				{
					RECT r;
					::GetClientRect( hWnd, &r );
					::FillRect( ps.hdc, &r, blackBrush );

					::SelectObject( ps.hdc, arialFont );
					char buf[2];
					buf[0] = char(counter);
					buf[1] = 0;
					::TextOut( ps.hdc, 100,100, buf, 1 );

					// copy the letter into an RGBA structure

					// find the extends of the letter
					size_t width = 0;
					size_t white = RGB(255,255,255);
					size_t black = RGB(0,0,0);
					size_t h = lh;
					for ( size_t x=0; x<40; x++ )
					for ( size_t y=0; y<h; y++ )
					{
						size_t col = ::GetPixel( ps.hdc, 99+x,100+y );
						if ( col==white )
						{
							::SetPixel( ps.hdc, 99+x, 100+y, black );
						}
						else
						{
							::SetPixel( ps.hdc, 99+x, 100+y, white );
							if ( x > width )
							{
								width = x;
							}
						}
					}

					if (counter==32)
					{
						width = 4;
					}
					if (width >= lw)
					{
						width = lw - 1;
					}
					StoreLetter( counter, ps.hdc, 99,100, width, h );

					if ( width > maxWidth )
					{
						maxWidth = width;
					}
				}
				WriteFont( fontName, maxWidth+1 ); // +1 to get extra space

				::EndPaint(hWnd,&ps);

				::PostQuitMessage( 0 );

	        	break;
			}

		case WM_TIMER:
			{
				::InvalidateRect( hWnd, NULL, true );
				break;
			}

		default:   // Passes it on if unproccessed
		    return (DefWindowProc(hWnd, message, wParam, lParam));

	}
    return (0L);
}


