#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <string>

//Do subclasses later for handling name updates on init

class ControllerUpdateEvent {
public:
    std::string what;
    int bankIndex;
    ControllerUpdateEvent(std::string w, float in) {
        what = w;
        bankIndex = in;
    }
};

#endif // CONTROLLEREVENT_H
