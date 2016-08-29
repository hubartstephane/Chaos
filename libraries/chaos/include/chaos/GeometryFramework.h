#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MathTools.h>

namespace chaos
{

// ==============================================================================================
// utility function
// ==============================================================================================

/** returns the min component of a vector */
template<typename T>
T GetMinComponent(glm::tvec1<T> const & src)
{
  return src.x;
}

/** returns the min component of a vector */
template<typename T>
T GetMinComponent(glm::tvec2<T> const & src)
{
  return glm::min(src.x, src.y);
}

/** returns the min component of a vector */
template<typename T>
T GetMinComponent(glm::tvec3<T> const & src)
{
  return glm::min(src.x, glm::min(src.y, src.z));
}

/** returns the min component of a vector */
template<typename T>
T GetMinComponent(glm::tvec4<T> const & src)
{
  return glm::min(glm::min(src.x, src.y), glm::min(src.z, src.w));
}

/** returns the max component of a vector */
template<typename T>
T GetMaxComponent(glm::tvec1<T> const & src)
{
  return src.x;
}

/** returns the max component of a vector */
template<typename T>
T GetMaxComponent(glm::tvec2<T> const & src)
{
  return glm::max(src.x, src.y);
}

/** returns the max component of a vector */
template<typename T>
T GetMaxComponent(glm::tvec3<T> const & src)
{
  return glm::max(src.x, glm::max(src.y, src.z));
}

/** returns the max component of a vector */
template<typename T>
T GetMaxComponent(glm::tvec4<T> const & src)
{
  return glm::max(glm::max(src.x, src.y), glm::max(src.z, src.w));
}

// ==============================================================================================
// Some common typedefs for geometry
// ==============================================================================================

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;

typedef glm::ivec2 int2;
typedef glm::ivec3 int3;
typedef glm::ivec4 int4;

template<typename T, int dimension> class type_ray;
template<typename T, int dimension> class type_box;
template<typename T, int dimension> class type_sphere;
template<typename T, int dimension> class type_triangle;

template<typename T> using type_ray2      = type_ray<T, 2>;
template<typename T> using type_ray3      = type_ray<T, 3>;
template<typename T> using type_box2      = type_box<T, 2>;
template<typename T> using type_box3      = type_box<T, 3>;
template<typename T> using type_sphere2   = type_sphere<T, 2>;
template<typename T> using type_sphere3   = type_sphere<T, 3>;
template<typename T> using type_triangle2 = type_triangle<T, 2>;
template<typename T> using type_triangle3 = type_triangle<T, 3>;

using ray2      = type_ray2<float>;
using ray3      = type_ray3<float>;
using box2      = type_box2<float>;
using box3      = type_box3<float>;
using sphere2   = type_sphere2<float>;
using sphere3   = type_sphere3<float>;
using triangle2 = type_triangle2<float>;
using triangle3 = type_triangle3<float>;

// ==============================================================================================
// geometry classes
// ==============================================================================================

/**
 * base class for geometry objects that have some commons definitions
 */

template<typename T> 
class type_geometric_base
{
public:

  /** the type of the components */
  typedef T type;
  /** the type of 2D vectors */
  typedef glm::tvec2<type> vec2_type;
  /** the type of 3D vectors */
  typedef glm::tvec3<type> vec3_type;
};

template<typename T, int dimension> class type_geometric;

/**
* geometry class specialisation for 2 dimensions objects
*/

template<typename T> 
class type_geometric<T, 2> : public type_geometric_base<T>
{
public:

  /** the dimension of the space */
  static int const dimension = 2;
  /** the type of vector */
  typedef vec2_type vec_type;
};

/**
* geometry class specialisation for 3 dimensions objects
*/

template<typename T>
class type_geometric<T, 3> : public type_geometric_base<T>
{
public:

  /** the dimension of the space */
  static int const dimension = 3;
  /** the type of vector */
  typedef vec3_type vec_type;
};


// ==============================================================================================
// boxes classes
// ==============================================================================================

template<typename T, int dimension>
class type_box : public type_geometric<T, dimension>
{
public:

