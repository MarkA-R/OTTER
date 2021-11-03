#include "Raycast.h"
#include <iostream>
Raycast::Raycast(glm::vec3 Origin, glm::vec3 Direction, float d, float s)
{
	distance = d;
	step = s;
	origin = Origin;
	direction = Direction;
}

std::vector<glm::vec3> Raycast::crossedPoints()
{
	std::vector<glm::vec3> crossed = std::vector<glm::vec3>();
	float totalSteps = 0;
	int timeoutCounter = 0;
	while (totalSteps <= distance)
	{
		totalSteps += step;
		timeoutCounter++;
		if (timeoutCounter > 500) {
			std::cout << "Raycast timed out. ORIGIN: (" << origin.x << "," << origin.y << "," << origin.z << ") DIRECTION: (" << direction.x << "," << direction.y << "," << direction.z << ")" << std::endl;
			break;
		}
		crossed.push_back(glm::vec3(origin.x + (direction.x *totalSteps), origin.y + (direction.y * totalSteps), origin.z + (direction.z * totalSteps)));
	}
	return crossed;
}
