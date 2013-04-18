/*
 * Track.h
 *
 *  Created on: 2013-01-12
 *      Author: jamie
 */

#ifndef TRACK_H_
#define TRACK_H_
#include "lo/lo.h"
#include "stdio.h"
#include "../../JackMIDI/jackMIDI.h"

#define CC_MASK 0b10110000

//Reference to the mutexes
extern pthread_mutex_t txMutex;
extern pthread_mutex_t rxMutex;

class Track {
	char gain;
	bool modified;
public:
	Track();
	Track(char);
	//Get and set methods
	char getTrackGain(void);
	void setTrackGain(char);
	void sendTrack(Jack *pJack, int destTrack);
	void setModified(bool mod=true);
	bool getModified();
};

#endif /* TRACK_H_ */