  typedef typename type_geometric<T, dimension>::vec_type vec_type;

  /** constructor (empty box2) */
  type_box() : half_size((T)-1.0f) {}
  /** copy constructor */
  type_box(type_box const & src) : position(src.position), half_size(src.half_size) {}
  /** other constructor */
  type_box(vec_type const & in_position, vec_type const & in_half_size) : position(in_position), half_size(in_half_size) {}
  /** construct a box from 2 points */
  type_box(std::pair<vec_type, vec_type> const & pts)
  {
    vec_type a = glm::min(pts.first, pts.second);
    vec_type b = glm::max(pts.first, pts.second);

    position  = (b + a) / static_cast<type>(2);
    half_size = (b - a) / static_cast<type>(2);
  }

  /** get the corners of the box */
  std::pair<vec_type, vec_type> GetCorners() const
  {
    assert(!IsEmpty());
    return std::make_pair(position - half_size, position + half_size);
  }

  /** returns true whether the box is empty */
  bool IsEmpty() const
  {
    return glm::any(glm::lessThan(half_size, vec_type((T)0.0f)));
  }

  /** set the box has an empty box */
  void SetEmpty()
  {
    half_size = vec_type((T)-1.0f);
  }

  /** increase the box size with a single vertex */
  void Extend(vec_type const & v)
  {
    if (IsEmpty())
    {
      position = v;
      half_size = vec_type((T)0.0f);
    }
    else
    {
      std::pair<vec_type, vec_type> corners = GetCorners();
      corners.first  = glm::min(corners.first, v);
      corners.second = glm::max(corners.second, v);
      *this = type_box(corners);
    }
  }

  /** returns true whether the point is contained in the box */
  bool Contains(vec_type const & pt) const
  {
    return glm::all(glm::lessThanEqual(glm::abs(pt - position), half_size));
  }

public:

