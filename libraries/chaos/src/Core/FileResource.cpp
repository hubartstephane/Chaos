#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void FileResource::SetPath(boost::filesystem::path const & in_path)
	{
		// shu49
		// si on test toute les redirections pour la date, on va se retrouver avec un timestamp qui ne correspond pas du tout au fichier considere


		FileTools::WithFile(in_path, [this](boost::filesystem::path const& p)
		{
			try
			{
				file_timestamp = boost::filesystem::last_write_time(p);
				return true;
			}
			catch (boost::filesystem::filesystem_error const &)
			{
			}
			return false;
		});
		path = in_path;
	}

}; // namespace chaos
