#include "Send.h"

Send::Send(char newTrackId) {
    trackId = newTrackId;
}

Send::Send(char newTrackId, char sendId) : ArdourControllable(sendId) {
    trackId = newTrackId;
}

Send::Send(char newTrackId, char sendId, char newGain) : ArdourControllable(sendId,newGain) {
    trackId = newTrackId;
}

void Send::sendToArdour(Jack *pJack) {
    char *data = new char[3];
    data[0] = CC_MASK + id;
    data[1] = (char) trackId;
    data[2] = gain;
    pthread_mutex_lock(&midiMutex);
    pJack->eventVector.push_back(MidiEvent(data));
    pthread_mutex_unlock(&midiMutex);
}
