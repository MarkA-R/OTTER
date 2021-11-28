#pragma once
#include "NOU/Entity.h"
using namespace nou;
struct Entry {
public:
	float getT();
	float getDistance();
	glm::vec3 getPosition();
	Entry(float, float, glm::vec3);
private:
	
	
	
	float u;
	float distance;//distance along curve, not from origin
	glm::vec3 position;
};
class MotionTable
{
protected:
	std::vector<Entry> entries;

public:
	MotionTable(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float);
	//interpolates entries based on a given value, does not return actual entries
	Entry getEntryUsingT(float);
	Entry getEntryUsingDistance(float);
	Entry getEntryUsingPoint(glm::vec3);
	Entry getAtIndex(int);
	Entry getLast();
	std::vector<Entry> getEntries();

	
};


