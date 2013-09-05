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


#ifndef SCENEPARSER_H_
#define SCENEPARSER_H_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "../Scene/Scene.h"

using namespace std;

class SceneParser {
	//Saves all track parameters to a file with the same same as the scene.scene
	void saveScene(Scene *sourceScene, string fileName);
	void parseNodes(Scene *destScene, xmlNode *);
public:
	SceneParser();
	virtual ~SceneParser();
	//Reads a file and saves track parameters
	void loadSceneFromFile(Scene *destScene, string fileName);
	//Saves Scene to a different file name than the scene name
	void saveSceneToFile(Scene *sourceScene, string fileName);

};

#endif /* SCENEPARSER_H_ */
