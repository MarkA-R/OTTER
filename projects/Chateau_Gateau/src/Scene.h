#pragma once
class Scene
{
public:
	Scene();
	~Scene();
	virtual void Update(float deltaTime);
	virtual void Setup();
	void setMainCamera(int);
	int getMainCamera();

};

