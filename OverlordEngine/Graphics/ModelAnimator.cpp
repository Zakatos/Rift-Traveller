//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ModelAnimator.h"
#include "../Diagnostics/Logger.h"


ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(vector<XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}


ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < GetClipCount())
	{
		
		SetAnimation(m_pMeshFilter->m_AnimationClips.at(clipNumber));
	}
	else
	{
		Reset();
		Logger::LogWarning(L"clipNumber is bigger than clip count!");
		return;
	}
	//If not,
	//	Call Reset
	//	Log a warning with an appropriate message
	//	return
	//else
	//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
	//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(wstring clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (UINT i = 0; i < GetClipCount(); i++)
	{
		if (m_pMeshFilter->m_AnimationClips.at(i).Name == clipName)
		{
			SetAnimation(m_pMeshFilter->m_AnimationClips.at(i));
		}
		else
		{
			Reset();
			Logger::LogWarning(L"clip not found");
			return;
		}
	}
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
		m_TickCount = 0;
		m_AnimationSpeed = 1.0f;
	}
	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	
	//If m_ClipSet is true
	if (m_ClipSet)
	{
		auto transforms = m_CurrentClip.Keys.begin()->BoneTransforms;

		m_Transforms.assign(transforms.begin(), transforms.end());

	}
	else
	{
		
		XMFLOAT4X4 identityMatrix{ 1,0,0,0,
									0,1,0,0,
									0,0,1,0,
									0,0,0,1};

		UINT count = m_pMeshFilter->m_BoneCount;
		m_Transforms.assign(count, identityMatrix);
	}
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)

	/*if (m_ClipSet = true)
	{
		m_CurrentClip.Keys.
	}*/
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;

		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount

		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0) m_TickCount += m_CurrentClip.Duration;
		}
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration) m_TickCount -= m_CurrentClip.Duration;
		}
		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		auto keyIterator = m_CurrentClip.Keys.begin();

		for (keyIterator; keyIterator != m_CurrentClip.Keys.end(); ++keyIterator)
		{
			if (m_TickCount >= keyIterator->Tick)
			{
				keyA = *keyIterator;
				if (keyIterator + 1 == m_CurrentClip.Keys.end())
				{
					keyB = *m_CurrentClip.Keys.begin();
				}
				else
				{
					keyB = *(keyIterator + 1);
				}
			}
		}
		
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		
		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//Clear the m_Transforms vector
		//FOR every boneTransform in a key (So for every bone)
		m_Transforms.clear();

		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++)
		{
			
			auto transformA = keyA.BoneTransforms.at(i);
			XMMATRIX tranformAMat = XMLoadFloat4x4(&transformA);
			
			auto transformB = keyB.BoneTransforms.at(i);
			XMMATRIX tranformBMat = XMLoadFloat4x4(&transformB);

			XMVECTOR translationA;
			XMVECTOR rotA;
			XMVECTOR scaleA;
			XMMatrixDecompose(&scaleA, &rotA, &translationA, tranformAMat);

			XMVECTOR translationB;
			XMVECTOR rotB;
			XMVECTOR scaleB;
			XMMatrixDecompose(&scaleB, &rotB, &translationB, tranformBMat);

			float tickA = keyA.Tick;
			float tickB = keyB.Tick;

			float blend = (m_TickCount - tickA) / (tickB - tickA);
			XMVECTOR lerpTranslation = XMVectorLerp(translationA, translationB, blend);
			XMVECTOR slerpRot = XMQuaternionSlerp(rotA, rotB, blend);
			XMVECTOR lerpScale = XMVectorLerp(scaleA, scaleB, blend);

			
			XMMATRIX result = XMMatrixScalingFromVector(lerpScale)*
				XMMatrixRotationQuaternion(slerpRot)*
				XMMatrixTranslationFromVector(lerpTranslation);

			XMFLOAT4X4 resultMatrix;
			XMStoreFloat4x4(&resultMatrix, result);

			m_Transforms.push_back(resultMatrix);
		}
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
	}
}
