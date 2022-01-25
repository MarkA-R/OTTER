/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

CMorphAnimator.h
Simple animator component for demonstrating morph target animation.

As a convention in NOU, we put "C" before a class name to signify
that we intend the class for use as a component with the ENTT framework.
*/

#pragma once

#include "NOU/Entity.h"

namespace nou
{
	class Mesh;

	class CMorphAnimator
	{
	public:

		CMorphAnimator(Entity& owner);
		void SetFrameTime(float f);
		void SetFrames(const std::vector<Mesh*> frames);
		void setFrameAndTime(int currentFrame, int nextFrame, float t);
		void setMeshAndTime(Mesh* currentFrame, Mesh* nextFrame, float t);
		float getT();
		void addToT(float);
		void setT(float);
		~CMorphAnimator() = default;

		CMorphAnimator(CMorphAnimator&&) = default;
		CMorphAnimator& operator=(CMorphAnimator&&) = default;

		void Update(float deltaTime);

		// TODO: Add function to load frames
		
	protected:

		Entity* m_owner;
		
		class AnimData
		{
		public:

			//TODO: You'll need to define a way to store and manage full
			//animation clips for the exercise.
			//const Mesh* frame0;
			//const Mesh* frame1;
			std::vector<Mesh*> animationFrames;
			//The time inbetween frames.
			float frameTime;

			AnimData();
			~AnimData() = default;
		};

		std::unique_ptr<AnimData> m_data;

		float m_timer = 0;
		bool m_forwards;
		

	};
}