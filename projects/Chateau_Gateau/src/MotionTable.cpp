#include "MotionTable.h"
#include <iostream>
template<typename T>
T Catmull(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{

	return 0.5f * (2.f * p1 + t * (-p0 + p2)
		+ t * t * (2.f * p0 - 5.f * p1 + 4.f * p2 - p3)
		+ t * t * t * (-p0 + 3.f * p1 - 3.f * p2 + p3));
}

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

float getDistance(glm::vec3 b, glm::vec3 a) {
	return sqrt(pow(abs(a.x - b.x), 2) + pow(abs(a.y - b.y), 2) + pow(abs(a.z - b.z), 2));
}

Entry::Entry(float a, float b, glm::vec3 c) {
	u = a;
	distance = b;
	position = c;
}

float Entry::getT()
{
	return u;
}

float Entry::getDistance()
{
	return distance;
}

glm::vec3 Entry::getPosition()
{
	return position;
}

MotionTable::MotionTable(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float totalD)
{
	float samples = 10;
	int index = 0;
	for (float i = 0.f; i <= 1.1f;) {
		//std::cout << "MT " << i << std::endl;
		//std::cout << i << std::endl;
		glm::vec3 currentPos = Catmull(p0, p1, p2, p3, i);
		if (entries.size() >= 1) {
			Entry previous = entries[index];
			//glm::vec3 currentPos = Catmull(p0, p1, p2, p3, i);
			//std::cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << std::endl;
			entries.push_back(Entry(i,( getDistance(previous.getPosition(), currentPos) + previous.getDistance()) + totalD, currentPos));
			index++;
		}
		else
		{
			//std::cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << std::endl;

			entries.push_back(Entry(i, totalD, currentPos));
		}
		i += (1.f / (samples));
		
	}
}

MotionTable::MotionTable(std::vector<glm::vec3> a)
{
	int samples = 20;
	float u = 0.f;
	int index = 0;
	int timesToWait = samples;
	int waitedTime = 0.f;
	int sampleIndex = 0;
	int p0 = a.size() - 1;
	int p1 = 0;
	int p2 = 1;
	int p3 = 2;
	//float timesToAdd = (1.f / (samples)) * a.size();
	for (float i = 0.f; i <= a.size();) {
		//std::cout << "MT " << i << std::endl;
		//std::cout << i << std::endl;
		//TODO:
		//make it one long curve. use the p0 and p3 stuff from the other movement update function.
		//instead of interating i = 0 i < all frames, just loop from currentframe to next frame +- 1 or something
		if (u > 1) {
			u -= 1;
			p1++;
			p2++;
		}
		
		
		p0 = p1 - 1;
		p3 = p2 + 1;
		if (p0 >= a.size()) {
			p0 = a.size() - 1;
		}
		if (p1 >= a.size()) {
			p1 = a.size() - 1;
		}
		if (p2 >= a.size()) {
			p2 = a.size() - 1;
		}
		if (p3 >= a.size()) {
			p3 = a.size() - 1;
		}
		
		
		
		
		//std::cout << p0 << " " << p1 << " " << p2 << " " << p3 << " --- " << u << std::endl;
		glm::vec3 currentPos = Catmull(a[p0], a[p1], a[p2], a[p3], u);
		if (entries.size() >= 1) {
			Entry previous = entries.back();
			//glm::vec3 currentPos = Catmull(p0, p1, p2, p3, i);
			//std::cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << std::endl;
			//std::cout << glm::distance(previous.getPosition(), currentPos) + previous.getDistance() << " " << sampleIndex << std::endl;
			entries.push_back(Entry(i, (glm::distance(currentPos,previous.getPosition() ) + previous.getDistance()), currentPos));
			//index++;
		}
		else
		{
			//std::cout << currentPos.x << " " << currentPos.y << " " << currentPos.z << std::endl;

			entries.push_back(Entry(i, 0, a[0]));
		}
		i += (1.f / (samples));
		u += (1.f / (samples));
		waitedTime++;
		sampleIndex++;

	}

	std::cout << "Motion table created with length of: " << entries.size() << std::endl;
}

MotionTable::MotionTable()
{
}



Entry MotionTable::getEntryUsingT(float u)
{
	Entry previousEntry = entries[0];
	Entry currentEntry = entries[0];
	float percent = 0.f;
	for each (Entry e in entries) {
		currentEntry = e;
		if (previousEntry.getT() <= u && u <= currentEntry.getT()) {
			break;
		}
		previousEntry = e;
	}
	percent = (u + previousEntry.getT()) / currentEntry.getT();
	return Entry(Lerp(previousEntry.getT(),currentEntry.getT(),percent), 
		Lerp(previousEntry.getDistance(), currentEntry.getDistance(), percent), 
		Lerp(previousEntry.getPosition(), currentEntry.getPosition(), percent));
}

Entry MotionTable::getEntryUsingDistance(float d)
{
	Entry previousEntry = entries[0];
	Entry currentEntry = entries[0];
	float percent = 0.f;
	for each (Entry e in entries) {
		currentEntry = e;
		std::cout << "PREV " << previousEntry.getDistance() << " CURR " << currentEntry.getDistance() << std::endl;
		if (previousEntry.getDistance() <= d && d <= currentEntry.getDistance()) {			
			break;
		}
		previousEntry = e;
	}
	percent = (d - previousEntry.getDistance()) / (currentEntry.getDistance() - previousEntry.getDistance());
	return Entry(Lerp(previousEntry.getT(), currentEntry.getT(), percent),
		Lerp(previousEntry.getDistance(), currentEntry.getDistance(), percent),
		Lerp(previousEntry.getPosition(), currentEntry.getPosition(), percent));
}

Entry MotionTable::getEntryUsingPoint(glm::vec3 p)
{
	Entry previousEntry = entries[0];
	Entry currentEntry = entries[0];
	float percent = 0.f;
	for each (Entry e in entries) {
		currentEntry = e;
		if (previousEntry.getPosition().x <= p.x && p.x <= currentEntry.getPosition().x) {
			if (previousEntry.getPosition().y <= p.y && p.y <= currentEntry.getPosition().y) {
				if (previousEntry.getPosition().z <= p.z && p.z <= currentEntry.getPosition().z) {
					break;
				}
			}
		
		}
		previousEntry = e;
	}
	percent = (p.x + previousEntry.getPosition().x) / currentEntry.getPosition().x;
	return Entry(Lerp(previousEntry.getT(), currentEntry.getT(), percent),
		Lerp(previousEntry.getDistance(), currentEntry.getDistance(), percent),
		Lerp(previousEntry.getPosition(), currentEntry.getPosition(), percent));
}

Entry MotionTable::getAtIndex(int i)
{
	return entries[i];
}

Entry MotionTable::getLast()
{
	return entries.back();
}

std::vector<Entry> MotionTable::getEntries()
{
	return entries;
}
