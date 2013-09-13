#ifndef ARDOURCONTROLLABLE_H
#define ARDOURCONTROLLABLE_H

#include "../JackMIDI/jackMIDI.h"

class ArdourControllable
{
protected:
    char gain;
    bool modified;
    char id;
public:
    ArdourControllable();
    ArdourControllable(char newGain);
    virtual void sendToArdour(Jack *pJack) =0;
};

#endif // ARDOURCONTROLLABLE_H
