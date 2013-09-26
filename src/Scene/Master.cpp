#include "Master.h"

void Master::sendToArdour(Jack *pJack) {
    char *data = new char[3];
    data[0] = CC_MASK;
    data[1] = id;
    data[2] = gain;
    pthread_mutex_lock(&txMutex);
    pJack->eventVector.push_back(MidiEvent(data));
    for(unsigned i = 0; i<sends.size(); i++) {
        sends[i].sendToArdour(pJack);
    }
    pthread_mutex_unlock(&txMutex);
}

void Master::setMuted(bool mod) {
    muted = mod;
}

bool Master::getMuted() {
    return muted;
}

char Master::getPanDir(void) {
    return panDir;
}

void Master::setPanDir(char newPanDir) {
    panDir = newPanDir;
}

char Master::getPanWidth(void){
    return panWidth;
}

void Master::setPanWidth(char newPanWidth) {
    panWidth = newPanWidth;
}
