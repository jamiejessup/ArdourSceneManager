#include "OSCServer.h"

OSCServer::OSCServer(jack_ringbuffer_t *cb)
{
    touchOSC = NULL;
    contacted = false;
    controllerBuffer = cb;
    trackBank = busBank = 0;
    numTrackBanks = numBusBanks = 1;

    pthread_mutex_init(&idMutex, NULL);
    pthread_mutex_init(&trackBankMutex, NULL);
    pthread_mutex_init(&busBankMutex, NULL);

    activate(1,1);
}

void OSCServer::activate(int numTB, int numBB){
    numTrackBanks = numTB;
    numBusBanks = numBB;

    if(!started){
        //start a new server on port 3820 will change to config file
        serverThread = lo_server_thread_new("8000", NULL);

        /* add method that will match any path and args */
        lo_server_thread_add_method(serverThread, NULL, NULL, staticHandler, static_cast<void*>(this));

        lo_server_thread_start(serverThread);
        started = true;
    }

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
            char data[3];
            if(pthread_mutex_lock(&idMutex) == 0) {
                data[0] = (char) CC_MASK;
                data[1] = trackIds[atoi(pathStr.c_str()) - 1];
                data[2] = (char) ((int) argv[0]->f);

                pthread_mutex_unlock(&idMutex);
            }
            MidiEvent midiEvent(data,true);
            //Send it to the jack client to handle send to Ardour
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

        if(controllable == "bank") {
            if (argv[0]->f > 0.5){
                pathStr= pathStr.substr(pathStr.find("/") + 1);
                if(pathStr == "up"){
                    int tb = getTrackBank();
                    if((tb+1) < numTrackBanks) {
                        sendTrackBank(tb+1);
                        setTrackBank(tb+1);
                    }
                }
                else if(pathStr == "down"){
                    int tb = getTrackBank();
                    if(tb > 0) {
                        sendTrackBank(tb-1);
                        setTrackBank(tb-1);
                    }
                }

            }
        }


    } else if (object == "bus") {

        if(controllable == "fader") {
            //get the bus number(void *)this
            pathStr= pathStr.substr(pathStr.find("/") + 1);
            char data[3];
            if(pthread_mutex_lock(&idMutex) == 0) {
                data[0] = (char) CC_MASK;
                data[1] = busIds[atoi(pathStr.c_str())-1];
                data[2] = (char) ((int) argv[0]->f);

                pthread_mutex_unlock(&idMutex);
            }
            MidiEvent midiEvent(data,true);
            //Send it to the jack client to handle send to Ardour
            //Send it to the jack client to handle send to Ardour
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

        if(controllable == "bank") {
            if (argv[0]->f > 0.5){
                pathStr= pathStr.substr(pathStr.find("/") + 1);
                if(pathStr == "up"){
                    int bb = getBusBank();
                    if((bb+1) < numBusBanks) {
                        sendBusBank(bb+1);
                        setBusBank(bb+1);
                    }
                }
                else if(pathStr == "down"){
                    int bb = getBusBank();
                    if(bb > 0) {
                        sendBusBank(bb-1);
                        setBusBank(bb-1);
                    }
                }

            }
        }


    }
    else if (object == "master") {
        if(controllable == "fader") {
            //build a midi event from the fader value and track number
            char data[3] = {(char) CC_MASK,MASTER_CC,(char)((int) argv[0]->f)};
            MidiEvent midiEvent(data,true);
            //Send it to the jack client to handle send to Ardour
            jack_ringbuffer_write(controllerBuffer, (char *) &midiEvent,sizeof(MidiEvent));
        }

    }

    else if (object == "scene") {
    }

    return 0;
}

void OSCServer::firstContact(lo_address addr) {
    touchOSC = lo_address_new(lo_address_get_hostname(addr),"9000");
    sendTrackBank(trackBank);
    sendBusBank(busBank);
    sendMaster();
}

void OSCServer::sendTrackBank(int bankNumber) {
    strcpy(cu.what,"trackBank");
    cu.bankIndex = bankNumber;
    jack_ringbuffer_write(controllerUpdate,(char *) &cu, sizeof(ControllerUpdateEvent));
    lo_send(touchOSC,"/controller/track/bank/number","s",std::string(std::to_string(bankNumber+1)).c_str());
    for(int i=0; i<8; i++){
        lo_send(touchOSC,(std::string("/controller/track/label/").append(std::to_string(i+1))).c_str(),"s",
                (std::string("Track ").append(std::to_string(bankNumber*8+i+1))).c_str());
    }
}

void OSCServer::sendBusBank(int bankNumber) {
    //Request an update for the bus bank faders
    strcpy(cu.what,"busBank");
    cu.bankIndex = bankNumber;
    jack_ringbuffer_write(controllerUpdate,(char *) &cu, sizeof(ControllerUpdateEvent));
    lo_send(touchOSC,"/controller/bus/bank/number","s",std::string(std::to_string(bankNumber+1)).c_str());
    for(int i=0; i<8; i++){
        lo_send(touchOSC,(std::string("/controller/bus/label/").append(std::to_string(i+1))).c_str(),"s",
                (std::string("Track ").append(std::to_string(bankNumber*8+i+1))).c_str());
    }
}

void OSCServer::sendMaster(){
    strcpy(cu.what,"master");
    jack_ringbuffer_write(controllerUpdate,(char *) &cu, sizeof(ControllerUpdateEvent));
}

void OSCServer::sendToController(std::string &path, float value){
    if(touchOSC!=NULL){
        lo_send(touchOSC, path.c_str(), "f", value);
    }
}


void OSCServer::setTrackIds(char *data) {
    if(pthread_mutex_lock(&idMutex) == 0) {
        for(int i=0; i<8; i++){
            trackIds[i] = *(data++);
        }
        pthread_mutex_unlock(&idMutex);
    }
}

void OSCServer::setBusIds(char *data) {
    if(pthread_mutex_lock(&idMutex) == 0) {
        for(int i=0; i<4; i++) {
            busIds[i] = *(data++);
        }
        pthread_mutex_unlock(&idMutex);
    }
}

void OSCServer::setIds(char *trackData, char *busData){
    setTrackIds(trackData);
    setBusIds(busData);
}

void OSCServer::setTrackBank(int tb) {
    if(pthread_mutex_lock(&trackBankMutex) == 0) {
        trackBank = tb;
        pthread_mutex_unlock(&trackBankMutex);
    }
}

void OSCServer::setBusBank(int bb){
    if(pthread_mutex_lock(&busBankMutex) == 0) {
        busBank = bb;
        pthread_mutex_unlock(&busBankMutex);
    }
}

int OSCServer::getTrackBank(){
    int tb;
    if(pthread_mutex_lock(&trackBankMutex) == 0) {
        tb = trackBank;
        pthread_mutex_unlock(&trackBankMutex);
    }
    return tb;
}

int OSCServer::getBusBank(){
    int bb;
    if(pthread_mutex_lock(&busBankMutex) == 0) {
        bb = busBank;
        pthread_mutex_unlock(&busBankMutex);
    }
    return bb;
}
