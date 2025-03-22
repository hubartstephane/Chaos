#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#if _WIN32
#include <nlohmann/json.hpp>
#endif

#if _LINUX
#include <json.hpp>
#endif

int main(int argc, char ** argv)
{
	nlohmann::json j;
	j["name"] = 666.6f;
	j["truc"] = "my truc";

	std::cout << j.dump(2) << std::endl;

	return 0;
}
 