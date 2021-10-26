#pragma once
class Scene
{
public:
	Scene();
	~Scene();
	void Update(float deltaTime);
	void Setup();
	void setMainCamera(int);
	int getMainCamera();

};

