#include "FillBar.h"

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

void FillBar::checkT() {
	if (t < 0) {
		t = 0;
	}
	if (t > maxT) {
		t = maxT;
	}
}

void FillBar::addT(float x)
{
	t += x;
	checkT();
}

void FillBar::setT(float x)
{
	t = x;
	checkT();
}

float FillBar::getT()
{

	return t;

}

void FillBar::setup(MaterialCreator& bottom, MaterialCreator& top, Transform pos, float scale)
{
	position = pos;
	t = 0;
	float transformScale = scale;

	bar = Entity::Allocate();
	bar.get()->Add<CMorphMeshRenderer>(*bar, *bottom.getMesh(), *bottom.getMaterial());
	
	bar.get()->transform.SetParent(&position);
	bar.get()->transform.m_rotation = position.m_rotation;
	/*
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));// *
		//;
		*/
	bar.get()->transform.m_scale = glm::vec3(1.0, 1.0, 1.0) * transformScale;
	bar.get()->transform.m_pos = position.m_pos;
	
	auto& animator = bar.get()->Add<CMorphAnimator>(*bar.get());
	animator.SetFrameTime(1.0f);
	frames.push_back(bottom.getMesh().get());
	frames.push_back(top.getMesh().get());
	animator.SetFrames(frames);
	
}



void FillBar::updateArrow()
{
	bar.get()->Get<CMorphAnimator>().setFrameAndTime(0, 1, t);
	

}

void FillBar::setPosition(Transform pos)
{
	position = pos;
	bar.get()->transform = pos;


}

Entity* FillBar::getEntity()
{
	return bar.get();
}
