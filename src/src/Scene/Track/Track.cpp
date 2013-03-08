/*
 * Track.cpp
 *
 *  Created on: 2013-01-12
 *      Author: jamie
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

void Track::SetTrackGain(char newGain) {
	pthread_mutex_lock(&rxMutex);
	gain = newGain;
	pthread_mutex_unlock(&rxMutex);
}

char Track::GetTrackGain(void) {
	return gain;
}

void Track::SendTrack(Jack *pJack, int destTrack) {
	char *data = new char[3];
	data[0] = CC_MASK;
	data[1] = (char) destTrack;
	data[2] = gain;
	pthread_mutex_lock(&txMutex);
	pJack->eventVector.push_back(MidiEvent(data));
	pthread_mutex_unlock(&txMutex);
}

void Track::SetModified(bool mod) {
	pthread_mutex_lock(&rxMutex);
	modified = mod;
	pthread_mutex_unlock(&rxMutex);
}

bool Track::GetModified() {
	return modified;
}
