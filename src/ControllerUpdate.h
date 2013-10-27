#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <string.h>
#include <string>

//Do subclasses later for handling name updates on init

class ControllerUpdateEvent {
public:
    char what[10];
    int bankIndex;
    ControllerUpdateEvent(char* w, float in) {
        strcpy(what,w);
        bankIndex = in;
    }
    ControllerUpdateEvent(){
        strcpy(what,"");
        bankIndex = 0;
    }
};

#endif // CONTROLLEREVENT_H
