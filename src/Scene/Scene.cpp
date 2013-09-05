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

#include "Scene.h"

Scene::Scene(Jack * pJackIn) {
	pJack = pJackIn;
}


void Scene::sendSceneToArdour() {
	//Send vale of master track Gain
	master.sendTrack(pJack, MASTER_CC);
	//Send the values of the rest for the tracks
	for (int i = 0; i < (int) tracks.size(); i++) {
		tracks[i].sendTrack(pJack, i+1);
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
	if (master.getModified())
		updated++;
	for (int i = 0; i < (int) tracks.size(); i++) {
		if (tracks[i].getModified())
			updated++;
	}
	return updated;
}

Scene::~Scene() {

}