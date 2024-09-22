#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int main(int argc, char ** argv)
{
	Assimp::Importer importer;
	
	aiScene const * g_scene = importer.ReadFile("wrong_filename", aiProcessPreset_TargetRealtime_Quality);

	std::cout << "test assimp" << std::endl;
	
	return 0;
}
 