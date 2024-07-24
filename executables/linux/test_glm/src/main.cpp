#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>

int main(int argc, char ** argv)
{
	glm::vec3 v = {1.0f, 2.0f, 3.0f};
	v = v + v * 6.0f;
	
	std::cout << "[" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
	return 0;
}
 