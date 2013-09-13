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

#include "Track.h"

Track::Track() {
	gain = 0;
	modified = false;
}

Track::Track(char newGain) {
	gain = newGain;
	modified = false;
}


void Track::setTrackGain(char newGain) {
	pthread_mutex_lock(&rxMutex);
	gain = newGain;
	pthread_mutex_unlock(&rxMutex);
}

char Track::getTrackGain(void) {
	return gain;
}

void Track::sendTrack(Jack *pJack, int destTrack) {
	char *data = new char[3];
	data[0] = CC_MASK;
	data[1] = (char) destTrack;
	data[2] = gain;
	pthread_mutex_lock(&txMutex);
	pJack->eventVector.push_back(MidiEvent(data));
	pthread_mutex_unlock(&txMutex);
}

void Track::setModified(bool mod) {
	pthread_mutex_lock(&rxMutex);
	modified = mod;
	pthread_mutex_unlock(&rxMutex);
}

bool Track::getModified() {
	return modified;
}
