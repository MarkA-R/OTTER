#pragma once
#include "globalHeaders.h"
class Camera
{
public:
	Camera();
	~Camera();
	void setPosition(glm::vec3);
	void setNormal(glm::vec3);
	void lookAt(glm::vec3);
	void setUp(glm::vec3);
	void setWindowSize(int, int);
	void setFOVRadians(float);
	void setOrthographicVerticalScale(float);
	void setOrthographicMode(bool);
	glm::vec3 getPosition();
	glm::vec3 getNormal();
	glm::vec3 getUp();
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getViewProjection();
	float getOrthographicVerticalScale();
	bool isOrthoEnabled();


private:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 up;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 viewProjection;
	float nearPlane;
	float farPlane;
	float aspect;
	float orthographicVerticalScale;
	float fov;
	bool isOrthographic;
	bool isDirty;
	void calculateProjection();
	void calculateView();


};

