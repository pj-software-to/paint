//
// Created by Patricia M Marukot on 2020-05-10.
//

#ifndef PAINT_SELECTION_H
#define PAINT_SELECTION_H
/*
 * Base class that contains fcn isWithinBounds()
 * which should be overridden by all child classes
 * The function should return true if the point is
 * within the selection area. False otherwise.
 */
class Selection {
public:
  inline virtual bool isWithinBounds(wxPoint &point);
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
  wxCoord minX;
  wxCoord minY;
  wxCoord maxX;
  wxCoord maxY;

  inline RectangleSelection();
  inline RectangleSelection(wxPoint p0, wxPoint p1);
  inline bool isWithinBounds(wxPoint &point);
};

class CircleSelection : public Selection {
public:
  wxPoint _c;
  int _r;
  bool isWithinBounds(wxPoint &point);
};

class LassoSelection : public Selection {
public:
  bool isWithinBounds(wxPoint &point);
};


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

inline bool Selection::isWithinBounds(wxPoint &pt) {
  return true;
}
#endif //PAINT_SELECTION_H
