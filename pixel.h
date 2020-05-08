#ifndef PIXEL_H
#define PIXEL_H

class Color {
  public:
    inline Color();
    inline Color(char r, char g, char b);
    char r;
    char g;
    char b;
};

class Pixel {
  public:
    inline Pixel();
    inline Pixel(char r, char g, char b, wxCoord x, wxCoord y);
    inline Pixel(Color &&color, wxPoint &p);
    inline Pixel(Color &color, wxPoint &p);

    Color color;
    wxCoord x;
    wxCoord y;
};

inline Pixel::Pixel() {}

inline Pixel::Pixel(char r, char g, char b, wxCoord x, wxCoord y) {
  this->color.r = r;
  this->color.g = g;
  this->color.b = b;
  this->x = x;
  this->y = y;
}

inline Pixel::Pixel(Color &&color, wxPoint &p) {
  this->color = color;
  this->x = p.x;
  this->y = p.y;
}

inline Pixel::Pixel(Color &color, wxPoint &p) {
  this->color = color;
  this->x = p.x;
  this->y = p.y;
}

inline Color::Color() {}

inline Color::Color(char r, char g, char b) {
  this->r = r;
  this->g = g;
  this->b = b;
}

#endif
