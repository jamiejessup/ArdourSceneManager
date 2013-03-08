/*
 * SceneParser.h
 *
 *  Created on: 2013-03-08
 *      Author: jamie
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
