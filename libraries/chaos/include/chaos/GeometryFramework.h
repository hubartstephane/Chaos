#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

// ==============================================================================================
// Some common typedefs for geometry
// ==============================================================================================

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;

typedef glm::ivec2 int2;
typedef glm::ivec3 int3;
typedef glm::ivec4 int4;

template<typename T> class type_ray2;
template<typename T> class type_ray3;
template<typename T> class type_box2;
template<typename T> class type_box3;
template<typename T> class type_circle;
template<typename T> class type_sphere;


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

/**
 * a generic class for geometric figure
 */
template<typename T = float>
class type_geometric
{
public:

  /** the type of the components */
  typedef T type;
  /** the type of 2D vectors */
  typedef glm::tvec2<type> vec2_type;
  /** the type of 3D vectors */
  typedef glm::tvec3<type> vec3_type;
};

/**
 * a child class for geometrics objects in 2D space
 */

template<typename T = float>
class type_geometric2 : public type_geometric<T>
{
public:
  /** the type of vectors */
  typedef vec2_type vec_type;
};

/**
 * a child class for geometrics objects in 3D space
 */

template<typename T = float>
class type_geometric3 : public type_geometric<T>
{
public:
  /** the type of vectors */
  typedef vec3_type vec_type;
};


/**
 * a generic class for triangle in 2D
 */

template<typename T = float>
class type_triangle2 : public type_geometric2<T>
{
public:
  
  /** default constructor */
  type_triangle2(){}
  /** copy constructor */
  type_triangle2(type_triangle2 const & src) : a(src.a), b(src.b), c(src.c){}   
  /** constructor from points */
  type_triangle2(vec2_type const & in_a, vec2_type const & in_b, vec2_type const & in_c) : a(in_a), b(in_b), c(in_c){}   
  
public:

  /** first point of the triangle */
  vec2_type a;
  /** second point of the triangle */
  vec2_type b;
  /** third point of the triangle */
  vec2_type c;  
};

/** equality test function for triangles */
template<typename T>
bool operator == (type_triangle2<T> const & t1, type_triangle2<T> const & t2)
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
template<typename T>
bool operator != (type_triangle2<T> const & t1, type_triangle2<T> const & t2)
{
  return !(t1 == t2);
}

/**
 * The usual type_triangle2 with float components
 */
typedef type_triangle2<> triangle2;

/**
 * a generic class for triangle in 3D
 */

template<typename T = float>
class type_triangle3 : public type_geometric3<T>
{
public:
  
  /** default constructor */
  type_triangle3(){}
  /** copy constructor */
  type_triangle3(type_triangle3 const & src) : a(src.a), b(src.b), c(src.c){}   
  /** constructor from points */
  type_triangle3(vec3_type const & in_a, vec3_type const & in_b, vec3_type const & in_c) : a(in_a), b(in_b), c(in_c){}   
  
public:

  /** first point of the triangle */
  vec3_type a;
  /** second point of the triangle */
  vec3_type b;
  /** third point of the triangle */
  vec3_type c;  
};

/** equality test function for triangles */
template<typename T>
bool operator == (type_triangle3<T> const & t1, type_triangle3<T> const & t2)
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
template<typename T>
bool operator != (type_triangle3<T> const & t1, type_triangle3<T> const & t2)
{
  return !(t1 == t2);
}

/**
 * The usual type_triangle3 with float components
 */
typedef type_triangle3<> triangle3;

/**
 * Generic class for ray in 2D
 */

template<typename T = float> 
class type_ray2 : public type_geometric2<T>
{
public:

  /** default constructor */
  type_ray2(){}
  /** copy constructor */
  type_ray2(type_ray2 const & src) : position(src.position), direction(src.direction){} 
  /** other constructor */
  type_ray2(vec2_type const & in_position, vec2_type const & in_direction) : position(in_position), direction(in_direction){}
  /** construct a ray from 2 points */
  type_ray2(std::pair<vec2_type, vec2_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)){}

public:

  /** the starting position of the ray in space */
  vec2_type position;
  /** the direction of the ray in space */
  vec2_type direction;
};

/** equality function for ray */
template<typename T>
bool operator == (type_ray2<T> const & r1, type_ray2<T> const & r2)
{
  return (r1.position == r2.position) && (r1.direction == r2.direction);
}

/** difference function for ray */
template<typename T>
bool operator != (type_ray2<T> const & r1, type_ray2<T> const & r2)
{
  return !(r1 == r2);
}

