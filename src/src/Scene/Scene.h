/*
 * Scene.h
 *
 *  Created on: 2013-01-12
 *      Author: jamie
 */

#ifndef SCENE_H_
#define SCENE_H_

#define ARDOUR_PORT "3819"
#define UNITY_GAIN 0
#define MASTER_URI 318

#include <iostream>
#include <vector>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "../JackMIDI/jackMIDI.h"
#include "Track/Track.h"


using namespace std;

class Scene {
	string name;
	Jack *pJack;
public:
	//Tracks
	Track master;
	vector<Track> tracks;
	//Constructor and destructor
	Scene(Jack *pJack);
	virtual ~Scene();
	//Sends all track parameters to ardour with Jack MIDI
	void sendSceneToArdour(void);
	//Reads a file and saves track parameters
	void setName(string nm);
	//Get the name of the scene
	string getName();
	//Get the number of tracks that have been updated since thier initialization.
	int getUpdatedTracks();
};

#endif /* SCENE_H_ */
