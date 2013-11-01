#include "Master.h"

void Master::sendToArdour(Jack *pJack) {
    char *data = new char[3];
    data[0] = CC_MASK;
    data[1] = id;
    data[2] = gain;

    MidiEvent midiEvent(data);
    //Send it to the jack client to handle send to Ardour
    jack_ringbuffer_write(pJack->sceneLoadBuffer, (char *) &midiEvent,sizeof(MidiEvent));

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
