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
    modified = mod;
}

bool ArdourControllable::getModified() {
    return modified;
}

void ArdourControllable::setGain(char newGain) {
    gain = newGain;
}

char ArdourControllable::getGain(void) {
    return gain;
}

char ArdourControllable::getId(void) {
    return id;
}

void ArdourControllable::setId(char newId) {
    id = newId;
}
