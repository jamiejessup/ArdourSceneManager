/*
#include "OSCServer.h"

OSCServer::OSCServer(std::vector<MidiEvent> *ev)
{
    eventVector = ev;
    //start a new server on port 3820 will change to config file
    serverThread = lo_server_thread_new("3820", error);
    lo_server_thread_add_method(serverThread, NULL, NULL, messageHandler, NULL);

    lo_server_thread_start(serverThread);
}

OSCServer::~OSCServer() {
    lo_server_thread_free(serverThread);
}

static int OSCServer::messageHandler(const char *path, const char *types, lo_arg ** argv,
                                     int argc, void *data, void *user_data)
{
    std::cout << "Hello, I should be sending this to Ardour" << std::endl;

}
*/
