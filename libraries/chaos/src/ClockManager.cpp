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

  void ClockEvent::NextExecution()
  {
    ++execution_count;
    tick_count = 0;
    event_info.start_time = event_info.frequency;
  }

  void ClockEvent::Tick(ClockEventTickData const & tick_data)
  {
    ClockEventTickResult result = TickImpl(tick_data);

    // want to continue to next frame
    if (result.IsContinued())
    {
      ++tick_count;
      return;
    }
    // want to finish the event
    if (result.IsCompleted())
    {
      if (event_info.IsRepeatedInfinitly())
        NextExecution();
      else if (execution_count++ < event_info.repetition_count)
        NextExecution();
      else
        RemoveFromClock();
      return;
    }


   

#if 0
    ClockEventInfo const & event_info = clock_event->GetEventInfo();

    if (tick_result.IsContinued())
      ++clock_event->tick_count;
    else if (tick_result.IsCompleted())
      clock_event->NextRepetition();

    {
      if (event_info.IsRepeated())





        registered_event.clock_event->RemoveFromClock();
    }
    else if (tick_result.IsRestarted())
    {



    }
#endif
  }

	void Clock::TriggerClockEvents(ClockEventTickSet & events_set)
	{
    for (ClockEventTickRegistration const & registered_event : events_set)
    {
      ClockEvent * clock_event = registered_event.clock_event.get();

      // degenerated use case :
      //   the processing of a previous event remove another event from execution
      if (clock_event == nullptr)
        continue;

      // compute the tick information & tick
      ClockEventTickData tick_data;
      tick_data.time1 = registered_event.time1;
      tick_data.time2 = registered_event.time2;
      tick_data.delta_time = registered_event.delta_time;

      clock_event->Tick(tick_data);     
    }
	}

	bool Clock::TickClock(double delta_time) // should only be called on TopLevel node (public interface)
	{
		assert(parent_clock == nullptr);

    ClockEventTickSet event_tick_set;

		bool result = TickClockImpl(delta_time, 1.0, event_tick_set);
		TriggerClockEvents(event_tick_set);
		return result;
	}

	bool Clock::TickClockImpl(double delta_time, double cumulated_factor, ClockEventTickSet & event_tick_set) // protected interface
	{
		// internal tick
		if (paused || time_scale == 0.0)
			return false;		
		// register all events in the list
    double old_clock_time = clock_time;
    clock_time = clock_time + time_scale * delta_time;

    double time1 = old_clock_time;
    double time2 = clock_time;
    for (size_t i = 0; i < pending_events.size(); ++i)
    {
      ClockEventInfo const & event_info = pending_events[i]->GetEventInfo();

      if (event_info.IsTooLateFor(clock_time))
      {
        pending_events[i]->RemoveFromClock(); // XXX : we know RemoveFromClock = "RemoveReplace"
        --i;
      }
      else if (event_info.IsInsideRange(old_clock_time, clock_time))
      {
        ClockEventTickRegistration registration;
        registration.clock_event = pending_events[i];
        registration.time1 = old_clock_time;
        registration.time2 = clock_time;
        registration.delta_time = delta_time;
        
        if (event_info.start_time <= time1)
          registration.abs_time_to_start = 0.0;
        else
          registration.abs_time_to_start = (event_info.start_time - time1) * cumulated_factor;

        event_tick_set.insert(registration);
      }
		}
		// recursive tick 
		size_t child_count = children_clocks.size();
		for (size_t i = 0; i < child_count ; ++i)
			children_clocks[i]->TickClockImpl(time_scale * delta_time, cumulated_factor * time_scale, event_tick_set);

		return true;
	}

	Clock * Clock::CreateChildClock(int id, ClockCreateParams const & params)
	{
    // want an ID : automatic or user-defined
    if (id != 0) 
    {
      Clock * top_level_clock = GetTopLevelParent();

      // user-defined
      if (id > 0 && top_level_clock->GetChildClock(id, false) != nullptr) // cannot add a clock whose ID is already in use
        return nullptr;
      // generate a own ID if necessary
      else 
      {
        assert(id < 0);
        
        id = top_level_clock->FindUnusedID(true);
        if (id < 0)
          return nullptr;
      }
    }

    // allocate the clock
		boost::intrusive_ptr<Clock> child_clock = boost::intrusive_ptr<Clock>(new Clock(params)); 
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
		// make the event_info, relative to current_time if necessary
    if (relative_time)
      event_info.start_time += clock_time;
    // is it too late for that event ?
		if (event_info.IsTooLateFor(clock_time))
			return false;
		// do the registration
		clock_event->clock = this;
		clock_event->event_info = event_info;
    clock_event->tick_count = 0;
    clock_event->execution_count = 0;
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
