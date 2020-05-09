#ifndef __HELPER_H__
#define __HELPER_H__

#include <math.h>

typedef wxPoint wxVec;
typedef wxRealPoint wxRealVec;

inline double length(const wxPoint &p1, const wxPoint &p2) {
  wxVec v = p1-p2;
  return sqrt((double)(v.x*v.x + v.y*v.y));
}

inline wxRealVec normalize(const wxRealVec &v) {
  double len = sqrt(v.x*v.x + v.y*v.y);
  return wxRealVec(v.x/len, v.y/len);
}

#endif /* __HELPER_H__ */