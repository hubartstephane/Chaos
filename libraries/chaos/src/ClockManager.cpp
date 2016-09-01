#include <chaos/ClockManager.h>

namespace chaos
{

ClockManager::~ClockManager()
{
  Clean();
}

int ClockManager::FindUnusedID() const
{
  // search bigger and smaller index (no memory allocation)
  int smaller = std::numeric_limits<int>::max();
  int bigger  = 0;
  for (Clock const * clock : clocks)
  {
    smaller = min(smaller, clock->clock_id);
    bigger  = max(bigger, clock->clock_id);
  }
  if (smaller > 1)
    return smaller - 1;
  if (bigger < std::numeric_limits<int>::max())
    return bigger + 1;

  // second pass require memory allocation
  std::vector<int> IDs;
  for (Clock const * clock : clocks)
    IDs.push_back(clock->clock_id);
  std::sort(IDs.begin(), IDs.end()); // store all ID's

  for (size_t i = 0 ; i < IDs.size() - 1 ; ++i)
    if (IDs[i] + 1 < IDs[i + 1]) // search for holes in the ID list
      return IDs[i] + 1;

  return -1;
}

Clock * ClockManager::AddClock(int clock_id, double in_time_scale, bool in_paused)
{
  assert(clock_id != 0); // ID 0 reserved

  if (GetClock(clock_id) != nullptr) // cannot add a clock whose ID is already i used
    return nullptr;

  if (clock_id < 0) // generate a own ID if necessary
  {
    clock_id = FindUnusedID();
    if (clock_id < 0)
      return nullptr;
  }

  Clock * clock = new Clock(); // allocate the clock
  if (clock != nullptr)
  {
    clock->time_scale = in_time_scale;
    clock->paused     = in_paused;
    clock->clock_id   = clock_id;
    clocks.push_back(clock);
  }
  return clock;
}

bool ClockManager::RemoveClock(int clock_id)
{
  assert(clock_id != 0); // ID 0 reserved

  auto it = std::find_if(
    clocks.begin(), clocks.end(), 
    [clock_id](Clock * clock) -> bool 
      {
        return clock->GetClockID() == clock_id;
      }
  );
  if (it != clocks.end())
  {
    delete(*it);  
    clocks.erase(it);
    return true;
  }
  return false;
}

bool ClockManager::TickManager(double delta_time)
{
  if (!TickClock(delta_time))
    return false;
  for (Clock * clock : clocks)
    clock->TickClock(delta_time * time_scale);
  return true;
}

void ClockManager::Clean()
{
  for (Clock * clock : clocks)
    delete(clock);
  clocks.clear();
}
  
Clock * ClockManager::GetClock(int clock_id)
{
  // ID == 0  for SELF
  if (clock_id == 0)
    return this;

  // find the clock by ID
  for (Clock * clock : clocks)
    if (clock->clock_id == clock_id)
      return clock;
  return nullptr;
}

Clock const * ClockManager::GetClock(int clock_id) const
{
  // ID == 0  for SELF
  if (clock_id == 0)
    return this;

  // find the clock by ID
  for (Clock const * clock : clocks)
    if (clock->clock_id == clock_id)
      return clock;
  return nullptr;
}

}; // namespace chaos
