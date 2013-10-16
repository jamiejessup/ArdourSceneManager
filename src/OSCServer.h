/*
#ifndef OSCSERVER_H
#define OSCSERVER_H

#include <iostream>
#include "lo/lo.h"
#include "JackMIDI/midi.h"


class OSCServer
{
    lo_address ardour;
    lo_address touchOSC;
    lo_server_thread serverThread;
    static int messageHandler(const char *path, const char *types, lo_arg ** argv,
                       int argc, void *data, void *user_data);
    std::vector<MidiEvent> *eventVector;

public:
    OSCServer(std::vector<MidiEvent> * ev);
    ~OSCServer();
};

#endif // OSCSERVER_H
*/
