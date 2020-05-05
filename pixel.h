#ifndef PIXEL_H
#define PIXEL_H

class Color {
  public:
    char r;
    char g;
    char b;
};

class Pixel {
  public:
    inline Pixel(char r, char g, char b, wxCoord x, wxCoord y);
    Color color;
    wxCoord x;
    wxCoord y;
};

inline Pixel::Pixel(char r, char g, char b, wxCoord x, wxCoord y) {
  this->color.r = r;
  this->color.g = g;
  this->color.b = b;
  this->x = x;
  this->y = y;
}

#endif
