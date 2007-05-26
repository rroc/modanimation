#pragma once

#include <string>
#include <vector>

using namespace std;

class GLMenu
{
public:
	GLMenu(void);
	~GLMenu(void);

	void addMenuLine(string line);

	void draw();

	//attributes
private:
	std::vector<char*> lines;
};
