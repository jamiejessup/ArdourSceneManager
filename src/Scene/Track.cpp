#include "Track.h"

void Track::sendToArdour(Jack *pJack) {

    char *data = new char[3];
    data[0] = CC_MASK;
    data[1] = (char) id;
    data[2] = gain;

    MidiEvent midiEvent(data);
    //Send it to the jack client to handle send to Ardour
    jack_ringbuffer_write(pJack->sceneLoadBuffer, (char *) &midiEvent,sizeof(MidiEvent));

    //Send our sends too
    for(unsigned i=0; i<sends.size(); i++){
        sends[i].sendToArdour(pJack);
    }


}

void Track::setSoloed(bool mod){
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

bool Track::getModified() {
    for(unsigned i = 0; i<sends.size(); i++){
        if(sends[i].getModified()){
            return true;
        }
    }

    return modified;
}
