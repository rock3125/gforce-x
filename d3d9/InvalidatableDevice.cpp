//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/InvalidatableDevice.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<InvalidatableDevice*> InvalidatableDevice::objects;

InvalidatableDevice::InvalidatableDevice()
{
	objects.push_back(this);
};

InvalidatableDevice::~InvalidatableDevice()
{
	// remove this from the list of objects
	std::vector<InvalidatableDevice*>::iterator pos = objects.begin();
	while (pos != objects.end())
	{
		if (*pos == this)
		{
			objects.erase(pos);
			break;
		}
		else
		{
			pos++;
		}
	}
};

void InvalidatableDevice::InvalidateObjects()
{
	for (int i = 0; i < objects.size(); i++)
	{
		InvalidatableDevice* invalidatableDevice = objects[i];
		invalidatableDevice->InvalidateDeviceObjects();
	};
};

void InvalidatableDevice::RestoreObjects()
{
	for (int i=0; i<objects.size(); i++)
	{
		objects[i]->RestoreDeviceObjects();
	};
};

