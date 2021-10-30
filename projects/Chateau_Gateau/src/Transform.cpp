#include "Transform.h"
#include <cmath>

bool isZero(float f) {
	if (f < 0.001 && f > -0.001) {
		return true;
	}
	return false;
}
Transform::Transform() {
	pos = glm::vec3(0);
	scale = glm::vec3(1);
	rotation = glm::vec4(getQuaternion(glm::vec3(0, 0, 1), glm::radians(1.f)));
}

void Transform::setPosition(glm::vec3 x)
{
	pos = x;
}

void Transform::addPosition(glm::vec3 x)
{
	pos = pos + x;
}

void Transform::addRotation(glm::vec4 x)//https://www.cprogramming.com/tutorial/3d/quaternions.html
{
	glm::vec4 a = rotation;

	rotation.w = ((a.w * x.w) - (a.x * x.x) - (a.y * x.y) - (a.z * x.z));
	rotation.x = ((a.w * x.x) - (a.x * x.w) - (a.y * x.z) - (a.z * x.y));
	rotation.y = ((a.w * x.y) - (a.x * x.z) - (a.y * x.w) - (a.z * x.x));
	rotation.z = ((a.w * x.z) - (a.x * x.y) - (a.y * x.x) - (a.z * x.w));

	
}

void Transform::setRotation(glm::vec4 x)
{
	rotation = x;
}


void Transform::setScale(glm::vec3 x)
{
	scale = x;
}

glm::vec3 Transform::getPosition()
{
	return pos;
}

glm::vec4 Transform::getRotation()
{
	return rotation;
}

glm::vec3 Transform::getScale()
{
	return scale;
}

glm::vec3 Transform::getEuler(glm::vec4 q)//code from: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
{//later
	double pi = 3.14159265359;
	glm::vec3 e = glm::vec3(0);
	double x1 = 2 * (q.w * q.x + q.y * q.z);
	double x2 = 1 - 2 * (q.x * q.x + q.y * q.y);
	e.x = std::atan2(x1, x2);

	double y1 = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(y1) >= 1) {
		e.y = std::copysign(pi / 2, y1);
	}
	else
	{
		e.y = std::asin(y1);
	}

	double z1 = 2 * (q.w * q.z + q.x * q.y);
	double z2 = 1 - 2 * (q.y * q.y + q.z * q.z);
	e.z = std::atan2(z1, z2);
	return e;
}

glm::vec4 Transform::getQuaternion(glm::vec3 axis, float angle)//WXYZ
{
	glm::vec4 q = glm::vec4(0);
	q.x = axis.x * sin(angle / 2);
	q.y = axis.y * sin(angle / 2);
	q.z = axis.z * sin(angle / 2);
	q.w = cos(angle / 2);
	return q;
}

//get axis and get angle are from https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
glm::vec3 Transform::getRotationAxis()
{
	glm::vec3 axis = glm::vec3(0);
	axis.x = rotation.x / sqrt(1 - rotation.w * rotation.w);
	axis.y = rotation.y / sqrt(1 - rotation.w * rotation.w);
	axis.z = rotation.z / sqrt(1 - rotation.w * rotation.w);
	return axis;
}

float Transform::getAxisAngle()//its in radians
{
	return 2* acos(rotation.w);
}