/**
 * The usual type_ray2 with float components
 */
typedef type_ray2<> ray2;

/**
 * Generic class for ray in 3D
 */

template<typename T = float> 
class type_ray3 : public type_geometric3<T>
{
public:

  /** default constructor */
  type_ray3(){}
  /** copy constructor */
  type_ray3(type_ray3 const & src) : position(src.position), direction(src.direction){} 
  /** other constructor */
  type_ray3(vec3_type const & in_position, vec3_type const & in_direction) : position(in_position), direction(in_direction){}
  /** construct a ray from 2 points */
  type_ray3(std::pair<vec3_type, vec3_type> const & pts) : position(pts.first), direction(glm::normalize(pts.second - pts.first)){}

public:

  /** the starting position of the ray in space */
  vec3_type position;
  /** the direction of the ray in space */
  vec3_type direction;
};

/** equality function for ray */
template<typename T>
bool operator == (type_ray3<T> const & r1, type_ray3<T> const & r2)
{
  return (r1.position == r2.position) && (r1.direction == r2.direction);
}

/** difference function for ray */
template<typename T>
bool operator != (type_ray3<T> const & r1, type_ray3<T> const & r2)
{
  return !(r1 == r2);
}

/**
 * The usual type_ray3 with float components
 */
typedef type_ray3<> ray3;

/**
 * Generic class for box in 2D
 */

template<typename T = float>
class type_box2 : public type_geometric2<T>
{
public:

  /** constructor (empty box2) */
  type_box2() : half_size((T)-1.0f){}
  /** copy constructor */
  type_box2(type_box2 const & src) : position(src.position), half_size(src.half_size){}
  /** other constructor */
  type_box2(vec2_type const & in_position, vec2_type const & in_half_size) : position(in_position), half_size(in_half_size){}
  /** construct a box from 2 points */
  type_box2(std::pair<vec2_type, vec2_type> const & pts)
  {
    vec2_type a = glm::min(pts.first, pts.second);
    vec2_type b = glm::max(pts.first, pts.second);

    position  = (b + a) / static_cast<type>(2);
    half_size = (b - a) / static_cast<type>(2);
  }

  /** increase the box size with a single vertex */
  void Extend(vec_type const & v)
  {
    if (IsEmpty())
    {
      position  = v;
      half_size = vec_type((T)0.0f);
    }
    else
    {
      std::pair<vec_type, vec_type> corners = GetCorners();
      corners.first  = glm::min(corners.first, v);
      corners.second = glm::max(corners.second, v);
      *this = type_box2(corners);
    }
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
    return (half_size.x < 0) || (half_size.y < 0);
  }

  /** set the box has an empty box */
  void SetEmpty()
  {
    half_size = vec2_type((T)-1.0f);
  }

  /** returns the perimeter of the box */
  type GetPerimeter() const 
  {
    return static_cast<type>(4) * (half_size.x + half_size.y);
  }

  /** returns the surface of the box */
  type GetSurface() const  
  {
    return static_cast<type>(4) * (half_size.x * half_size.y);
  }

  /** returns true whether the point is contained in the box */
  bool Contains(type const & pt) const
  {
    return glm::all(glm::lessThanEqual(glm::abs(pt - position), half_size));
  }

  /** returns the bounding circle for the box */
  type_circle<type> GetBoundingCircle() const;
  /** returns the inner circle for the box */
  type_circle<type> GetInnerCircle() const;

  /** the center of the box */
  vec2_type position;
  /** the half size the box */
  vec2_type half_size;
};

/** equality function for box */
template<typename T>
bool operator == (type_box2<T> const & b1, type_box2<T> const & b2)
{
  return (b1.position == b1.position) && (b1.half_size == b2.half_size);
}

/** difference function for box */
template<typename T>
bool operator != (type_box2<T> const & b1, type_box2<T> const & b2)
{
  return !(b1 == b2);
}


/**
 * The usual box2 with float components
 */
typedef type_box2<> box2;

/** returns one of the 4 box2 obtained by splitting the src */
template<typename T>
type_box2<T> GetSplitBox(type_box2<T> const & src, int i, int j);

/** returns the "aspect" of the box (width/height) */
template<typename T>
typename type_box2<T>::type GetBoxAspect(type_box2<T> const & b);

/** returns intersection of box2 */
template<typename T>
box2 operator & (type_box2<T> const & b1,type_box2<T> const & b2);

