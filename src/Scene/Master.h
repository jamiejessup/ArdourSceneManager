#ifndef MASTER_H
#define MASTER_H

#include "ArdourControllable.h"

#define MASTER_ID 119

class Master : public ArdourControllable
{
    bool muted;
    char panDir;
    char panWidth;
public:
    void setMuted(bool mod=true);
    bool getMuted();
    char getPanDir(void);
    void setPanDir(char);
    char getPanWidth(void);
    void setPanWidth(char);
    void sendToArdour(Jack *pJack);
    Master() : ArdourControllable((char) MASTER_ID) {}
    Master(char gain) : ArdourControllable((char) MASTER_ID,gain) {}
};

#endif // MASTER_H
