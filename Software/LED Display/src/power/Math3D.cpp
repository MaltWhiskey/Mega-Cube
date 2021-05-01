#include "Math3D.h"

#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/*------------------------------------------------------------------------------
 * Vector3 CLASS
 *----------------------------------------------------------------------------*/
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f){};
Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_){};
Vector3::Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z){};

// add, subtract (operator +, -, +=, -=)
Vector3 Vector3::operator+(const Vector3& v) const {
  return Vector3(x + v.x, y + v.y, z + v.z);
}
Vector3 Vector3::operator-(const Vector3& v) const {
  return Vector3(x - v.x, y - v.y, z - v.z);
}
Vector3& Vector3::operator+=(const Vector3& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
Vector3& Vector3::operator-=(const Vector3& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}
// negative (operator -)
Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }

// multiply, divide by scalar (operator *, /, *=, /=)
Vector3 Vector3::operator*(float s) const {
  return Vector3(x * s, y * s, z * s);
}
Vector3 Vector3::operator/(float s) const {
  return Vector3(x / s, y / s, z / s);
}
Vector3& Vector3::operator*=(float s) {
  x *= s;
  y *= s;
  z *= s;
  return *this;
}
Vector3& Vector3::operator/=(float s) {
  x /= s;
  y /= s;
  z /= s;
  return *this;
}

// cross product (operator *, *=)
Vector3 Vector3::cross(const Vector3& v) const {
  return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}
Vector3 Vector3::operator*(const Vector3& v) const { return cross(v); }
Vector3& Vector3::operator*=(const Vector3& v) { return *this = cross(v); }

// dot product (operator %)
float Vector3::dot(const Vector3& v) const {
  return x * v.x + y * v.y + z * v.z;
}
float Vector3::operator%(const Vector3& v) const { return dot(v); }

// normalize
Vector3& Vector3::normalize() { return *this /= magnitude(); }
Vector3 Vector3::normalized() const { return *this / magnitude(); }
// magnitude
float Vector3::magnitude() const { return sqrt(norm()); }
float Vector3::norm() const { return x * x + y * y + z * z; }

// rotate v by an angle and this vector holding an axis using Rodrigues formula
Vector3 Vector3::rotate(float angle, const Vector3& v) const {
  // Angle is in degree and is converted to radian by multiplying by 2PI/360
  float c = cosf(2 * M_PI / 360 * angle);
  float s = sinf(2 * M_PI / 360 * angle);
  // normalize this vector to get n hat
  Vector3 n = normalized();
  // (1-cos(0))(v.n)n + cos(0)v + sin(0)(n x v)
  return n * v.dot(n) * (1 - c) + v * c + n.cross(v) * s;
}

// Test if this vector (point) is within a spherical radius of v inclusive
bool Vector3::inside(const Vector3& v, float radius) const {
  return ((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) +
              (v.z - z) * (v.z - z) <=
          radius * radius);
}
// Test if this vector (point) is inside a box, low inclusive, high exclusive
bool Vector3::inside(const Vector3& l, const Vector3& h) const {
  return (x < h.x && x >= l.x) && (y < h.y && y >= l.y) &&
         (z < h.z && z >= l.z);
}

/*------------------------------------------------------------------------------
 * Quaternion CLASS
 *----------------------------------------------------------------------------*/
Quaternion::Quaternion() : w(0.0f), v(Vector3(0.0, 0.0, 0.0)) {}
// Make a quaternion from a scalar and a vector
Quaternion::Quaternion(float w_, const Vector3& v_) {
  w = w_;
  v = v_;
}
Quaternion::Quaternion(const Quaternion& q) : w(q.w), v(q.v) {}
// Make a unit quaternion from an axis as a vector and an angle
// Theoretically the magnitude of v_ can be used to specify the rotation
// Using an angle makes things more convenient
Quaternion::Quaternion(const Vector3& v_, float a) {
  // normalize v to get n hat (unit vector with length = 1)
  Vector3 n = v_.normalized();
  // Angle is in degree and is converted to radian by 2PI/360 * angle
  // Angles are divided by 2 when using quaternions so back to PI/360
  a = a * M_PI / 360;
  // Multiply n hat by sin(0) (scale n, but no directional change)
  // So v still represents the axis of rotation, but changed magnitude
  v = n * sinf(a);
  // Store cos(0) in the scalar part of the quaternion, results in a
  // unit quaternion => w^2+x^2+y^2+z^2=1, since sin(x)^2+cos(x)^2=1
  // Magnitude is always one. To stack rotations multiply unit
  // quaternions together and keep magnitude 1. So multiple rotations
  // without changing size of an object
  w = cosf(a);
}

// add, subtract (operator +, -, +=, -=)
Quaternion Quaternion::operator+(const Quaternion& q) const {
  return Quaternion(w + q.w, v + q.v);
}
Quaternion Quaternion::operator-(const Quaternion& q) const {
  return Quaternion(w - q.w, v - q.v);
}
Quaternion& Quaternion::operator+=(const Quaternion& q) {
  w += q.w;
  v += q.v;
  return *this;
}
Quaternion& Quaternion::operator-=(const Quaternion& q) {
  w -= q.w;
  v -= q.v;
  return *this;
}

// multiply, divide by scalar (operator *, /, *=, /=)
Quaternion Quaternion::operator*(float s) const {
  return Quaternion(w * s, v * s);
}
Quaternion Quaternion::operator/(float s) const {
  return Quaternion(w / s, v / s);
}
Quaternion& Quaternion::operator*=(float s) {
  w *= s;
  v *= s;
  return *this;
}
Quaternion& Quaternion::operator/=(float s) {
  w /= s;
  v /= s;
  return *this;
}

// multiply quaternions (operator *, /, *=)
Quaternion Quaternion::operator*(const Quaternion& q) const {
  Quaternion r;
  r.w = w * q.w - v.dot(q.v);
  r.v = v * q.w + q.v * w + v.cross(q.v);
  return r;
}
Quaternion Quaternion::operator/(const Quaternion& q) const {
  return *this * q.inversed();
}
Quaternion& Quaternion::operator*=(const Quaternion& q) {
  return *this = operator*(q);
}

// dot product (operator %)
float Quaternion::dot(const Quaternion& q) const {
  return w * q.w + v.dot(q.v);
}
float Quaternion::operator%(const Quaternion& q) const { return dot(q); }

// inverse
Quaternion& Quaternion::inverse() {
  conjugate();
  return *this *= 1 / norm();
}
Quaternion Quaternion::inversed() const { return conjugated() * (1 / norm()); }
// conjugate
Quaternion& Quaternion::conjugate() {
  v = -v;
  return *this;
}
Quaternion Quaternion::conjugated() const { return Quaternion(w, -v); }
// normalize
Quaternion& Quaternion::normalize() { return *this /= magnitude(); }
Quaternion Quaternion::normalized() const { return *this / magnitude(); }
// magnitude
float Quaternion::magnitude() const { return sqrt(norm()); }
float Quaternion::norm() const { return w * w + v.dot(v); }

// rotate v by this quaternion
Vector3 Quaternion::rotate(const Vector3& v) const {
  // creates a pure quaternion from a vector
  Quaternion p = Quaternion(0, v);
  // multiply (p)(q)(pi) and return vector part
  return ((*this) * p * (*this).inversed()).v;
}