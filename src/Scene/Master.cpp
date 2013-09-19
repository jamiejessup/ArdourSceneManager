#include "Master.h"

void Master::sendToArdour(Jack *pJack) {
    char *data = new char[3];
    data[0] = CC_MASK;
    data[1] = id;
    data[2] = gain;
    pthread_mutex_lock(&txMutex);
    pJack->eventVector.push_back(MidiEvent(data));
    pthread_mutex_unlock(&txMutex);
}

