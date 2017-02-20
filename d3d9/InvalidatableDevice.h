//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////
// wrap the d3d device

class InvalidatableDevice
{
	InvalidatableDevice(const InvalidatableDevice&);
	const InvalidatableDevice& operator=(const InvalidatableDevice&);
public:
	InvalidatableDevice();
	virtual ~InvalidatableDevice();

	// called from Interface when the time is right
	static void InvalidateObjects();
	static void RestoreObjects();

	// pure virtuals, need to be overwritten and implemented
	// by InvalidatableDevice objects
	virtual void RestoreDeviceObjects() = 0;
	virtual void InvalidateDeviceObjects() = 0;

protected:
	static std::vector<InvalidatableDevice*> objects;
};

