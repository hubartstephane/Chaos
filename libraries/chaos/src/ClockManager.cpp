#include <chaos/ClockManager.h>

namespace chaos
{  
	Clock::Clock(ClockCreateParams const & params) : 
		time_scale(params.time_scale), 
		paused(params.paused)
	{

	}

	Clock::~Clock()
	{
		// after destructor, the children vector will be cleared
		// the problem is that children may survive to their parent death due to reference count
		// so orphan all children to ensure they cannot access parent's anymore
		size_t child_count = children_clocks.size();
		for (size_t i = 0; i < child_count; ++i)
			children_clocks[i]->parent_clock = nullptr;	
		// same thing with events
		size_t event_count = pending_events.size();
		for (size_t i = 0; i < event_count; ++i)
			pending_events[i]->clock = nullptr;
	}

	bool Clock::IsDescendantClock(Clock const * child_clock) const
	{
		assert(child_clock != nullptr);

		// consider that a clock is it own child !!
		if (child_clock == this)
			return true;
		// recurse
		size_t count = children_clocks.size();
		for (size_t i = 0; i < count; ++i)
			if (children_clocks[i]->IsDescendantClock(child_clock))
				return true;
		return false;
	}

	Clock * Clock::GetChildClock(int id, bool recursive)
	{
		Clock * result = nullptr;
		if (id > 0)  // no clock with negative ID !
		{
			if (clock_id == id)
				result = this;
			else if (recursive)
			{
				size_t count = children_clocks.size();
				for (size_t i = 0; (i < count) && (result == nullptr); ++i)
					result = children_clocks[i]->GetChildClock(id, recursive);
			}		
		}
		return result;
	}

	Clock const * Clock::GetChildClock(int id, bool recursive) const
	{
		Clock const * result = nullptr;
		if (id > 0)  // no clock with negative ID !
		{
			if (clock_id == id)
				result = this;
			else if (recursive)
			{
				size_t count = children_clocks.size();
				for (size_t i = 0; (i < count) && (result == nullptr); ++i)
					result = children_clocks[i]->GetChildClock(id, recursive);
			}		
		}
		return result;
	}

	Clock * Clock::GetTopLevelParent()
	{
		Clock * result = this;
		while (result->parent_clock != nullptr)
			result = result->parent_clock;
		return result;
	}

	Clock const * Clock::GetTopLevelParent() const
	{
		Clock const * result = this;
		while (result->parent_clock != nullptr)
			result = result->parent_clock;
		return result;
	}

	void Clock::TriggerClockEvents(std::vector<ClockEvent *> & clock_events)
	{

	}

	bool Clock::TickClock(double delta_time) // should only be called on TopLevel node (public interface)
	{
		assert(parent_clock == nullptr);

		std::vector<ClockEvent *> clock_events;

		bool result = TickClockImpl(delta_time, clock_events);
		TriggerClockEvents(clock_events);		
		return result;
	}

	bool Clock::TickClockImpl(double delta_time, std::vector<ClockEvent *> & clock_events) // protected interface
	{
		// internal tick
		if (paused || time_scale == 0.0)
			return false;
		clock_time = clock_time + time_scale * delta_time;
		// register all events in the list



		// recursive click
		size_t count = children_clocks.size();
		for (size_t i = 0; i < count ; ++i)
			children_clocks[i]->TickClockImpl(time_scale * delta_time, clock_events);

		return true;
	}

	Clock * Clock::CreateChildClock(int id, ClockCreateParams const & params)
	{
		if (id > 0 && GetTopLevelParent()->GetChildClock(id, false) != nullptr) // cannot add a clock whose ID is already in use
			return nullptr;

		if (id < 0) // generate a own ID if necessary
		{
			id = GetTopLevelParent()->FindUnusedID(true);
			if (id < 0)
				return nullptr;
		}

		boost::intrusive_ptr<Clock> child_clock = boost::intrusive_ptr<Clock>(new Clock(params)); // allocate the clock
		if (clock != nullptr)
		{
			child_clock->clock_id = id;
			child_clock->parent_clock = this;
			children_clocks.push_back(child_clock);
		}   
		return child_clock.get();
	}

	bool Clock::RemoveFromParent()
	{
		Clock * tmp = parent_clock;
		if (tmp != nullptr) 
		{
			size_t count = tmp->children_clocks.size();
			for (size_t i = 0; i < count; ++i)
			{
				if (tmp->children_clocks[i].get() == this) // remove swap
				{
					if (i != count - 1) // nothing to do if already last element
						tmp->children_clocks[i] = std::move(tmp->children_clocks.back());							

					parent_clock = nullptr;          // XXX : we cannot invert these 2 lines because, because 'this' could be deleted and then
					tmp->children_clocks.pop_back(); //       and then we would access 'parent_clock' member after destructor
					return true;                     //       that's why we are using 'tmp'
				}
			}		
		}
		return false;
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
				smaller_id = min(smaller_id, clock->clock_id);
				bigger_id = max(bigger_id, clock->clock_id);
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

	int Clock::FindUnusedID(bool recursive) const
	{
		// STEP 1 : fast answer, with no vector usage
		//          search bigger and smaller ids (no memory allocation)
		//          if max and min values found are not MAX/MIN INTEGER values, just use increment/decrement values
		int smaller_id = (clock_id > 0) ? clock_id : std::numeric_limits<int>::max();
		int bigger_id  = (clock_id > 0) ? clock_id : 0;

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
		for (size_t i = 0 ; i < count - 1 ; ++i)
			if (IDs[i] + 1 < IDs[i + 1]) // search for holes in the ID list
				return IDs[i] + 1;

		return -1;
	}


	bool Clock::AddPendingEvent(ClockEvent * clock_event, ClockEventInfo event_info, bool relative_time)
	{
		assert(clock_event != nullptr);

		// event already registered ?
		if (clock_event->clock != nullptr)
			return false;
		// is it too late for that event ?
		if (event_info.IsTooLateFor(clock_time))
			return false;
		// do the registration
		clock_event->clock = this;
		clock_event->event_info = event_info;
		pending_events.push_back(clock_event);

		return true;
	}

	bool ClockEvent::RemoveFromClock()
	{
		Clock * tmp = clock; // keep a trace of parent
		if (tmp != nullptr)
		{
			size_t count = tmp->pending_events.size();
			for (size_t i = 0; i < count; ++i)
			{
				if (tmp->pending_events[i].get() == this)
				{
					if (i != count - 1)
						tmp->pending_events[i] = std::move(tmp->pending_events.back());

					clock = nullptr;                // XXX : we cannot invert these 2 lines because this could be destroyed 
					tmp->pending_events.pop_back(); // and we then would access 'clock' member after destructor 
					return true;                    // that's why we are using 'tmp'
				}		                            
			}		
		}			
		return false;
	}


}; // namespace chaos
