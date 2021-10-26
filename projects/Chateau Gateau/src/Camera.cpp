#include "Camera.h"
#include "globalHeaders.h"
Camera::Camera() {
	pos = glm::vec3(0.f,0.f,0.f);
	 normal = glm::vec3(0.f, 0.f, 1.f);
	 up = glm::vec3(0.0f, 1.0f, 0.0f);
	 view = glm::mat4(1.0f);
	 projection = glm::mat4(1.0f);
	 viewProjection = glm::mat4(1.0f);
	 nearPlane = 0.001f;
	 farPlane = 100.f;
	 aspect = 16/9;
	 orthographicVerticalScale = 1.f;
	 fov = glm::degrees(60.f);
	 isOrthographic = false;
	 isDirty = true;
}
Camera::~Camera() {

}

void Camera::setPosition(glm::vec3 x)
{
	pos = x;
}

void Camera::setNormal(glm::vec3 x)
{
	normal = x;
}

void Camera::lookAt(glm::vec3 x)
{
	normal = glm::normalize(x - pos);
}

void Camera::setUp(glm::vec3 x)
{
	up = x;
}

void Camera::setWindowSize(int x, int y)
{
	aspect = x / y;
}

void Camera::setFOVRadians(float x)
{
	fov = x;
}

void Camera::setOrthographicVerticalScale(float x)
{
	orthographicVerticalScale = x;
}

void Camera::setOrthographicMode(bool x)
{
	isOrthographic = x;
}

glm::vec3 Camera::getPosition()
{
	return pos;
}

glm::vec3 Camera::getNormal()
{
	return normal;
}

glm::vec3 Camera::getUp()
{
	return up;
}

glm::mat4 Camera::getView()
{
	return view;
}

glm::mat4 Camera::getProjection()
{
	return projection;
}

glm::mat4 Camera::getViewProjection()
{
	isDirty = false;
	return projection * view;
}

float Camera::getOrthographicVerticalScale()
{
	return orthographicVerticalScale;
}

bool Camera::isOrthoEnabled()
{
	return isOrthographic;
}

void Camera::calculateProjection()
{
	if (isOrthographic) {
		float w = orthographicVerticalScale * aspect;
		float h = orthographicVerticalScale / 2;
		projection = glm::ortho(-w, w, -h, h, nearPlane, farPlane);
	}
	else
	{
		projection = glm::perspective(fov, aspect, nearPlane, farPlane);
	}
	isDirty = true;
}

void Camera::calculateView()
{
	view = glm::lookAt(pos, pos + normal, up);
	isDirty = true;
}
