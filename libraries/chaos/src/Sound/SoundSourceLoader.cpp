#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	SoundSource * SoundSourceLoader::LoadObject(char const * name, nlohmann::json const * json) const
	{
		SoundSource * result = nullptr;
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p);
			result = LoadObject(path, name);
			if (result != nullptr)
				result->InitializeFromJSON(json);
		}
		return result;
	}

	SoundSource * SoundSourceLoader::LoadObject(FilePathParam const & path, char const * name) const
	{
		return LoadObjectHelper(path, name, [this](FilePathParam const & path)
		{
			return GenSourceObject(path);
		},
		[this](SoundSource* source)
		{
			manager->sources.push_back(source);
		});
	}

	SoundSource * SoundSourceLoader::GenSourceObject(FilePathParam const & path) const
	{
		// get the irrklang engine
		irrklang::ISoundEngine * engine = manager->GetIrrklangEngine();
		if (engine == nullptr)
			return nullptr;

		// load the file
		boost::filesystem::path const& resolved_path = path.GetResolvedPath();

		Buffer<char> buffer = FileTools::LoadFile(path, LoadFileFlag::NoErrorTrace);
		if (buffer == nullptr)
		{
			SoundLog::Error("SoundSourceLoader::GenSourceObject: fail to load [%s]", resolved_path.string().c_str());
			return nullptr;
		}
		// create the source on irrklang side
		// XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
		shared_ptr<irrklang::ISoundSource> irrklang_source = engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, resolved_path.string().c_str(), true);
		if (irrklang_source == nullptr)
			return nullptr;
		// insert the result
		SoundSource * result = new SoundSource();
		if (result == nullptr)
			return nullptr;
		// last initializations
		result->sound_manager = manager;
		result->irrklang_source = irrklang_source;

		return result;
	}

	bool SoundSourceLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager != nullptr && manager->FindSourceByPath(path) != nullptr);
	}

	bool SoundSourceLoader::IsNameAlreadyUsedInManager(ObjectRequest request) const
	{
		return (manager != nullptr && manager->FindSource(request) != nullptr);
	}

}; // namespace chaos