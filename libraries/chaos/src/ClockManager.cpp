#include <chaos/ClockManager.h>

namespace chaos
{

ClockManager::~ClockManager()
{
  Clean();
}

Clock * ClockManager::AddClock(int clock_id, double in_time_scale, bool in_paused)
{
  assert(clock_id != 0); // ID 0 reserved

  Clock * clock = GetClock(clock_id);
  if (clock != nullptr)
  {
    clock->time_scale = in_time_scale;
    clock->paused     = in_paused;
  }
  else
  {
    clock = new Clock(in_time_scale, in_paused);
    clocks.insert(std::make_pair(clock_id, clock));
  }
  return clock;    
}

void ClockManager::RemoveClock(int clock_id)
{
  assert(clock_id != 0); // ID 0 reserved

  auto it = clocks.find(clock_id);
  if (it != clocks.end())
  {
    delete(it->second);  
    clocks.erase(it);
  }
}

void ClockManager::TickManager(double delta_time)
{
  if (delta_time != 0.0)
  {
    TickClock(delta_time);
    if (!paused && time_scale != 0.0)
      for (auto it : clocks)
        it.second->TickClock(delta_time * time_scale);
  }
}

void ClockManager::Clean()
{
  for (auto it : clocks)
    delete(it.second);
  clocks.clear();
}
  
Clock * ClockManager::GetClock(int clock_id)
{
  // ID == 0  for SELF
  if (clock_id == 0)
    return this;

  auto it = clocks.find(clock_id);
  if (it != clocks.end())
    return it->second;
  return nullptr;
}

Clock const * ClockManager::GetClock(int clock_id) const
{
  // ID == 0  for SELF
  if (clock_id == 0)
    return this;

  auto it = clocks.find(clock_id);
  if (it != clocks.cend())
    return it->second;
  return nullptr;
}

}; // namespace chaos
