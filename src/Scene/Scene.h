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

#ifndef SCENE_H_
#define SCENE_H_

#define ARDOUR_PORT "3819"
#define UNITY_GAIN 0
#define MASTER_ID 119

#include <iostream>
#include <vector>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include "../JackMIDI/jackMIDI.h"
#include "Track.h"
#include "Master.h"


using namespace std;

class Scene {
	string name;
	Jack *pJack;
public:
	//Tracks
    Master master;
    vector<Track> tracks;
    vector<Track> busses;
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
    int getUpdatedBusses();
};

#endif /* SCENE_H_ */
