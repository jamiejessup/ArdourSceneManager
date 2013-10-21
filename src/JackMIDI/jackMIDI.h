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

#ifndef JACK
#define JACK

#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#include <cmath>
#include "midi.h"
#include <vector>
#include "../ControllerEvent.h"


#define ARDOUR_MIDI_CTL_IN "ardour:MIDI control in"
#define ARDOUR_MIDI_CTL_OUT "ardour:MIDI control out"
#define CC_MASK 0b10110000
#define CC_NIBBLE 0b1011
#define NOTE_ON_NIBBLE 0b1001
#define NOTE_OFF_NIBBLE 0b100


//Two defines because sometimes Ardour outputs on one less than the midi map
#define MASTER_CC 119
#define MASTER_CC2 118

class ASMView;
class Scene;

class Jack
{
    float bpm;
    ASMView* pASMView;
    float midiCCToFaderGain(char);
    int process(jack_nframes_t nframes);
    int playbackIndex;
    ControllerEvent controllerEvent;

    jack_client_t*	client;
    jack_port_t*		inputPort;
    jack_port_t*		outputPort;

public:
    Jack(ASMView*);
    ~Jack();
    void activate();

    static int staticProcess(jack_nframes_t nframes, void *arg);
    std::vector<MidiEvent> eventVector;
    jack_ringbuffer_t *controllerBuffer;
    std::vector<MidiEvent> *getEventVector();
    jack_ringbuffer_t *sceneUpdateBuffer;

};

#endif 
