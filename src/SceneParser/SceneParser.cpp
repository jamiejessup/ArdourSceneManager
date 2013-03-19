/*
 * SceneParser.cpp
 *
 *  Created on: 2013-03-08
 *      Author: jamie
 */

#include "SceneParser.h"

SceneParser::SceneParser() {
	// nothing to initialize
}

SceneParser::~SceneParser() {
}

void SceneParser::loadSceneFromFile(Scene *destScene, string fileName) {
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	/*
	 * Re-initliaize master
	 */
	destScene->master.SetModified(false);
	/*
	 * Re initialize all tracks
	 */
	destScene->tracks.clear();

	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile(fileName.c_str(), NULL, 0);

	if (doc == NULL) {
		printf("error: could not parse file %s\n", fileName.c_str());
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	parseNodes(destScene, root_element);

	/*free the document */
	xmlFreeDoc(doc);

	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();

}

void SceneParser::parseNodes(Scene *destScene, xmlNode * a_node) {
	xmlNode *cur_node = NULL;
	unsigned char *name;
	unsigned char *trackNo;
	unsigned char *property;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			property = xmlGetProp(cur_node, BAD_CAST "gain");
			trackNo = xmlGetProp(cur_node, BAD_CAST "trackNo");
			if (strcmp((char*) cur_node->name, "name") == 0) {
				name = xmlGetProp(cur_node, BAD_CAST "name");
				destScene->setName((char*) name);
			}
			if (property != NULL) {
				if (strcmp((char*) cur_node->name, "master") == 0) {
					destScene->master.SetTrackGain(
							(char) atoi((char*) property));
				}
				if (trackNo != NULL) {
					destScene->tracks.push_back(
							Track((char) atoi((char*) property)));
				}
			}
		}

		parseNodes(destScene, cur_node->children);
	}
}

void SceneParser::saveSceneToFile(Scene *sourceScene, string fn) {
	//Check if we already have the file extension in our filename
	size_t found = fn.find(".scene");
	//If we don't then append it
	if (found == std::string::npos) {
		fn = fn + ".scene";
	}
	saveScene(sourceScene, fn);
}

void SceneParser::saveScene(Scene *sourceScene, string fn) {

	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL, node = NULL, groupNode = NULL;/* node pointers */

	char buff[256];
	int i;
	char gainStr[10];
	char trackNoStr[10];

	LIBXML_TEST_VERSION
	;

	/*
	 * Creates a new document, a node and set it as a root node
	 */

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Scene");
	xmlDocSetRootElement(doc, root_node);

	/*
	 * Write the name of the scene
	 */
	node = xmlNewChild(root_node, NULL, BAD_CAST "name", NULL);
	xmlNewProp(node, BAD_CAST "name", BAD_CAST sourceScene->getName().c_str());
	/*
	 * The master Bus
	 */
	node = xmlNewChild(root_node, NULL, BAD_CAST "master", NULL);
	// Its level
	sprintf(gainStr, "%d", (int) sourceScene->master.GetTrackGain());
	xmlNewProp(node, BAD_CAST "gain", BAD_CAST gainStr);
	/*
	 * The Tracks
	 */
	groupNode = xmlNewChild(root_node, NULL, BAD_CAST "tracks", NULL);
	// many Tracks
	for (i = 0; i < (int) sourceScene->tracks.size(); i++) {
		sprintf(buff, "track%d", i + 1);
		sprintf(trackNoStr, "%d", i + 1);
		sprintf(gainStr, "%d", (int) sourceScene->tracks[i].GetTrackGain());
		node = xmlNewChild(groupNode, NULL, BAD_CAST buff, NULL);
		xmlNewProp(node, BAD_CAST "gain", BAD_CAST gainStr);
		xmlNewProp(node, BAD_CAST "trackNo", BAD_CAST trackNoStr);
	}

	/*
	 * Dumping document to file
	 */
	xmlSaveFormatFileEnc(fn.c_str(), doc, "UTF-8", 1);

	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();
	/*
	 * this is to debug memory for regression tests
	 */
	xmlMemoryDump();

	sourceScene->master.SetModified(false);
	for (unsigned int i = 0; i < sourceScene->tracks.size(); i++) {
		sourceScene->tracks[i].SetModified(false);
	}
}
