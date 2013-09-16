#ifndef TRACK_H
#define TRACK_H

#include "ArdourControllable.h"

class Track : public ArdourControllable
{
public:
    void sendToArdour(Jack *pJack);
    Track(char id) : ArdourControllable(id) {}
    Track(char id,char gain) : ArdourControllable(id,gain) {}
};

#endif // TRACK_H
