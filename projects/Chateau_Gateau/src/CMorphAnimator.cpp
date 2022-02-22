/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

CMorphAnimator.cpp
Simple animator component for demonstrating morph target animation.

As a convention in NOU, we put "C" before a class name to signify
that we intend the class for use as a component with the ENTT framework.
*/

#include "CMorphAnimator.h"
#include "CMorphMeshRenderer.h"
#include "NOU/Mesh.h"
#include <iostream>

namespace nou
{
	template<typename T>
	T Lerp(T s, T e, T t) {
		return ((1.f - t) * s) + (t * e);
	}

	CMorphAnimator::AnimData::AnimData()
	{
		animationFrames = std::vector<Mesh*>();
		frameTime = 1.0f;
	}

	

	CMorphAnimator::CMorphAnimator(Entity& owner)
	{
		m_owner = &owner;
		m_data = std::make_unique<AnimData>();
		m_timer = 0.0f;
		m_forwards = true;
	}

	
	void CMorphAnimator::SetFrameTime(float f) {
		m_data->frameTime = f;
	}
	void CMorphAnimator::SetFrames(std::vector<Mesh*> frames) {
		m_data->animationFrames = frames;
	}

	void CMorphAnimator::setFrameAndTime(int currentFrame, int nextFrame, float t){
		m_owner->Get<CMorphMeshRenderer>().UpdateData(*m_data->animationFrames[(int)currentFrame], *m_data->animationFrames[(int)nextFrame], t);

	}
	void CMorphAnimator::setMeshAndTime(Mesh* currentFrame, Mesh* nextFrame, float t) {
		m_owner->Get<CMorphMeshRenderer>().UpdateData(*currentFrame, *nextFrame, t);

	}

	float CMorphAnimator::getT()
	{
		return m_timer;
	}

	void CMorphAnimator::addToT(float x)
	{
		m_timer += x;

		
	}

	void CMorphAnimator::addToTLessThanOne(float x)
	{
		m_timer += x;

		if (m_timer > 1) {
			m_timer = 1;
		}
		if (m_timer < 0) {
			m_timer = 0;
		}
	}

	void CMorphAnimator::setT(float x)
	{
		m_timer = x;
	}

	void CMorphAnimator::Update(int frame1Index, int frame2Index)
	{
		m_owner->Get<CMorphMeshRenderer>().UpdateData(*m_data->animationFrames[frame1Index], *m_data->animationFrames[frame2Index], m_timer);

	}

	void CMorphAnimator::Update(float deltaTime)
	{
		// TODO: Complete this function
		m_forwards = true;
		m_timer += deltaTime;
		double frameNumber;
		double nextFrame;
		double t = modf(m_timer, &frameNumber);
		nextFrame = frameNumber + 1;
		if (m_forwards) {
			if (frameNumber >= (m_data->animationFrames.size()-1)) {
				frameNumber = (int)frameNumber % (m_data->animationFrames.size());//(animationFrames.size()) % (int)frameNumber
				nextFrame = (int)(frameNumber + 1) % (m_data->animationFrames.size());
			}

			
		}
		else
		{
			if (frameNumber >= (m_data->animationFrames.size() -1)) {
				frameNumber = m_data->animationFrames.size() - 2;
				nextFrame = m_data->animationFrames.size() - 1;
			}

		}

		m_owner->Get<CMorphMeshRenderer>().UpdateData(*m_data->animationFrames[(int)frameNumber], *m_data->animationFrames[(int)nextFrame], t);
		//std::cout << frameNumber << std::endl;
		
	}

}