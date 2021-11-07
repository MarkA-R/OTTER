#pragma once
class Machine
{
public:
	bool isOnline();
	void setOnline(bool);
protected:
	bool online = false;
};

