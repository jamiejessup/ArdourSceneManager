#ifndef ARDOURCONTROLLABLE_H
#define ARDOURCONTROLLABLE_H

#include "../JackMIDI/jackMIDI.h"
#include <jack/ringbuffer.h>

//Reference to the mutexes
extern pthread_mutex_t midiMutex;
class ArdourControllable
{
protected:
    char gain;
    bool modified;
    char id;
    void setId(char);
public:
    char getGain(void);
    void setGain(char);
    void setModified(bool mod=true);
    virtual bool getModified();
    char getId(void);
    ArdourControllable(char id);
    ArdourControllable();
    ArdourControllable(char id, char newGain);
    virtual void sendToArdour(Jack *pJack) =0;
};

#endif // ARDOURCONTROLLABLE_H
