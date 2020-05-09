//
// Created by Patricia M Marukot on 2020-05-06.
//

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <math.h>
#include <vector>
#include <iostream>

#include "helper.h"
#include "interpolation.h"

#define ABS(x) ((x) < 0 ? -(x) : x)

/*
 * This is a library to perform spline interpolation
 * between multiple points.
 * Used for MOUSE_MOVE events because the mouse capture
 * rate is too slow to generate a smooth drawing.
 */

/*
 * Performs LINEAR INTERPOLATION between 2 points
 * Formula:
 *    y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
 */
std::vector<wxPoint> lerp(wxPoint p0, wxPoint p1, int width)
{
  int x0, x1, y0, y1;
  if (p0.x < p1.x) {
    x0 = p0.x;
    x1 = p1.x;
    y0 = p0.y;
    y1 = p1.y;
  } else {
    x1 = p0.x;
    x0 = p1.x;
    y1 = p0.y;
    y0 = p1.y;
  }

  /* 
   * Adjusting thiccness of line through width param
   * This draws only a square centered around (x,y).
   * 
   * Note: 
   * Algorithm is inefficient due to multiple overlapping
   * pixels. For example, from the current implementation,
   * points (x,y) and (x+1,y) - which are next to one another
   * in pixel space - will have virtually identical surrounding
   * points. Not bothering to optimize this because despite this,
   * all drawing operations are still fast.
   */
  std::vector<wxPoint> points;
  auto thicc = [&points, &width](int x, int y) {
    int left, right;
    int top, bot;
    
    left = std::max(0, x - width/2);
    right = x + width/2;
    top = std::max(0, y - width/2);
    bot = y + width/2; 

    int i,j;
    for (i=left; i<right; i++) {
      for (j=top; j<bot; j++) {
        points.push_back(wxPoint(i, j));
      }
    }
  };

  {
    /* Interpolate along x axis */
    int sampleRate = ABS(p1.x - p0.x);
    int i, x, y;
    for (i=0; i < sampleRate; i++) {
      x = x0 + i;
      y = y0 + (x - x0) * (y1 - y0) / (x1 - x0);

      thicc(x, y);
    }
  }

  if (p0.y < p1.y) {
    x0 = p0.x;
    x1 = p1.x;
    y0 = p0.y;
    y1 = p1.y;
  } else {
    x1 = p0.x;
    x0 = p1.x;
    y1 = p0.y;
    y0 = p1.y;
  }
  {
    /* Interpolate along y axis */
    int sampleRate = ABS(p1.y - p0.y);
    int i, x, y;
    for (i=0; i < sampleRate; i++) {
      y = y0 + i;
      x = x0 + (x1 - x0) * (y - y0) / (y1 - y0);

      thicc(x, y);
    }
  }
  return points;
}