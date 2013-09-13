#include "ArdourControllable.h"

ArdourControllable::ArdourControllable()
{
    gain = 0;
    modified = false;
}

ArdourControllable::ArdourControllable(char newGain)
{
    gain = newGain;
    modified = false;
}
