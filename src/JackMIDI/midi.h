/*
This file is part of Ardour Scene Manager.
Copyright 2013, Jamie Jessup

Ardour Scene Manager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ardour Scene Manager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ardour Scene Manager. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MIDI_H_
#define MIDI_H_
#include <string.h>


class MidiEvent {
public:
    char data[3];
    bool fromController;
    MidiEvent(char *inData, bool fromCtl=false) {
        memcpy(data, inData, sizeof(char)*3);
        fromController = fromCtl;
    }
    MidiEvent(){
        fromController = false;
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
    }
};
#endif /* MIDI_H_ */
