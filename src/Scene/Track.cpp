#include "Track.h"

void Track::sendToArdour(Jack *pJack) {
    char *data = new char[3];
    data[0] = CC_MASK;
    data[1] = (char) id;
    data[2] = gain;
    pthread_mutex_lock(&txMutex);
    pJack->eventVector.push_back(MidiEvent(data));
    pthread_mutex_unlock(&txMutex);
}

void Track::setSoled(bool mod){
    soloed = mod;
}

bool Track::getSoloed(){
    return soloed;
}

void Track::setMuted(bool mod) {
    muted = mod;
}

bool Track::getMuted() {
    return muted;
}

char Track::getPanDir(void) {
    return panDir;
}

void Track::setPanDir(char newPanDir) {
    panDir = newPanDir;
}

char Track::getPanWidth(void){
    return panWidth;
}

void Track::setPanWidth(char newPanWidth) {
    panWidth = newPanWidth;
}
