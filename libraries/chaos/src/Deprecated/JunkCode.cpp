#include "chaos/ChaosPCH.h"
// JunkCode.cpp is not intended to be compiled at all
//              it is just a place where to put code or algorithm i do not want to be lost
//
#if 0



if (msg == WM_INPUTLANGCHANGE)
{

	HKL hkl = (HKL)lParam;
	// LANGIDs are deprecated. Use BCP 47 locale names where possible.
	LANGID langId = LOWORD(HandleToUlong(hkl));

	WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
	LCIDToLocaleName(MAKELCID(langId, SORT_DEFAULT), localeName, LOCALE_NAME_MAX_LENGTH, 0);

	// Get the ISO abbreviated language name (for example, "eng").
	WCHAR lang[9];
	GetLocaleInfoEx(localeName, LOCALE_SISO639LANGNAME2, lang, 9);

	// Get the keyboard layout identifier (for example, "00020409" for United States-International keyboard layout)
	WCHAR keyboardLayoutId[KL_NAMELENGTH];
	GetKeyboardLayoutNameW(keyboardLayoutId);
}






#if 0

GLuint vertex_array = 0;
GLuint query = 0;
GLuint renderbuffer = 0;
GLuint framebuffer = 0;

WithWindowContext(shared_context, [&vertex_array, &query, &renderbuffer, &framebuffer]()
{
	glCreateVertexArrays(1, &vertex_array);
	glCreateQueries(GL_SAMPLES_PASSED, 1, &query);
	glCreateRenderbuffers(1, &renderbuffer);
	glCreateFramebuffers(1, &framebuffer);
});

Window* w = CreateTypedWindow(main_window_class, window_params, window_hints);

WithWindowContext(w->GetGLFWHandler(), [vertex_array, &query, &renderbuffer, &framebuffer]()
{
	bool b0 = glIsVertexArray(vertex_array);  // false
	bool b1 = glIsQuery(query);               // false
	bool b2 = glIsRenderbuffer(renderbuffer); // true
	bool b3 = glIsFramebuffer(framebuffer);   // true

	query = query;
});

#endif





















// =======================================================
// Finding an unused ID
// =======================================================

int Clock::FindUnusedID(bool recursive) const
{
	// STEP 1 : fast answer, with no vector usage
	//          search bigger and smaller ids (no memory allocation)
	//          if max and min values found are not MAX/MIN INTEGER values, just use increment/decrement values
	int smaller_id = (clock_id > 0) ? clock_id : std::numeric_limits<int>::max();
	int bigger_id = (clock_id > 0) ? clock_id : 0;

	FindUnusedIDStep1(smaller_id, bigger_id, recursive);
	if (bigger_id < std::numeric_limits<int>::max())   // XXX : try BIGGER first because ID will increment : 1, 2, 3 ..
		return bigger_id + 1;                            // if you try SMALLER first , you'll get MAX - 1 (that will later block BIGGER)
	if (smaller_id > 1)
		return smaller_id - 1;

	// STEP 2 : store all ID's in a vector
	//          sort the vector
	//          detect a 'hole' inside the vector an use it
	//          => require memory allocation
	std::vector<int> IDs;
	if (clock_id > 0)
		IDs.push_back(clock_id);
	FindUnusedIDStep2(IDs, recursive); // store all ID's
	std::sort(IDs.begin(), IDs.end());

	size_t count = IDs.size();
	for (size_t i = 0; i < count - 1; ++i)
		if (IDs[i] + 1 < IDs[i + 1]) // search for holes in the ID list
			return IDs[i] + 1;

	return -1;
}

void Clock::FindUnusedIDStep1(int & smaller_id, int & bigger_id, bool recursive) const // iteration over MIN & MAX ID's
{
	size_t count = children_clocks.size();
	for (size_t i = 0; i < count; ++i)
	{
		if (smaller_id == 1 && bigger_id == std::numeric_limits<int>::max()) // no need to continue STEP1 recursion, we can say that there
			return;                                                            // will not be a fast answer, have to go STEP 2

		Clock const * clock = children_clocks[i].get(); // ignore clock with "NO" id
		if (clock->clock_id > 0)
		{
			smaller_id = std::min(smaller_id, clock->clock_id);
			bigger_id = std::max(bigger_id, clock->clock_id);
		}
		if (recursive)
		{
			clock->FindUnusedIDStep1(smaller_id, bigger_id, recursive);
		}
	}
}

void Clock::FindUnusedIDStep2(std::vector<int> & IDs, bool recursive) const
{
	size_t count = children_clocks.size();
	for (size_t i = 0; i < count; ++i)
	{
		Clock const * clock = children_clocks[i].get();
		if (clock->clock_id > 0)
			IDs.push_back(clock->clock_id); // store only 'REAL' id's
		if (recursive)
			clock->FindUnusedIDStep2(IDs, recursive);
	}
}



#endif
