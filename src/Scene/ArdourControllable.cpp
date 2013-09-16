#include "ArdourControllable.h"

ArdourControllable::ArdourControllable()
{
    id = 0;
    gain = 0;
    modified = false;
}

ArdourControllable::ArdourControllable(char newId) {
    id = newId;
    gain = 0;
    modified = false;
}

ArdourControllable::ArdourControllable(char newId, char newGain)
{
    id = newId;
    gain = newGain;
    modified = false;
}

void ArdourControllable::setModified(bool mod) {
    pthread_mutex_lock(&rxMutex);
    modified = mod;
    pthread_mutex_unlock(&rxMutex);
}

bool ArdourControllable::getModified() {
    return modified;
}

void ArdourControllable::setGain(char newGain) {
    pthread_mutex_lock(&rxMutex);
    gain = newGain;
    pthread_mutex_unlock(&rxMutex);
}

char ArdourControllable::getGain(void) {
    return gain;
}
