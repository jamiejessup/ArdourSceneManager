


/*
 * Author: Jamie Jessup
 * File: jackMIDI.h
 * Description: Source Code for handling MIDI Events related to Ardour mixer levels
 */

#ifndef JACK
#define JACK

#include <iostream>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <cmath>
#include "midi.h"
#include <vector>

#define CC_MASK 0b10110000
#define ARDOUR_MIDI_CTL_IN "ardour:MIDI control in"
#define ARDOUR_MIDI_CTL_OUT "ardour:MIDI control out"

#define MASTER_CC 119

extern clock_t begin, end;
extern double time_spent;

class MainWindow;
class Scene;

class Jack
{
	float bpm;
	Scene *pMyScene;
	MainWindow* pASMView;
	float midiCCToFaderGain(char);
	int process(jack_nframes_t nframes);
	int playbackIndex;

	jack_client_t*	client;
	jack_port_t*		inputPort;
	jack_port_t*		outputPort;

	public:
		Jack(MainWindow*);
		~Jack();
		
		void activate();
		void setScene(Scene*);
		static int staticProcess(jack_nframes_t nframes, void *arg);
		std::vector<MidiEvent> eventVector;
	
};

#endif 
