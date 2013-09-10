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

#ifndef TRACK_H_
#define TRACK_H_
#include "stdio.h"
#include "../JackMIDI/jackMIDI.h"
#include "Send.h"

#define CC_MASK 0b10110000

//Reference to the mutexes
extern pthread_mutex_t txMutex;
extern pthread_mutex_t rxMutex;

class Track {
	char gain;
	bool modified;
    bool muted;
    bool soloed;
    char panDirection;
    char panWidth;
    std::vector<Send> sends;
public:
	Track();
    Track(char gain);
    Track(char gain, std::vector<char> sendGains);
	//Get and set methods
	char getTrackGain(void);
	void setTrackGain(char);
	void sendTrack(Jack *pJack, int destTrack);
	void setModified(bool mod=true);
	bool getModified();
};

#endif /* TRACK_H_ */
