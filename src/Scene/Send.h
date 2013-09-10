#ifndef SEND_H
#define SEND_H

#include "../JackMIDI/jackMIDI.h"

//Reference to the mutexes
extern pthread_mutex_t txMutex;
extern pthread_mutex_t rxMutex;

class Send
{
    char gain;
    bool modified;

public:
    Send();
    void sendSend(Jack *pJack, int sourceTrack, int sendNum);
    //Get and set methods
    char getTrackGain(void);
    void setTrackGain(char);
    void setModified(bool mod=true);
    bool getModified();
};

#endif // SEND_H
