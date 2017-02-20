#pragma once

#include "system/modelImporter.h"

class Skin; // fwd.
class Bone;

////////////////////////////////////////////////////
// bvh = bio-vision hierarchy

/*

	bvh			->	'HIERARCHY' hierarchy motion

	hierarchy	->	'ROOT' ident '{' offsetChannels '}'								|
					'JOINT' ident '{' offsetChannels [ '}' | hierarchy ] hierarchy	|
					'End' 'Site' '{' 'OFFSET' f f f '}'

	offsetChannels	->	'OFFSET' f f f
						[ 'CHANNELS' i ['Xposition' 'Yposition' 'Zposition' 'Xrotation' 'Zrotation' 'Yrotation'] ]

	motion		->	'MOTION' 'Frames' ':' i
					'Frame' 'Time' ':' f
					[f f f f f...]+

*/


/*
HIERARCHY
ROOT hip
{
	OFFSET	0.00  0.00  0.00 
	CHANNELS 6 Xposition Yposition Zposition Xrotation Zrotation Yrotation
	JOINT abdomen
	{
		OFFSET	0.000000 0.000000 0.000000
		CHANNELS 3 Xrotation Zrotation Yrotation
		JOINT chest
		{
			OFFSET	0.000000 5.018152 -1.882228
			CHANNELS 3 Xrotation Zrotation Yrotation
			JOINT neckDummy
			{
				OFFSET	0.000000 8.316447 0.784897
				CHANNELS 3 Xrotation Yrotation Zrotation
				JOINT neck
				{
					OFFSET	0.000000 2.280413 -0.392801
					CHANNELS 3 Xrotation Zrotation Yrotation
					JOINT head
					{
						OFFSET	0.000000 3.496879 0.529469
						CHANNELS 3 Xrotation Zrotation Yrotation
						JOINT figureHair
						{
							OFFSET	0.000000 4.699570 0.720622
							CHANNELS 3 Zrotation Yrotation Xrotation
							End Site
							{
								OFFSET 0.000000 -6.419331 0.000000
							}
						}
MOTION
Frames:     31
Frame Time: 0.033333
0.000000 36.019642 -0.225302 17.050800 0.000000 -15.117300 12.123600 -0.266650 -0.329145 -13.150100 2.351230 14.680100 0.000000 0.000000 0.000000 -4.835840 1.622700 -0.134806 -3.465930 -0.589411 0.719630 0.000000 0.000000 0.000000 5.035830 -2.367540 -2.657900 -62.280090 36.021900 -0.677780 -13.663729 -4.878168 10.222091 -16.778969 -11.448589 0.328913 -2.833590 4.114660 -2.402260 70.378891 26.903761 1.211260 47.494614 26.370754 2.880988 7.761272 29.674999 -1.128774 -59.433193 12.224557 -3.196975 29.195473 -2.584550 -1.951688 -27.274197 11.705568 3.853327 1.015180 -5.413131 1.074486 33.684574 4.046022 2.624600 -14.958674 13.755541 -4.153951 

*/

class BVH : public ModelImporter
{
public:
	BVH();
	~BVH();

	// parse the complete bvh
	WorldObject* ParseLoadedFile();

private:
	void ParseBVH();
	class HierarchyObj;
	HierarchyObj* Hierarchy(HierarchyObj*);
	void OffsetChannels(float& o1, float& o2, float& o3, std::vector<int>& channels);
	void Motion();

	// helper - parse 3 floats
	void Get3Floats(float& f1, float& f2, float& f3);

	enum
	{
		X_POS,
		Y_POS,
		Z_POS,
		X_ROT,
		Y_ROT,
		Z_ROT
	};

	// count the number of channels in the bone hierarchy
	static int GetChannelCount(HierarchyObj*);

	// convert a hierarchy to bones
	Bone* ConvertToBones(HierarchyObj* obj, int& offset);
	void GetPosRot(D3DXVECTOR3& pos, D3DXQUATERNION& quat, int animIndex, int offset,
				   const D3DXVECTOR3& inpos, const std::vector<int>& channels);

private:
	class HierarchyObj
	{
	public:
		HierarchyObj()
		{
			position = D3DXVECTOR3(0,0,0);
		}

		std::string			name;
		std::vector<int>	channels;
		D3DXVECTOR3			position;

		std::vector<HierarchyObj*>	children;
	};

	// Hierarchy root of system
	HierarchyObj*	root;

	// animation settings
	int channelCount;
	float frameTime;
	std::vector<float*>	animation;
};