  /** the center of the box */
  vec_type position;
  /** the half size the box */
  vec_type half_size;
};


// ==============================================================================================
// boxes functions
// ==============================================================================================

/** equality function for box */
template<typename T, int dimension>
bool operator == (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
{
  return (b1.position == b1.position) && (b1.half_size == b2.half_size);
}

/** difference function for box */
template<typename T, int dimension>
bool operator != (type_box<T, dimension> const & b1, type_box<T, dimension> const & b2)
{
  return !(b1 == b2);
}

/** returns the perimeter of the box */
template<typename T> 
T GetPerimeter(type_box2<T> const & b)
{
  return static_cast<type>(4) * (b.half_size.x + b.half_size.y);
}

/** returns the surface of the box */
template<typename T>
T GetSurface(type_box2<T> const & b)
{
  return static_cast<type>(4) * (b.half_size.x * b.half_size.y);
}

/** return the volume of the box */
template<typename T>
T GetVolume(type_box3<T> const & b)
{
  return static_cast<type>(8) * b.half_size.x * b.half_size.y * b.half_size.z;
}

/** return the surface of the box */
template<typename T>
T GetSurface(type_box3<T> const & b)
{
  return static_cast<type>(8) *((b.half_size.x * b.half_size.y) + (b.half_size.y * b.half_size.z) + (b.half_size.z * b.half_size.x));
};

/** returns the bounding circle for the box */
template<typename T>
type_sphere2<T> GetBoundingCircle(type_box2<T> const & b)
{
  return b.IsEmpty() ? type_sphere2<T>() : type_sphere2<T>(b.position, glm::length(b.half_size));
}

/** returns the inner circle for the box */
template<typename T>
type_sphere2<T> GetInnerCircle(type_box2<T> const & b)
{
  return b.IsEmpty() ? type_sphere2<T>() : type_sphere2<T>(b.position, GetMinComponent(b.half_size));
}

/** returns the bounding sphere for the box */
template<typename T>
type_sphere3<T> GetBoundingSphere(type_box3<T> const & b)
{
  return b.IsEmpty() ? type_sphere3<T>() : type_sphere3<T>(b.position, glm::length(b.half_size));
}

/** returns the inner sphere for the box */
template<typename T>
type_sphere3<T> GetInnerSphere(type_box3<T> const & b)
{
  return b.IsEmpty() ? type_sphere3<T>() : type_sphere3<T>(b.position, GetMinComponent(b.half_size));
}

/** returns the "aspect" of the box (width/height) */
template<typename T>
T GetBoxAspect(type_box2<T> const & b)
{
  return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
}

/** intersection of 2 boxes */
template<typename T, int dimension>
type_box<T, dimension> operator & (const type_box<T, dimension> & b1, const type_box<T, dimension> & b2)
{
  typedef typename type_box<T, dimension>::vec_type vec_type;

  if (b1.IsEmpty() || b2.IsEmpty()) // any of the 2 is empty, intersection is empty
    return type_box<T, dimension>();

  vec_type A1 = b1.position + b1.half_size;
  vec_type B2 = b2.position - b2.half_size;

  if (glm::any(glm::lessThanEqual(A1, B2)))
    return type_box<T, dimension>();

  vec_type B1 = b1.position - b1.half_size;
  vec_type A2 = b2.position + b2.half_size;

  if (glm::any(glm::lessThanEqual(A2, B1)))
    return type_box<T, dimension>();

  vec_type A = glm::min(A1, A2);
  vec_type B = glm::max(B1, B2);

  return type_box<T, dimension>(std::make_pair(A, B));
}

/** union of 2 boxes */
template<typename T, int dimension>
type_box<T, dimension> operator | (const type_box<T, dimension> & b1, const type_box<T, dimension> & b2)
{
  typedef typename type_box<T, dimension>::vec_type vec_type;

  if (b1.IsEmpty()) // if one is empty, returns other
    return b2;
  if (b2.IsEmpty())
    return b1;

  vec_type A1 = b1.position + b1.half_size;
  vec_type A2 = b2.position + b2.half_size;

  vec_type B1 = b1.position - b1.half_size;
  vec_type B2 = b2.position - b2.half_size;

  vec_type A = glm::max(A1, A2);
  vec_type B = glm::min(B1, B2);

  return type_box<T, dimension>(std::make_pair(A, B));
}

/** returns one of the sub-boxes obtained by splitting the src */
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

/** returns one of the sub-boxes obtained by splitting the src */
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

// ==============================================================================================
// triangle classes
// ==============================================================================================

template<typename T, int dimension>
class type_triangle : public type_geometric<T, dimension>
{
public:

  typedef typename type_geometric<T, dimension>::vec_type vec_type;

  /** default constructor */
  type_triangle() {}
  /** copy constructor */
  type_triangle(type_triangle const & src) : a(src.a), b(src.b), c(src.c) {}
  /** constructor from points */
  type_triangle(vec_type const & in_a, vec_type const & in_b, vec_type const & in_c) : a(in_a), b(in_b), c(in_c) {}

public:

  /** first point of the triangle */
  vec_type a;
  /** second point of the triangle */
  vec_type b;
  /** third point of the triangle */
  vec_type c;
};

// ==============================================================================================
// triangles functions
// ==============================================================================================

/** equality test function for triangles */
template<typename T, int dimension>
bool operator == (type_triangle<T, dimension> const & t1, type_triangle<T, dimension> const & t2)
{
  if (t1.a == t2.a)
  {
    if (t1.b == t2.b && t1.c == t2.c)
      return true;
    if (t1.c == t2.b && t1.b == t2.c)
      return true;
  }
  if (t1.a == t2.b)
  {
    if (t1.b == t2.a && t1.c == t2.c)
      return true;
    if (t1.c == t2.a && t1.b == t2.c)
      return true;
  }
  if (t1.a == t2.c)
  {
    if (t1.b == t2.a && t1.c == t2.b)
      return true;
    if (t1.c == t2.a && t1.b == t2.b)
      return true;
  }
  return false;
}

/** difference test function for triangle */
template<typename T, int dimension>
bool operator != (type_triangle<T, dimension> const & t1, type_triangle<T, dimension> const & t2)
{
  return !(t1 == t2);
}

// ==============================================================================================
// ray classes
// ==============================================================================================

template<typename T, int dimension>
class type_ray : public type_geometric<T, dimension>
{
public:

  typedef typename type_geometric<T, dimension>::vec_type vec_type;

  /** default constructor */
  type_ray() {}
  /** copy constructor */
  type_ray(type_ray const & src) : position(src.position), direction(src.direction) {}
  /** other constructor */
  type_ray(vec_type const & in_position, vec_type const & in_direction) : position(in_position), direction(in_direction) {}
  /** construct a ray from 2 points */
  type_ray(std::pair<vec_type, vec_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)) {}

public:

  /** the starting position of the ray in space */
  vec_type position;
  /** the direction of the ray in space */
  vec_type direction;
};

// ==============================================================================================
// ray functions
// ==============================================================================================

/** equality function for ray */
template<typename T, int dimension>
bool operator == (type_ray<T, dimension> const & r1, type_ray<T, dimension> const & r2)
{
  return (r1.position == r2.position) && (r1.direction == r2.direction);
}

/** difference function for ray */
template<typename T, int dimension>
bool operator != (type_ray<T, dimension> const & r1, type_ray<T, dimension> const & r2)
{
  return !(r1 == r2);
}

// ==============================================================================================
// sphere/circle classes
// ==============================================================================================

template<typename T, int dimension>
class type_sphere : public type_geometric<T, dimension>
{
public:

