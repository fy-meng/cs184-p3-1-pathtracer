#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.

  Vector3D dist = r.o - this->o;
  double a = r.d.norm2(), b = 2 * dot(dist, r.d), c = dist.norm2() - this->r2;
  double determinant = b * b - 4 * a * c;
  if (determinant >= 0) {
    t1 = (-b - sqrt(determinant)) / (2 * a);
    t2 = (-b + sqrt(determinant)) / (2 * a);
    return true;
  } else
    return false;
}

bool Sphere::intersect(const Ray& r) const {

  // (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.

  double t1, t2;
  if (test(r, t1, t2)) {
    if (r.min_t <= t1 && t1 <= r.max_t) {
      r.max_t = t1;
      return true;
    } else if (r.min_t <= t2 && t2 <= r.max_t) {
      r.max_t = t2;
      return true;
    } else
      return false;
  } else
    return false;
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

  double t1, t2;
  if (test(r, t1, t2)) {
    if (r.min_t <= t1 && t1 <= r.max_t)
      i->t = r.max_t = t1;
    else if (r.min_t <= t2 && t2 <= r.max_t)
      i->t = r.max_t = t2;
    else
      return false;

    i->n = r.o + i->t * r.d - this->o;
    i->n.normalize();
    i->primitive = this;
    i->bsdf = get_bsdf();

    return true;
  } else
    return false;
}

void Sphere::draw(const Color& c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c, float alpha) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