/** returns union of box2 */
template<typename T>
box2 operator | (type_box2<T> const & b1,type_box2<T> const & b2);

/**
 * Generic class for box in 3D
 */
template<typename T = float>
class type_box3 : public type_geometric3<T>
{
public:

  /** constructor (empty sphere) */
  type_box3() : half_size((T)-1.0f){}
  /** copy constructor */
  type_box3(type_box3 const & src) : position(src.position), half_size(src.half_size){}
  /** other constructor */
  type_box3(vec3_type const & in_position, vec3_type const & in_half_size) : position(in_position), half_size(in_half_size){}
  /** construct a box from 2 points */
  type_box3(std::pair<vec3_type, vec3_type> const & pts)
  {
    vec3_type a = glm::min(pts.first, pts.second);
    vec3_type b = glm::max(pts.first, pts.second);

    position  = (b + a) / static_cast<type>(2);
    half_size = (b - a) / static_cast<type>(2);
  }

  /** increase the box size with a single vertex */
  void Extend(vec_type const & v)
  {
    if (IsEmpty())
    {
      position  = v;
      half_size = vec_type((T)0.0f);
    }
    else
    {
      std::pair<vec_type, vec_type> corners = GetCorners();
      corners.first  = glm::min(corners.first, v);
      corners.second = glm::max(corners.second, v);
      *this = type_box3(corners);
    }
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
    return (half_size.x < 0) || (half_size.y < 0) || (half_size.z < 0);
  }

  /** set the box has an empty box */
  void SetEmpty()
  {
    half_size = float3((T)-1.0f);
  }

  /** return the volume of the box */
  type GetVolume() const 
  {
    return static_cast<type>(8) * half_size.x * half_size.y * half_size.z;
  }

  /** return the surface of the box */
  type GetSurface() const 
  {
    return static_cast<type>(8) *((half_size.x * half_size.y) + (half_size.y * half_size.z) + (half_size.z * half_size.x));
  };


  /** returns true whether the point is contained in the box */
  bool Contains(vec3_type const & pt) const
  {
    return glm::all(glm::lessThanEqual(glm::abs(pt - position), half_size));
  }

  /** returns the bounding sphere for the box */
  type_sphere<type> GetBoundingSphere() const;
  /** returns the inner sphere for the box */
  type_sphere<type> GetInnerSphere() const;

public:

  /** the center of the box */
  vec3_type position;
  /** the half size the box */
  vec3_type half_size;
};

/** equality function for box */
template<typename T>
bool operator == (type_box3<T> const & b1, type_box3<T> const & b2)
{
  return (b1.position == b1.position) && (b1.half_size == b2.half_size);
}

/** difference function for box */
template<typename T>
bool operator != (type_box3<T> const & b1, type_box3<T> const & b2)
{
  return !(b1 == b2);
}

/**
 * The usual type_box3 with float components
 */
typedef type_box3<> box3;

/** returns one of the 8 box3 obtained by splitting the src */
template<typename T>
type_box3<T> GetSplitBox(type_box3<T> const & src, int i, int j, int k);

/** returns intersection of box3 */
template<typename T>
type_box3<T> operator & (type_box3<T> const & b1,type_box3<T> const & b2);

/** returns union of box3 */
template<typename T>
type_box3<T> operator | (type_box3<T> const & b1,type_box3<T> const & b2);


/**
 * Generic class for circle(2D)
 */

template<typename T = float>
class type_circle : public type_geometric2<T>
{
public:

  /** constructor (empty circle) */
  type_circle() : radius((T)-1.0f){}
  /** copy constructor */
  type_circle(type_circle const & src) : position(src.position), radius(src.radius){}
  /** other constructor */
  type_circle(vec2_type const & in_position, float in_radius) : position(in_position), radius(in_radius){}

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

  /** returns the perimeter of the circle */
  float GetPerimeter() const 
  {
    return static_cast<type>(2.0 * M_PI) * radius;
  }

  /** returns the surface of the circle */
  float GetSurface() const 
  {
    return static_cast<type>(M_PI) * radius * radius;
  }

  /** returns true whether the point is contained in the circle */
  bool Contains(vec2_type const & pt) const
  {
    return glm::length2(pt - position) <= radius * radius;
  }

  /** returns the bounding box of the circle */
  type_box2<type> GetBoundingBox() const;
  /** returns the bounding box of the circle (square) */
  type_box2<type> GetInnerBox() const;

public:

