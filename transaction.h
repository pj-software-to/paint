#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include "pixel.h"

class Transaction {
  private:
    std::vector<Pixel *> pixels;

  public:
    inline Transaction();
    inline void update(Pixel *p); 
};

inline Transaction::Transaction() {}

inline void Transaction::update(Pixel *p) {
  pixels.push_back(p);
}

#endif /* TRANSACTION_H */
