/*
 * midi.h
 *
 *  Created on: 2013-03-06
 *      Author: jamie
 */

#ifndef MIDI_H_
#define MIDI_H_
#include <string.h>


class MidiEvent {
public:
	MidiEvent(char *inData) {
		memcpy(data, inData, sizeof(char)*3);
	}
	char data[3];
};

#endif /* MIDI_H_ */
