//
// Created by Patricia M Marukot on 2020-05-10.
//

#ifndef PAINT_SELECTION_H
#define PAINT_SELECTION_H

#include "pixel.h"
#include "helper.h"

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

  inline virtual bool isWithinBounds(wxPoint &point) {}

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
  inline bool isWithinBounds(wxPoint &point) {}
};

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
#endif //PAINT_SELECTION_H
