#include <chaos/GeometryFramework.h>

namespace chaos
{

template<typename T>
type_circle<T> type_box2<T>::GetBoundingCircle() const
{
  return IsEmpty()? circle() : circle(position, glm::length(half_size));
}

template<typename T>
type_circle<T> type_box2<T>::GetInnerCircle() const
{
  return IsEmpty()? circle() : circle(position, GetMinComponent(half_size));
}

template<typename T>
type_box2<T> GetSplitBox(type_box2<T> const & b, int i, int j)
{
  typedef typename type_box2<T>::vec2_type vec2_type;

  assert((i == 0) || (i == 1));
  assert((j == 0) || (j == 1));
  i = (i << 1) - 1;  
  j = (j << 1) - 1;
  type_box2<T> new_half_size = b.half_size / static_cast<T>(2);

  return type_box2<T>(
    b.position + new_half_size * vec2_type(static_cast<T>(i), static_cast<T>(j)), 
    new_half_size);
}

template<typename T>
T GetBoxAspect(type_box2<T> const & b)
{
  return (b.half_size.y)? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
}

template<typename T>
type_box2<T> operator & (const type_box2<T> & b1,const type_box2<T> & b2)
{
  typedef typename type_box2<T>::vec2_type vec2_type;

  if (b1.IsEmpty() || b2.IsEmpty())
    return type_box2<T>();
 
  vec2_type p1 = b1.position + b1.half_size;
  vec2_type p2 = b2.position - b2.half_size;

  if (p1.x <= p2.x || p1.y <= p2.y)
    return type_box2<T>();

  vec2_type p3 = b1.position - b1.half_size;
  vec2_type p4 = b2.position + b2.half_size;

  if (p4.x <= p3.x || p4.y <= p3.y)
    return type_box2<T>();

  vec2_type a = glm::min(p1, p4);
  vec2_type b = glm::max(p3, p2);

  return type_box2<T>((a + b) / static_cast<T>(2), (b - a) / static_cast<T>(2));
}

template<typename T>
type_box2<T> operator | (const type_box2<T> & b1,const type_box2<T> & b2)
{
  typedef typename type_box2<T>::vec2_type vec2_type;

  if (b1.IsEmpty())
    return b2;
  if (b2.IsEmpty())
    return b1;

  vec2_type p1 = b1.position + b1.half_size;
  vec2_type p2 = b2.position + b2.half_size;

  vec2_type p3 = b1.position - b1.half_size;
  vec2_type p4 = b2.position - b2.half_size;

  vec2_type a = glm::max(p1, p2);
  vec2_type b = glm::min(p3, p4);

  return type_box2<T>((a + b) / static_cast<T>(2), (b - a) / static_cast<T>(2));
}

// ==============================================================================================
// box3 methods
// ==============================================================================================

template<typename T>
type_sphere<T> type_box3<T>::GetBoundingSphere() const
{
  return IsEmpty()? sphere() : sphere(position, glm::length(half_size));
}

template<typename T>
type_sphere<T> type_box3<T>::GetInnerSphere() const
{
  return IsEmpty()? sphere() : sphere(position, GetMinComponent(half_size));
}

template<typename T>
type_box3<T> GetSplitBox(type_box3<T> const & b, int i, int j, int k)
{
  typedef typename type_box3<T>::vec3_type vec3_type;

  assert((i == 0) || (i == 1));
  assert((j == 0) || (j == 1));
  assert((k == 0) || (k == 1));
  i = (i << 1) - 1;  
  j = (j << 1) - 1;
  k = (k << 1) - 1;
  vec3_type new_half_size = b.half_size / static_cast<T>(2);
  return type_box3<T>(
    b.position + new_half_size * vec3_type(static_cast<T>(i), static_cast<T>(j), static_cast<T>(k)), 
    new_half_size);
}

template<typename T>
type_box3<T> operator & (const type_box3<T> & b1,const type_box3<T> & b2)
{
  typedef typename type_box3<T>::vec3_type vec3_type;

  if (b1.IsEmpty() || b2.IsEmpty())
    return type_box3<T>();

  vec3_type p1 = b1.position + b1.half_size;
  vec3_type p2 = b2.position - b2.half_size;

  if (p1.x <= p2.x || p1.y <= p2.y || p1.z <= p2.z)
    return type_box3<T>();

  vec3_type p3 = b1.position - b1.half_size;
  vec3_type p4 = b2.position + b2.half_size;

  if (p4.x <= p3.x || p4.y <= p3.y || p4.z <= p3.z)
    return type_box3<T>();

  vec3_type a = glm::min(p1, p4);
  vec3_type b = glm::max(p3, p2);
  return type_box3<T>((a + b) / static_cast<T>(2), (b - a) / static_cast<T>(2));
}

template<typename T>
type_box3<T> operator | (const type_box3<T> & b1,const type_box3<T> & b2)
{
  typedef typename type_box3<T>::vec3_type vec3_type;

  if (b1.IsEmpty())
    return b2;
  if (b2.IsEmpty())
    return b1;

  vec3_type p1 = b1.position + b1.half_size;
  vec3_type p2 = b2.position + b2.half_size;

  vec3_type p3 = b1.position - b1.half_size;
  vec3_type p4 = b2.position - b2.half_size;

  vec3_type a = glm::max(p1, p2);
  vec3_type b = glm::min(p3, p4);

  return type_box3<T>((a + b) / static_cast<T>(2), (b - a) / static_cast<T>(2));
}



// ==============================================================================================
// circle methods
// ==============================================================================================

template<typename T>
type_box2<T> type_circle<T>::GetBoundingBox() const
{
  typedef typename type_circle<T>::vec2_type vec2_type;

  return IsEmpty()? type_box2<T>() : type_box2<T>(position, vec2_type(radius, radius));
}

template<typename T>
type_box2<T> type_circle<T>::GetInnerBox() const
{
  typedef typename type_circle<T>::vec2_type vec2_type;

  static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */
  return IsEmpty()? type_box2<T>() : type_box2<T>(position, vec2_type(radius * static_cast<T>(INV_SQRT2)));
}

template<typename T>
type_circle<T> operator & (const type_circle<T> & c1,const type_circle<T> & c2)
{
  typedef typename type_circle<T>::vec2_type vec2_type;

  if (c1.IsEmpty() || c2.IsEmpty())
    return type_circle<T>();
  if (c1.position == c2.position)
    return type_circle<T>(c1.position, glm::min(c1.radius, c2.radius));

  vec2_type delta_pos = c2.position - c1.position;    // vector that go from center 1 to center 2  
  T         distance  = glm::length(delta_pos);       // length of such a vector

  if (distance >= c1.radius + c2.radius) // circles too far => returns the empty circle
    return type_circle<T>();

  T t1 = c1.radius / distance;  // positive     
  T t2 = c2.radius / distance;  // positive 
    
  T a = glm::max(-t1, static_cast<T>(1) - t2);    
  T b = glm::min( t1, static_cast<T>(1) + t2);
    
  return type_circle<T>(
    c1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}

template<typename T>
type_circle<T> operator | (const type_circle<T> & c1,const type_circle<T> & c2)
{
  typedef typename type_circle<T>::vec2_type vec2_type;

  if (c1.IsEmpty())
    return c2;
  if (c2.IsEmpty())
    return c1;
  if (c1.position == c2.position)
    return type_circle<T>(c1.position, glm::max(c1.radius, c2.radius));

  vec2_type delta_pos = c2.position - c1.position;    // vector that go from center 1 to center 2  
  T         distance  = glm::length(delta_pos);       // length of such a vector
  
  T t1 = c1.radius / distance;  // positive   
  T t2 = c2.radius / distance;  // positive   
  
  T a = glm::min(-t1, static_cast<T>(1) - t2);    
  T b = glm::max( t1, static_cast<T>(1) + t2);
  
  return type_circle<T>(
    c1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}

// ==============================================================================================
// sphere methods
// ==============================================================================================

template<typename T>
type_box3<T> type_sphere<T>::GetBoundingBox() const
{
  typedef typename type_sphere<T>::vec3_type vec3_type;

  return IsEmpty()? type_box3<T>() : type_box3<T>(position, vec3_type(radius));
}

template<typename T>
type_box3<T> type_sphere<T>::GetInnerBox() const
{
  typedef typename type_sphere<T>::vec3_type vec3_type;

  static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */
  return IsEmpty()? type_box3<T>() : type_box3<T>(position, vec3_type(radius * static_cast<T>(INV_SQRT3)));
}

template<typename T>
type_sphere<T> operator & (const type_sphere<T> & s1,const type_sphere<T> & s2) // intersection
{
  typedef typename type_sphere<T>::vec3_type vec3_type;

  if (s1.IsEmpty() || s2.IsEmpty())
    return type_sphere<T>();
  if (s1.position == s2.position)
    return type_sphere<T>(s1.position, glm::min(s1.radius, s2.radius));

  vec3_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2  
  T         distance  = glm::length(delta_pos);       // length of such a vector

  if (distance >= s1.radius + s2.radius) // sphere too far => returns the empty sphere
    return type_sphere<T>();

  T t1 = s1.radius / distance;  // positive     
  T t2 = s2.radius / distance;  // positive 
    
  T a = glm::max(-t1, static_cast<T>(1) - t2);    
  T b = glm::min( t1, static_cast<T>(1) + t2);
    
  return type_sphere<T>(
    s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}

template<typename T>
type_sphere<T> operator | (const type_sphere<T> & s1,const type_sphere<T> & s2) // union
{
  typedef typename type_sphere<T>::vec3_type vec3_type;

  if (s1.IsEmpty())
    return s2;
  if (s2.IsEmpty())
    return s1;
  if (s1.position == s2.position)
    return type_sphere<T>(s1.position, glm::max(s1.radius, s2.radius));

  vec3_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2  
  T         distance  = glm::length(delta_pos);       // length of such a vector
  
  T t1 = s1.radius / distance;  // positive   
  T t2 = s2.radius / distance;  // positive   
  
  T a = glm::min(-t1, static_cast<T>(1) - t2);    
  T b = glm::max( t1, static_cast<T>(1) + t2);
  
  return type_sphere<T>(
    s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}

}; // namespace chaos
