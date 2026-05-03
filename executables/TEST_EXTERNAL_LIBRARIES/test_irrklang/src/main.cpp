#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem/path.hpp>

#include <irrKlang.h>

using namespace irrklang;

int main(int argc, char ** argv)
{
	if (argc == 0)
	{
		std::cerr << "Missing arguments" << std::endl;
		return -1;
	}

	ISoundDeviceList * irrklang_devices = irrklang::createSoundDeviceList();
	if (irrklang_devices != nullptr)
	{
		for (int i = 0; i < irrklang_devices->getDeviceCount(); ++i)
			std::cout << "device[" << i << "]: " << irrklang_devices->getDeviceDescription(i) << std::endl;
		irrklang_devices->drop();
	}

	ISoundEngine* engine = createIrrKlangDevice();
	if (engine == nullptr)
	{
		std::cerr << "failed to create source ISoundEngine" << std::endl;
		return -1;
	}
	else
	{
		boost::filesystem::path application_path = argv[0];
		boost::filesystem::path application_directory = application_path.parent_path();
		boost::filesystem::path file_path = application_directory / "resources" / "sound.ogg";

		std::ifstream file(file_path.c_str(), std::ios::binary);
		if (file)
		{
			std::streampos start = file.tellg();
			file.seekg(0, std::ios::end);
			std::streampos end = file.tellg();
			file.seekg(0, std::ios::beg);

			size_t file_size = (size_t)(end - start);

			char* buffer = new char[file_size];
			if (buffer != nullptr)
			{
				file.read(buffer, file_size);

				ISoundSource* source = engine->addSoundSourceFromMemory(buffer, (ik_s32)file_size, "sound.ogg", true);
				if (source == nullptr)
				{
					std::cerr << "failed to create ISoundSource for: " << file_path.string().c_str() << std::endl;
				}
				else
				{
					ISound* sound = engine->play2D(
						source, 
						false,  // loop
						false,  // start paused
						true);  // track
					if (sound == nullptr)
					{
						std::cerr << "failed to create ISound" << std::endl;
					}
					else
					{
						while (!sound->isFinished())
						{
						}
						sound->drop();
					}
					engine->removeSoundSource(source); // source->drop() crashes
				}
				delete [] buffer;
			}
		}
		engine->drop();
	}
	return 0;
}
 