#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	FIBITMAPDeleter::FIBITMAPDeleter(bool in_release) : release(in_release)
	{
	}

	void FIBITMAPDeleter::operator ()(FIBITMAP* bitmap)
	{
		if (release)
			FreeImage_Unload(bitmap);
	}

	FIMULTIBITMAPDeleter::FIMULTIBITMAPDeleter(bool in_release) : release(in_release)
	{
	}

	void FIMULTIBITMAPDeleter::operator ()(FIMULTIBITMAP* multi_bitmap)
	{
		if (release)
			FreeImage_CloseMultiBitmap(multi_bitmap);
	}

}; // namespace chaos

