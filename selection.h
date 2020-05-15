//
// Created by Patricia M Marukot on 2020-05-10.
//

#ifndef PAINT_SELECTION_H
#define PAINT_SELECTION_H

#include <limits>

#include "pixel.h"
#include "helper.h"
#include "interpolation.h"

/*
 * Base class that contains fcn isWithinBounds()
 * which should be overridden by all child classes
 * The function should return true if the point is
 * within the selection area. False otherwise.
 */
class Selection {
public:
  wxCoord minX;
  wxCoord minY;
  wxCoord maxX;
  wxCoord maxY;

  inline virtual bool isWithinBounds(wxPoint &point);

  inline int getHeight() {
    return (int)(maxY - minY) + 1;
  }

  inline int getWidth() {
    return (int)(maxX - minX) + 1;
  }
};

/*
 * Derived classes
 * One for each type of selection:
 *    Rectangle, Circle, Lasso (freehand)
 * Each class contains shape specific
 * information about the selection.
 * As per above, all classes override
 * the isWithinBounds() fcn.
 */
class RectangleSelection : public Selection {
public:
  inline RectangleSelection();
  inline RectangleSelection(wxPoint p0, wxPoint p1);

  inline bool isWithinBounds(wxPoint &point);
};

class CircleSelection : public Selection {
public:
  wxPoint _c;
  double _r;

  inline CircleSelection(wxPoint p0, wxPoint p1);

  inline bool isWithinBounds(wxPoint &point);
};

class LassoSelection : public Selection {
public:
  int n;
  std::vector<wxPoint> border;

  inline LassoSelection(wxPoint p0, wxPoint p1,
      std::vector<wxPoint> &_border);

  inline bool isWithinBounds(wxPoint &point);
};

/************** Selection ****************/
/* Never actually used, just return true */
inline bool Selection::isWithinBounds(wxPoint &point) {
  return true;
}

/************** RectangleSelection ****************/
inline RectangleSelection::RectangleSelection(){}
inline RectangleSelection::RectangleSelection(wxPoint p0, wxPoint p1) {
  minX = p0.x < p1.x ? p0.x : p1.x;
  maxX = p0.x > p1.x ? p0.x : p1.x;
  minY = p0.y < p1.y ? p0.y : p1.y;
  maxY = p0.y > p1.y ? p0.y: p1.y;
}

inline bool RectangleSelection::isWithinBounds(wxPoint &pt) {
  if (pt.x < minX || pt.x > maxX
    || pt.y < minY || pt.y > maxY)
    return false;
  return true;
}

/************** CircleSelection ****************/
inline CircleSelection::CircleSelection(wxPoint p0, wxPoint p1){
  _r = length(p1, p0)/2;

  wxRealVec v;
  wxVec _v = p1 - p0;
  v = wxRealVec((double)_v.x, (double)_v.y);
  v = normalize(v);

  wxRealPoint _curr(p0);
  _c = wxPoint(_curr + _r*v);

  minX = _c.x - _r;
  maxX = _c.x + _r;
  minY = _c.y - _r;
  maxY = _c.y + _r;
}

inline bool CircleSelection::isWithinBounds(wxPoint &point)
{
  return (squaredLength(point, _c) < _r*_r);
}

/************** LassoSelection ****************/
inline LassoSelection::LassoSelection(wxPoint p0, wxPoint p1,
    std::vector <wxPoint> &_border)
{
  border = _border;
  n = border.size();

  if (n == 0) {
    minX = 0;
    minY = 0;
    maxX = 0;
    maxY = 0;
    return;
  }

  int i=0;
  minX = std::numeric_limits<int>::max();
  minY = std::numeric_limits<int>::max();
  maxX = -1;
  maxY = -1;
  for (i=0; i < n; i++) {
    minX = std::min(minX, border[i].x);
    minY = std::min(minY, border[i].y);
    maxX = std::max(maxX, border[i].x);
    maxY = std::max(maxY, border[i].y);
  }
}

inline bool LassoSelection::isWithinBounds(wxPoint &point) {
  int i, j, x, y;
  x = point.x;
  y = point.y;
  bool c = false;
  for (i=0, j=n-1; i < n; i++) {
    if (((border[i].y > y) != (border[j].y > y))
      && ((x < border[i].x + (border[j].x - border[i].x) *
            (y - border[i].y) / (border[j].y - border[i].y)))) {
      c = !c;
    }
    j = i;
  };
  return c;
}

#endif //PAINT_SELECTION_H
