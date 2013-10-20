#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <string>

class ControllerEvent {
public:
    std::string path;
    float value;
    ControllerEvent(std::string p, float val) {
        path = p;
        value = val;
    }
};

#endif // CONTROLLEREVENT_H
