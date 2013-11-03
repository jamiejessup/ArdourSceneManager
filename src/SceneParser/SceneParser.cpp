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
    destScene->master.setModified(false);
    /*
     * Re initialize all tracks and busses
     */
    destScene->tracks.clear();
    destScene->busses.clear();

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
    unsigned char *trackGain;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            /*
            For each of the savable items read their value
            */

            //Read the name of the scene
            if (strcmp((char*) cur_node->name, "name") == 0) {
                name = xmlGetProp(cur_node, BAD_CAST "name");
                destScene->setName((char*) name);
            }

            //Read the master track
            if (strcmp((char*) cur_node->name, "gain") == 0) {
                //Check that the current node's parent is "master"
                if (strcmp((char*) cur_node->parent->name, "master") == 0) {
                    trackGain = xmlGetProp(cur_node, BAD_CAST "value");
                    destScene->master.setGain(
                                (char) atoi((char*) trackGain));
                }
            }

            //Read all other track's gain
            if (strcmp((char*) cur_node->name, "trackNo") == 0) {
                //Check that the current node's parent is "track"
                if (strcmp((char*) cur_node->parent->name, "track") == 0) {
                    //find the track number
                    trackNo = xmlGetProp(cur_node, BAD_CAST "value");
                    if (strcmp((char*) cur_node->next->next->name, "gain") == 0) {
                        trackGain = xmlGetProp(cur_node->next->next, BAD_CAST "value");
                    }
                    else
                        trackGain = (unsigned char*) "0.0";

                    Track * track;
                    track = new Track((char) atoi((char*) trackNo),(char) atoi((char*) trackGain));

                    getTrackSends(cur_node->parent,track,atoi((char*) trackNo));

                    //Parent's parent is tracks for tracks and busses for busses
                    if(strcmp((char*) cur_node->parent->parent->name, "tracks") == 0) {
                        destScene->tracks.push_back(*track);
                    }
                    else if(strcmp((char*) cur_node->parent->parent->name, "busses") == 0) {
                        destScene->busses.push_back(*track);
                    }
                }

            }
            parseNodes(destScene, cur_node->children);
        }
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
    xmlNodePtr root_node = NULL;/* node pointers */
    int i;
    char gainStr[10];
    char trackNoStr[10];
    char sendGainStr[10];
    char sendNoStr[10];

    LIBXML_TEST_VERSION;

    /*(char) atoi((char*) trackGain)
     * Creates a new document, a node and set it as a root node
     */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "Scene");
    xmlDocSetRootElement(doc, root_node);

    /*
     * Write the name of the scene
     */
    xmlNodePtr nameNode = xmlNewChild(root_node, NULL, BAD_CAST "name", NULL);
    xmlNewProp(nameNode, BAD_CAST "name", BAD_CAST sourceScene->getName().c_str());
    /*
     * The master Bus
     */
    xmlNodePtr masterNode = xmlNewChild(root_node, NULL, BAD_CAST "master", NULL);
    // Its level
    xmlNodePtr gainNode = xmlNewChild(masterNode, NULL, BAD_CAST "gain", NULL);
    sprintf(gainStr, "%d", (int) sourceScene->master.getGain());
    xmlNewProp(gainNode, BAD_CAST "value", BAD_CAST gainStr);
    /*
     * The Tracks
     */
    xmlNodePtr tracksGroupNode = xmlNewChild(root_node, NULL, BAD_CAST "tracks", NULL);
    // many Tracks
    for (i = 0; i < (int) sourceScene->tracks.size(); i++) {
        xmlNodePtr trackNode = xmlNewChild(tracksGroupNode, NULL, BAD_CAST "track", NULL);
        xmlNodePtr trackNoNode = xmlNewChild(trackNode, NULL, BAD_CAST "trackNo", NULL);
        sprintf(trackNoStr, "%d", (int) sourceScene->tracks[i].getId());
        xmlNewProp(trackNoNode, BAD_CAST "value", BAD_CAST trackNoStr);
        xmlNodePtr gainNode = xmlNewChild(trackNode, NULL, BAD_CAST "gain", NULL);
        sprintf(gainStr, "%d", (int) sourceScene->tracks[i].getGain());
        xmlNewProp(gainNode, BAD_CAST "value", BAD_CAST gainStr);

        if(sourceScene->tracks[i].sends.size() > 0) {
            xmlNodePtr sendsGroupNode = xmlNewChild(trackNode, NULL, BAD_CAST "sends", NULL);
            //The tracks sends
            for(int j = 0; j < (int) sourceScene->tracks[i].sends.size(); j++) {
                xmlNodePtr sendNode = xmlNewChild(sendsGroupNode, NULL, BAD_CAST "send", NULL);
                xmlNodePtr sendNoNode = xmlNewChild(sendNode, NULL, BAD_CAST "sendNo", NULL);
                sprintf(sendNoStr, "%d", (int) sourceScene->tracks[i].sends[j].getId());
                xmlNewProp(sendNoNode, BAD_CAST "value", BAD_CAST sendNoStr);
                xmlNodePtr sendGainNode = xmlNewChild(sendNode, NULL, BAD_CAST "gain", NULL);
                sprintf(sendGainStr, "%d", (int) sourceScene->tracks[i].sends[j].getGain());
                xmlNewProp(sendGainNode, BAD_CAST "value", BAD_CAST sendGainStr);
            }
        }

    }

    /*
     * The Busses
     */
    xmlNodePtr bussesGroupNode = xmlNewChild(root_node, NULL, BAD_CAST "busses", NULL);
    // many Busses
    for (i = 0; i < (int) sourceScene->busses.size(); i++) {
        xmlNodePtr trackNode = xmlNewChild(bussesGroupNode, NULL, BAD_CAST "track", NULL);
        xmlNodePtr trackNoNode = xmlNewChild(trackNode, NULL, BAD_CAST "trackNo", NULL);
        sprintf(trackNoStr, "%d", (int) sourceScene->busses[i].getId());
        xmlNewProp(trackNoNode, BAD_CAST "value", BAD_CAST trackNoStr);
        xmlNodePtr gainNode = xmlNewChild(trackNode, NULL, BAD_CAST "gain", NULL);
        sprintf(gainStr, "%d", (int) sourceScene->busses[i].getGain());
        xmlNewProp(gainNode, BAD_CAST "value", BAD_CAST gainStr);
    }


    /*
     * Dumping document to file
     */
    xmlSaveFormatFileEnc(fn.c_str(), doc, "UTF-8", 1);

    /*(char) atoi((char*) trackGain)
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();

    sourceScene->master.setModified(false);
    for (unsigned int i = 0; i < sourceScene->tracks.size(); i++) {
        sourceScene->tracks[i].setModified(false);
    }
}

void SceneParser::getTrackSends(xmlNode* a_node, Track * track, int trackId) {
    xmlNode *cur_node = NULL;
    unsigned char *sendNo;
    unsigned char *gain;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(strcmp((char*) cur_node->name, "sendNo") == 0
                    && strcmp((char*) cur_node->parent->name, "send") == 0) {
                //find the track number
                sendNo = xmlGetProp(cur_node, BAD_CAST "value");
                if (strcmp((char*) cur_node->next->next->name, "gain") == 0) {
                    gain = xmlGetProp(cur_node->next->next, BAD_CAST "value");
                }
                else
                    gain = (unsigned char*) "0.0";
                Send *send;
                send = new Send((char)trackId,(char) atoi((char*) sendNo), (char) atoi((char*) gain));
                track->sends.push_back(*send);
            }
            getTrackSends(cur_node->children,track,trackId);
        }
    }
}

