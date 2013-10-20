#include "OSCServer.h"

OSCServer::OSCServer(jack_ringbuffer_t *cb) : ce("",0)
{
    touchOSC = NULL;
    contacted = false;
    controllerBuffer = cb;
    trackBank = busBank = 0;
    numTrackBanks = numBusBanks = 100;

    // create an instance of a ringbuffer that will hold up to 20 integers,
    // let the pointer point to it
    ardourOSCBuffer = jack_ringbuffer_create( 200 * sizeof(ControllerEvent));

    // lock the buffer into memory, this is *NOT* realtime safe, do it before
    // using the buffer!
    int res = jack_ringbuffer_mlock(ardourOSCBuffer);

    // check if we've locked the memory successfully
    if ( res ) {
        std::cout << "Error locking memory!" << std::endl;
    }

    //start the buffer poller thread
    pthread_create(&thread, NULL, controllerOutThread, static_cast<void*>(this));

    //start a new server on port 3820 will change to config file
    serverThread = lo_server_thread_new("8000", NULL);


    /* add method that will match any path and args */
    lo_server_thread_add_method(serverThread, NULL, NULL, staticHandler, static_cast<void*>(this));

    lo_server_thread_start(serverThread);
}

OSCServer::~OSCServer() {
    lo_server_thread_free(serverThread);
}

int OSCServer::staticHandler(const char *path,
                             const char *types, lo_arg **argv, int argc,
                             void *data, void *user_data)
{
    return static_cast<OSCServer*>(user_data)->genericHandler(path,types,argv,argc,data);
}

int OSCServer::genericHandler(
        const char *path, const char *types,
        lo_arg **argv, int argc, void *data)
{
    if(!contacted) {
        firstContact(lo_message_get_source((lo_message)data));
        contacted = true;
    }

    //find out what this message is for (track bus or master)

    std::string pathStr(path);

    pathStr = pathStr.substr(1);
    pathStr = pathStr.substr(pathStr.find("/") + 1);

    std::string object = pathStr.substr(0,pathStr.find("/"));

    pathStr = pathStr.substr(pathStr.find("/") + 1);
    std::string controllable = pathStr.substr(0,pathStr.find("/"));

    if(object == "track") {
        if(controllable == "fader") {
            //get the track number
            pathStr= pathStr.substr(pathStr.find("/") + 1);
            //build a midi event from the fader value and track number
            char data[3] = {(char) CC_MASK,(char) atoi(pathStr.c_str()),(char) ((int) argv[0]->f)};
            MidiEvent midiEvent(data);
            //Send it to the jack client to handle send to Ardour
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

        if(controllable == "bank") {
            if (argv[0]->f > 0.5){
                pathStr= pathStr.substr(pathStr.find("/") + 1);
                if(pathStr == "up"){
                    if((trackBank+1) < numTrackBanks) {
                        sendTrackBank(++trackBank);
                    }
                }
                else if(pathStr == "down"){
                    if(trackBank > 0) {
                        sendTrackBank(--trackBank);
                    }
                }

            }
        }


    } else if (object == "bus") {

        if(controllable == "fader") {
            //get the bus number(void *)this
            pathStr= pathStr.substr(pathStr.find("/") + 1);
            //build a midi event from the fader value and track number
            char data[3] = {(char) CC_MASK,(char) atoi(pathStr.c_str()),(char) ((int) argv[0]->f)};
            MidiEvent midiEvent(data);
            //Send it to the jack client to handle send to Ardour
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

        if(controllable == "bank") {
            if (argv[0]->f > 0.5){
                pathStr= pathStr.substr(pathStr.find("/") + 1);
                if(pathStr == "up"){
                    if((busBank+1) < numBusBanks) {
                        sendBusBank(++busBank);
                    }
                }
                else if(pathStr == "down"){
                    if(busBank > 0) {
                        sendBusBank(--busBank);
                    }
                }

            }
        }


    }
    else if (object == "master") {
        if(controllable == "fader") {
            //build a midi event from the fader value and track number
            char data[3] = {(char) CC_MASK,MASTER_CC,(char)((int) argv[0]->f)};
            MidiEvent midiEvent(data);
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

    }

    else if (object == "scene") {
    }


    return 0;
}

void OSCServer::firstContact(lo_address addr) {
    touchOSC = lo_address_new(lo_address_get_hostname(addr),"9000");
}

void OSCServer::sendTrackBank(int bankNumber) {
    lo_send(touchOSC,"/controller/track/bank/number","s",std::string(std::to_string(bankNumber+1)).c_str());

}

void OSCServer::sendBusBank(int bankNumber) {
    lo_send(touchOSC,"/controller/bus/bank/number","s",std::string(std::to_string(bankNumber+1)).c_str());
}


void *OSCServer::controllerOutThread(void * data) {
    OSCServer *server = (OSCServer *) data;

    ControllerEvent ce("This is a really long string so I don't get into trouble",0.0);

    for(;;) {

        if(server->touchOSC!=NULL) {
            //check the ring buffer to see if there is anything
            unsigned availableRead = jack_ringbuffer_read_space(server->ardourOSCBuffer);

            if ( availableRead >= sizeof(ControllerEvent) ) {
                jack_ringbuffer_read(server->ardourOSCBuffer,(char*)&ce,sizeof(ControllerEvent));
                lo_send(server->touchOSC,ce.path.c_str(),"f",ce.value);
            }
        }

        usleep(2000);
    }

    pthread_exit(NULL);

}
