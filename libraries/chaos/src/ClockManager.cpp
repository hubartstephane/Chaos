#include <chaos/ClockManager.h>

namespace chaos
{

void ClockManager::AddClock(int clock_id, double in_time_scale, bool in_paused)
{
  Clock * clock = GetClock(clock_id);
  if (clock != nullptr)
  {
    clock->time_scale = in_time_scale;
    clock->paused = in_paused;
  }
  else
    clocks.insert(std::make_pair(clock_id, Clock(in_time_scale, in_paused)));
}

void ClockManager::RemoveClock(int clock_id)
{
  clocks.erase(clock_id);
}

void ClockManager::TickManager(double delta_time)
{
  if (delta_time != 0.0)
  {
    TickClock(delta_time);
    if (!paused && time_scale != 0.0)
      for (auto it : clocks)
        it.second.TickClock(delta_time * time_scale);
  }
}

void ClockManager::Clean()
{
  clocks.clear();
}
  
Clock * ClockManager::GetClock(int clock_id)
{
  auto it = clocks.find(clock_id);
  if (it != clocks.end())
    return &it->second;
  return nullptr;
}

Clock const * ClockManager::GetClock(int clock_id) const
{
  auto it = clocks.find(clock_id);
  if (it != clocks.cend())
    return &it->second;
  return nullptr;
}

}; // namespace chaos
