/*
 * Scene.cpp
 *
 *  Created on: 2013-01-12
 *      Author: jamie
 */

#include "Scene.h"

Scene::Scene(Jack * pJackIn) {
	pJack = pJackIn;
}


void Scene::sendSceneToArdour() {
	//Send vale of master track Gain
	master.SendTrack(pJack, MASTER_CC);
	//Send the values of the rest for the tracks
	for (int i = 0; i < (int) tracks.size(); i++) {
		tracks[i].SendTrack(pJack, i+1);
	}
}


void Scene::setName(string nm) {
	name = nm;
}

string Scene::getName() {
	return name;
}

int Scene::getUpdatedTracks() {
	int updated = 0;
	if (master.GetModified())
		updated++;
	for (int i = 0; i < (int) tracks.size(); i++) {
		if (tracks[i].GetModified())
			updated++;
	}
	return updated;
}

Scene::~Scene() {

}