  typedef typename type_geometric<T, dimension>::vec_type vec_type;
  typedef typename type_geometric<T, dimension>::type     type;

  /** constructor (empty circle) */
  type_sphere() : radius((T)-1.0f) {}
  /** copy constructor */
  type_sphere(type_sphere const & src) : position(src.position), radius(src.radius) {}
  /** other constructor */
  type_sphere(vec_type const & in_position, type in_radius) : position(in_position), radius(in_radius) {}

  /** returns true whether the circle is empty */
  bool IsEmpty() const
  {
    return (radius < 0);
  }

  /** set the circle has an empty circle */
  void SetEmpty()
  {
    radius = (T)-1.0f;
  }

  /** returns true whether the point is contained in the circle */
  bool Contains(vec_type const & pt) const
  {
    return glm::length2(pt - position) <= radius * radius;
  }

public:

  /** the center of the circle */
  vec_type position;
  /** the radius of the circle */
  type radius;
};


// ==============================================================================================
// sphere/circle functions
// ==============================================================================================

/** equality function for circle */
template<typename T, int dimension>
bool operator == (type_sphere<T, dimension> const & c1, type_sphere<T, dimension> const & c2)
{
  return (b1.position == b1.position) && (b1.radius == b2.radius);
}

/** difference function for circle */
template<typename T, int dimension>
bool operator != (type_sphere<T, dimension> const & c1, type_sphere<T, dimension> const & c2)
{
  return !(c1 == c2);
}

/** returns the perimeter of the circle */
template<typename T>
float GetPerimeter(type_sphere2<T> const & c)
{
  return static_cast<T>(2.0 * M_PI) * c.radius;
}

/** returns the surface of the circle */
template<typename T>
float GetSurface(type_sphere2<T> const & c)
{
  return static_cast<T>(M_PI) * c.radius * c.radius;
}

/** returns the volume of the sphere */
template<typename T>
T GetVolume(type_sphere3<T> const & s)
{
  return static_cast<T>((4.0 / 3.0) * M_PI) * s.radius * s.radius * s.radius;
}

/** returns the surface of the sphere */
template<typename T>
float GetSurface(type_sphere3<T> const & s)
{
  return static_cast<T>(4.0 * M_PI) * s.radius * s.radius;
}


/** returns the bounding box of the circle */
template<typename T>
type_box2<T> GetBoundingBox(type_sphere2<T> const & c)
{
  typedef typename type_sphere2<T>::vec_type vec_type;

  return c.IsEmpty() ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius, c.radius));
}

/** returns the bounding box of the circle (square) */
template<typename T>
type_box2<T> GetInnerBox(type_sphere2<T> const & c)
{
  typedef typename type_sphere2<T>::vec_type vec_type;

  static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

  return c.IsEmpty() ? type_box2<T>() : type_box2<T>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
}

