#ifndef EDIT_DISTANCE_
#define EDIT_DISTANCE_

#include "TextDistanceInterface.h"

class BasicLevenshteinDistance : public TextDistanceInterface
{
  public:
    virtual int calcualteDistance (char const *s, char const *t);
  private:
    int minimum (int a, int b, int c);
    int *getCellPointer (int *pOrigin, int col, int row, int nCols);
    int getAt (int *pOrigin, int col, int row, int nCols);
    void putAt (int *pOrigin, int col, int row, int nCols, int x);
};

#endif
