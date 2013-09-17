#ifndef SEND_H
#define SEND_H

#include "ArdourControllable.h"

//Reference to the mutexes
extern pthread_mutex_t txMutex;
extern pthread_mutex_t rxMutex;

class Send : public ArdourControllable
{
    char trackId;
public:
    void sendToArdour(Jack *pJack);
    Send(char trackId);
    Send(char trackId, char sendId);
    Send(char trackId, char sendId, char gain);

};

#endif // SEND_H