template<typename T>
type_box3<T> GetBoundingBox(type_sphere3<T> const & s)
{
  typedef typename type_sphere3<T>::vec_type vec_type;

  return s.IsEmpty() ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius));
}

template<typename T>
type_box3<T> GetInnerBox(type_sphere3<T> const & s)
{
  typedef typename type_sphere3<T>::vec_type vec_type;

  static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

  return s.IsEmpty() ? type_box3<T>() : type_box3<T>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
}

/** returns intersection of 2 spheres */
template<typename T, int dimension>
type_sphere<T, dimension> operator & (const type_sphere<T, dimension> & s1, const type_sphere<T, dimension> & s2) // intersection
{
  typedef typename type_sphere<T, dimension>::vec_type vec_type;

  if (s1.IsEmpty() || s2.IsEmpty())
    return type_sphere<T, dimension>();
  if (s1.position == s2.position)
    return type_sphere<T, dimension>(s1.position, glm::min(s1.radius, s2.radius));

  vec_type delta_pos = s2.position - s1.position;   // vector that go from center 1 to center 2  
  T        distance  = glm::length(delta_pos);      // length of such a vector

  if (distance >= s1.radius + s2.radius) // sphere too far => returns the empty sphere
    return type_sphere<T, dimension>();

  T t1 = s1.radius / distance;  // positive     
  T t2 = s2.radius / distance;  // positive 

  T a = glm::max(-t1, static_cast<T>(1) - t2);
  T b = glm::min( t1, static_cast<T>(1) + t2);

  return type_sphere<T, dimension>(
    s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}

/** returns union of 2 spheres */
template<typename T, int dimension>
type_sphere<T, dimension> operator | (const type_sphere<T, dimension> & s1, const type_sphere<T, dimension> & s2) // union
{
  typedef typename type_sphere<T, dimension>::vec_type vec_type;

  if (s1.IsEmpty())
    return s2;
  if (s2.IsEmpty())
    return s1;
  if (s1.position == s2.position)
    return type_sphere<T, dimension>(s1.position, glm::max(s1.radius, s2.radius));

  vec_type delta_pos = s2.position - s1.position;    // vector that go from center 1 to center 2  
  T        distance  = glm::length(delta_pos);       // length of such a vector

  T t1 = s1.radius / distance;  // positive   
  T t2 = s2.radius / distance;  // positive   

  T a = glm::min(-t1, static_cast<T>(1) - t2);
  T b = glm::max( t1, static_cast<T>(1) + t2);

  return type_sphere<T, dimension>(
    s1.position + ((b + a) / static_cast<T>(2)) * delta_pos,
    ((b - a) / static_cast<T>(2)) * distance);
}


// ==============================================================================================
// Restriction function
// ==============================================================================================

template<typename T, int dimension>
bool RestrictToInside(type_box<T, dimension> & bigger, type_box<T, dimension> & smaller, bool move_big)
{
  bool result = false;

  assert(!bigger.IsEmpty() || smaller.IsEmpty());

  if (bigger.IsEmpty() || smaller.IsEmpty())
    return result;

  assert(glm::all(glm::lessThanEqual(smaller.half_size, bigger.half_size)));

  auto big_corners   = bigger.GetCorners();
  auto small_corners = smaller.GetCorners();

  int const count = dimension;
  for (int i = 0 ; i < count ; ++i)
  {
    T delta1 = small_corners.first[i] - big_corners.first[i];
    if (delta1 < 0)
    {    
      if (move_big)
        bigger.position[i] += delta1;
      else
        smaller.position[i] -= delta1;
      result = true;
    }
    else
    {
      T delta2 = small_corners.second[i] - big_corners.second[i];
      if (delta2 > 0)
      {
        if (move_big)
          bigger.position[i] += delta2;
        else
          smaller.position[i] -= delta2;    
        result = true;
      }   
    }
  }
  return result;
}

template<typename T, int dimension>
void RestrictToOutside(type_box<T, dimension> & src, type_box<T, dimension> & target)
{
  if (src.IsEmpty() || target.IsEmpty())
    return false;

  auto src_corners    = src.GetCorners();
  auto target_corners = target.GetCorners();

  if (glm::any(glm::lessThan(src_corners.second, target_corners.first)))
    return false;
  if (glm::any(glm::greaterThan(src_corners.first, target_corners.second)))
    return false;

  // compute the minimum distance, and best direction (+X, -X, +Y ...) to move the box
  T   best_distance  = (T)-1;
  int best_direction = -1;
  for (int i = 0 ; i < dimension ; ++i) 
  {
    // in positive direction (dist_pos is to be positive)
    T dist_pos = src_corners.second[i] - target_corners.first[i];
    if (dist_pos <= 0)
      return false; // no collision, nothing to do
    T dist_neg = target_corners.second[i] - src_corners.first[i];
    if (dist_neg <= 0)
      return false; // no collision, nothing to do

    if (best_distance < 0 || dist_pos < best_distance)
    {
      best_distance  = dist_pos;
      best_direction = 2 * i;    
    }
    // in negative direction (dist_neg is to be positive)
    if (best_distance < 0 || dist_neg < best_distance)
    {
      best_distance  = dist_neg;
      best_direction = 2 * i + 1;    
    }  
  }

  // do the displacement
  if (best_direction >= 0)
  {
    if ((best_direction & 1) == 0)
      target.position[best_direction / 2] += best_distance; // positive direction
    else
      target.position[best_direction / 2] -= best_distance; // negative direction  
    return true;
  }
  return false;
}

template<typename T, int dimension>
bool RestrictToInside(type_sphere<T, dimension> & bigger, type_sphere<T, dimension> & smaller, bool move_big)
{
  assert(!bigger.IsEmpty() || smaller.IsEmpty());

  assert(smaller.radius <= bigger.radius);

  if (bigger.IsEmpty() || smaller.IsEmpty())
    return false;

  auto delta_pos = smaller.position - bigger.position;
  T    l         = glm::length(delta_pos);
  
  if (l + smaller.radius <= bigger.radius) // smaller fully inside bigger
    return false;

  T factor = ((bigger.radius - smaller.radius) / l);

  delta_pos = delta_pos * factor;

  if (move_big)
    bigger.position = smaller.position - delta_pos;
  else
    smaller.position = bigger.position + delta_pos;

  return true;
}









template<typename T, int dimension>
bool RestrictToOutside(type_sphere<T, dimension> & src, type_sphere<T, dimension> & target)
{
  if (src.IsEmpty() || target.IsEmpty())
    return false;

  auto delta_pos = target.position - src.position;
  T    l         = glm::length(delta_pos);

  if (l - target.radius >= src.radius) 
    return false;

  if (l != 0)
  {
    T factor  = ((src.radius + target.radius) / l);
    delta_pos = delta_pos * factor;  
  }
  else
  {
    auto new_direction = type_sphere<T, dimension>::vec_type(0);
    new_direction[0] = (T)1;

    T factor  = (src.radius + target.radius);
    delta_pos = new_direction * factor;    
  }

  target.position = src.position + delta_pos;

  return true;
}

// ==============================================================================================
// Collision function
// ==============================================================================================

template<typename T, int dimension>
bool Collide(type_box<T, dimension> & src1, type_box<T, dimension> & src2)
{
  if (src1.IsEmpty() || src2.IsEmpty())
    return false;

  auto src1_corners = src1.GetCorners();
  auto src2_corners = src2.GetCorners();

  if (glm::any(glm::lessThan(src1_corners.second, src2_corners.first)))
    return false;
  if (glm::any(glm::greaterThan(src1_corners.first, src2_corners.second)))
    return false;

  return true;
}

template<typename T, int dimension>
bool Collide(type_sphere<T, dimension> & src1, type_sphere<T, dimension> & src2)
{
  if (src1.IsEmpty() || src2.IsEmpty())
    return false;

  return glm::length2(src1.position - src2.position) <= MathTools::Square(src1.radius + src2.radius);
}

}; // namespace chaos

