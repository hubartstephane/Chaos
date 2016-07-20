#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>

namespace chaos
{

/**
 * DoubleZone : testing if a point enters or exits to trigger an action can be tedious if the 
 *              point change its zone too often. DoubleZone helps avoiding this issue by using
 *              using 2 zones
 */
  
template<typename ZONE_TYPE> 
class DoubleZone
{
public:

  /** the type of the zone */
  typedef ZONE_TYPE zone_type;
  /** the type of the vector */
  typedef typename ZONE_TYPE::vec_type vec_type;
  
  /** point is inside inner zone */
  static int const NEAR_ZONE   = 0;
  /** point is betwen inner_zone and outer_zone */
  static int const MIDDLE_ZONE = 1;
  /** point is outside outer_zone */
  static int const FAR_ZONE    = 2; 

  /** no zone of change */
  static int const NOCHANGE_ZONE = 0;
  /** was not in the near zone, and then enters in it */
  static int const ENTER_ZONE    = 1; 
  /** was not in the far zone and enters in it */
  static int const LEAVE_ZONE    = 2; 

  /** returns the zone where is the point */
  template<typename T>
  int GetPointZone(vec_type const & p) const
  {
    if (!inside(p, outer_zone))
      return FAR_ZONE;
    if (!inside(p, inner_zone))
      return MIDDLE_ZONE;    
    return NEAR_ZONE;   
  }

  /** returns the point zone changement */
  template<typename>
  int CheckZoneChange(vec_type const & before, vec_type const & after) const
  {
    int before_zone = GetPointZone(before);
    int after_zone  = GetPointZone(after);
    return CheckZoneChange(before_zone, after_zone);
  }

  /** returns the point zone changement */
  template<typename>
  static int CheckZoneChange(int before, int after)
  {  
    if (before_zone != after_zone)
    {
      if (after_zone == NEAR_ZONE)
        return ENTER_ZONE;  
      if (after_zone == FAR_ZONE)
        return LEAVE_ZONE;        
    }
    return NOCHANGE_ZONE;  
  }

protected:

  /** the inner zone */
  zone_type inner_zone;
  /** the outer zone */
  zone_type outer_zone;
};

}; // namespace chaos
