#include "standardFirst.h"

#include "system/converters/lwo/lwoImport.h"
#include "system/model/model.h"
#include "system/model/mesh.h"

///////////////////////////////////////////////////////////////////////////////////

LwoImport::LwoImport()
	: fh(NULL)
{
	numberOfSurfaces = 0;
}

LwoImport::~LwoImport()
{
	if (fh != NULL)
	{
		fclose(fh);
		fh = NULL;
	}
}

WorldObject* LwoImport::ParseLoadedFile()
{
	throw new Exception("lwo import does not support a text parser");
	return NULL;
}

WorldObject* LwoImport::LoadBinary(std::string filename)
{
	fopen_s(&fh, filename.c_str(), "rb");
	if (fh == NULL)
	{
		SetError("file " + filename + " could not be opened");
		return NULL;
	}

    // Make sure the Lightwave file is an IFF file.
	long type;
    fread(&type, sizeof(long), 1, fh);
    if (type != 'FORM')
	{
		SetError("Not an IFF file (Missing FORM tag)");
		return NULL;
	}

	long datasize;
    fread(&datasize, sizeof(long), 1, fh);

    // Make sure the IFF file has a LWOB form type.
    fread(&type, sizeof(long), 1, fh);
    if (type != 'LWOB')
	{
		SetError("Not a lightwave object (Missing LWOB tag)");
		return NULL;
	}

    // Read all Lightwave chunks.
    long bytesread = 4;
    while (bytesread < datasize) 
	{
		long size;
		fread(&type, sizeof(long), 1, fh);
		fread(&size, sizeof(long), 1, fh);

		switch (type) 
		{
        case 'PNTS':
			{
				ReadVertices(size);
				break;
			}
        case 'POLS':
			{
				ReadPolygons(size);
				break;
			}
        case 'SRFS':
			{
				ReadSurfaceNames(size);
				break;
			}
        case 'SURF':
			{
				ReadSurface(size);
				break;
			}
		default:
			{
				// wind past the unknown block
				fseek(fh, (long)size, SEEK_CUR);
				break;
			}
		}
		bytesread += (size + 8);
	}

	fclose(fh);
	return NULL;
}

int LwoImport::ReadVertices(long numberOfBytes)
{
	// calculate how many points there are
	long numberOfPoints = numberOfBytes / (sizeof(float) * 3);

	// allocate memory
	float* points = new float[numberOfPoints * 3];
	if (points == NULL)
	{
		SetError("out of memory");
		return 0;
	}
	fread(points, sizeof(float), 3 * numberOfPoints, fh);
	return (int)numberOfPoints;
}

int LwoImport::ReadPolygons(long numberOfBytes)
{
    // POLS chunk must be preceded by the SRFS chunk
    if (numberOfSurfaces == 0)
	{
		throw new Exception("no SRFS chunk in this Lightwave file");
	}

	int numberOfPolygons = numberOfBytes / sizeof(short);
	short* polygons = new short[numberOfPolygons];

    fread((void *)polygons, sizeof(short), numberOfPolygons, fh);
    return numberOfPolygons;
}


void LwoImport::ReadSurface(long numberOfBytes)
{
    // Read the name of the surface
	char name[255];
	long bytesRead = 0;
	int index = 0;
	char ch = 0;
    do 
	{
		ch = fgetc(fh);
		bytesRead++;
		name[index++] = ch;
    } 
	while (ch != 0 && index < 255);
	if (ch != 0)
	{
		SetError("name exceeded 255 characters");
		return;
	}

    // If the length of the surface name is odd, skip another byte.
    if ((bytesRead % 2) > 0) 
	{
		ch = fgetc(fh);
		bytesRead++;
    }

	float sman = 0;

	float fcolour[3];

	float fdiff = 0.4f;
	float fspec = 0;
	float flumi = 0;
	float fglos = 0;
	float ftran = 0;

    while (bytesRead < numberOfBytes) 
	{
		// Handle the various sub-chunks.
		long type;
		short size;
		fread(&type, sizeof(long), 1, fh);
		fread(&size, sizeof(short), 1, fh);
		switch (type) 
		{
        case 'COLR':
			{
				char colour[4];
				fread(&colour, sizeof(char), 4, fh);
                for (int i = 0; i < 3; i++)
				{
					fcolour[i] = (float)colour[i] / 255.0f;
				}
                break;
			}
        case 'DIFF': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                fdiff = (float)var / 255.0f;
                break;
			}
        case 'SPEC': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                fspec = (float)var / 255.0f;
                break;
			}
        case 'GLOS': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                fglos = (float)var / 1025.0f;
                break;
			}
        case 'LUMI': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                flumi = (float)var / 255.0f;
                break;
			}
        case 'TRAN': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                ftran = (float)var / 255.0f;
                break;
			}
        case 'SMAN': 
			{
				short var;
				fread(&var, sizeof(short), 1, fh);
                break;
			}
        default:
			{
				fseek(fh, (long)size, SEEK_CUR);
				break;
			}
      }
      bytesRead += sizeof(long) + sizeof(short) + size;
    }
}

int LwoImport::ReadSurfaceNames(long numberOfBytes)
{
    char* names = new char[numberOfBytes];
    fread((void*)names, 1, numberOfBytes, fh);

    // How many surfaces are there?
    int i = 0;
	int numberOfSurfaces = 0;
    while (i < numberOfBytes)
	{
		i += strlen(&names[i]);
		numberOfSurfaces++;
		// Skip any extra nulls at the end of the surface name.
		while ((names[i] == '\0') && (i < numberOfBytes)) i++;
    }

    // Create surface name array.
    i = 0;
    for (int cnt = 0; cnt < numberOfSurfaces; cnt++) 
	{
		//srfnames[cnt] = SbString(&names[i]);
		i += strlen(&names[i]);
		while ((names[i] == '\0') && (i < numberOfBytes)) i++;
	}

	safe_delete(names);
	return numberOfSurfaces;
}

