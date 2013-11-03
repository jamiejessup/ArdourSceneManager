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

    MidiEvent midiEvent(data);
    //Send it to the jack client to handle send to Ardour
    jack_ringbuffer_write(pJack->sceneLoadBuffer, (char *) &midiEvent,sizeof(MidiEvent));

    std::cout << "Send a send on channel " << (int) id << std::endl;
}
