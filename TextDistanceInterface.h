#ifndef K1_PARSING_TEXTDISTANCEINTERFACE_H
#define K1_PARSING_TEXTDISTANCEINTERFACE_H


class TextDistanceInterface {
public:
    virtual int calcualteDistance(char const *s, char const *t)=0;
};


#endif //K1_PARSING_TEXTDISTANCEINTERFACE_H