  /** the center of the circle */
  vec2_type position;
  /** the radius of the circle */
  type radius;
};

/** equality function for circle */
template<typename T>
bool operator == (type_circle<T> const & c1, type_circle<T> const & c2)
{
  return (b1.position == b1.position) && (b1.radius == b2.radius);
}

/** difference function for circle */
template<typename T>
bool operator != (type_circle<T> const & c1, type_circle<T> const & c2)
{
  return !(c1 == c2);
}

/**
 * The usual circle with float components
 */
typedef type_circle<> circle;

/** returns intersection of circle */
template<typename T>
type_circle<T> operator & (type_circle<T> const & c1,type_circle<T> const & c2);

/** returns union of circle */
template<typename T>
type_circle<T> operator | (type_circle<T> const & c1,type_circle<T> const & c2);

/**
 * Generic class for sphere(3D)
 */

template<typename T = float>
class type_sphere : public type_geometric3<T>
{
public:

  /** constructor (empty sphere) */
  type_sphere() : radius((T)-1.0f){}
  /** copy constructor */
  type_sphere(type_sphere const & src) : position(src.position), radius(src.radius){}
  /** other constructor */
  type_sphere(vec3_type const & in_position, float in_radius) : position(in_position), radius(in_radius){}

  /** returns true whether the sphere is empty */
  bool IsEmpty() const
  {
    return (radius < 0);
  }

  /** set the circle has an empty circle */
  void SetEmpty()
  {
    radius = (T)-1.0f;
  }

  /** returns the volume of the sphere */
  float GetVolume() const 
  {
    return static_cast<type>((4.0 / 3.0) * M_PI) * radius * radius * radius;
  }

  /** returns the surface of the sphere */
  float GetSurface() const 
  {
    return static_cast<type>(4.0 * M_PI) * radius * radius;
  }

  /** returns true whether the point is contained in the circle */
  bool Contains(vec3_type const & pt) const
  {
    return glm::length3(pt - position) <= radius * radius;
  }

  /** returns the bounding box of the sphere */
  type_box3<type> GetBoundingBox() const;
  /** returns the bounding box of the sphere (square) */
  type_box3<type> GetInnerBox() const;

public:

  /** the center of the sphere */
  vec3_type position;
  /** the radius of the sphere */
  float  radius;
};

/** equality function for sphere */
template<typename T>
bool operator == (type_sphere<T> const & s1, type_sphere<T> const & s2)
{
  return (s1.position == s1.position) && (s1.radius == s2.radius);
}

/** difference function for sphere */
template<typename T>
bool operator != (type_sphere<T> const & s1, type_sphere<T> const & s2)
{
  return !(s1 == s2);
}

/**
 * The usual circle with float components
 */
typedef type_sphere<> sphere;

/** returns intersection of sphere */
template<typename T>
type_sphere<T> operator & (type_sphere<T> const & s1, type_sphere<T> const & s2);

/** returns union of sphere */
template<typename T>
type_sphere<T> operator | (type_sphere<T> const & s1, type_sphere<T> const & s2);


/**
 * Some methods to force an object to keep inside another
 */

template<typename T>
void ForceStayInside(type_box2<T> & bigger, type_box2<T> & smaller, bool move_big)
{
  assert(bigger.half_size.x >= smaller.half_size.x);
  assert(bigger.half_size.y >= smaller.half_size.y);



}

template<typename T>
void ForceStayInside(type_box3<T> & bigger, type_box3<T> & smaller, bool move_big)
{
  int count = 3;

#if _DEBUG
  for (int i = 0 ; i < count ; ++i)
  {
    assert(bigger.half_size[i] >= smaller.half_size[i]);
  }
#endif

  auto big_corners   = bigger.GetCorners();
  auto small_corners = smaller.GetCorners();

  for (int i = 0 ; i < count ; ++i)
  {
    float delta = small_corners.first[i] - big_corners.first[i];
    if (delta < 0)
    {    
      if (move_big)
        bigger.position[i] += delta;
      else
        smaller.position[i] -= delta;
    }
    else
    {
      float delta = small_corners.second[i] - big_corners.second[i];
      if (delta > 0)
      {
        if (move_big)
          bigger.position[i] += delta;
        else
          smaller.position[i] -= delta;    
      }   
    }
  }
}

}; // namespace chaos

