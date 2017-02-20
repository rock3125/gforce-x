#pragma once

class Bone;

/////////////////////////////////////////////////////////////////////////////

// each vertex has weights applied to it - this is a single
// bone, weight relationship for such a vertex

class BoneInfluence
{
public:
	BoneInfluence();
	~BoneInfluence();
	BoneInfluence(const BoneInfluence&);
	const BoneInfluence& operator=(const BoneInfluence&);

	// get bones and weights
	Bone** GetBones();
	float* GetWeights();
	int GetCount();

	// add a bone weight for this bone influence
	void Add(Bone* bone, float weight);

	// read and write bone influences
	virtual void Read(int, Bone* rootBone, BaseStreamer&);
	virtual void Write(int, BaseStreamer&);

private:
	friend class Skin;

	int			count;
	Bone**		bones;
	float*		weights;

	// signature and version
	static std::string boneInfluenceSignature;
	static int boneInfluenceVersion;
};

