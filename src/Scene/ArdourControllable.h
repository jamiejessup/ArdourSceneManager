#ifndef ARDOURCONTROLLABLE_H
#define ARDOURCONTROLLABLE_H

#include "../JackMIDI/jackMIDI.h"

//Reference to the mutexes
extern pthread_mutex_t txMutex;
extern pthread_mutex_t rxMutex;

class ArdourControllable
{
protected:
    char gain;
    bool modified;
    char id;
public:
    char getGain(void);
    void setGain(char);
    void setModified(bool mod=true);
    bool getModified();
    ArdourControllable(char id);
    ArdourControllable();
    ArdourControllable(char id, char newGain);
    virtual void sendToArdour(Jack *pJack) =0;
};

#endif // ARDOURCONTROLLABLE_H
