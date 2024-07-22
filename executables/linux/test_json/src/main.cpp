#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <json.hpp>

int main(int argc, char ** argv)
{
	nlohmann::json j;
	j["name"] = 666.6f;
	j["truc"] = "my truc";

	std::cout << j.dump(2) << std::endl;

	return 0;
}
 