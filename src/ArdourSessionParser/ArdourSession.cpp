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

#include "ArdourSession.h"

ArdourSessionParser::ArdourSessionParser() {

}

void ArdourSessionParser::init(string fn, Scene* pScene) {
    //clear all of the tracks in the current scene
    pScene->tracks.clear();

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */LIBXML_TEST_VERSION

            /*parse the file and get the DOM */
            doc = xmlReadFile(fn.c_str(), NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", fn.c_str());
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    parseNodes(root_element, pScene);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

}

ArdourSessionParser::~ArdourSessionParser() {
    // TODO Auto-generated destructor stub
}

void ArdourSessionParser::parseNodes(xmlNode * a_node, Scene *pScene) {
    xmlNode *cur_node = NULL;
    char gain;
    unsigned char *property;
    static int idCounter = 1;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            /*
             * Check if we are a Processor with parent as a Route
             * (otherwise we could be an aux send)
             * Aux sends will be implemented later
             * They are processors with processors as parents, the same otherwise
             */

            if (strcmp((char*) cur_node->name, "Processor") == 0
                    && strcmp((char*) cur_node->parent->name, "Route") == 0) {
                //	Check if we are in a processor of type "Amp" to get gain

                if (strcmp((char*) xmlGetProp(cur_node, BAD_CAST "name"), "Amp")
                        == 0) {
                    //Get the gain of the track from the Controllable attribute
                    property = xmlGetProp(xmlLastElementChild(cur_node),
                                          BAD_CAST "value");
                    if (property != NULL) {
                        //Convert gain to db, what we use internally
                        gain = absToCC(atof((char*) property));
                        //Are we the master track?
                        property = xmlGetProp(cur_node->parent,
                                              BAD_CAST "flags");
                        if (property == NULL) {
                            Track * track;
                            track = new Track((char)idCounter,gain);

                            //Find what sends are there for our track
                            getTrackSends(cur_node->parent->children,track,idCounter);

                            //Check if there is a mode property
                            //If it's not there we are a Bus
                            if(xmlHasProp(cur_node->parent, BAD_CAST "mode")) {
                                pScene->tracks.push_back(*track);
                            }
                            else {
                                pScene->busses.push_back(*track);
                            }
                            idCounter++;
                        } else if (strcmp((char*) property, "MasterOut") == 0) {
                            pScene->master.setGain(gain);
                        }
                        //check if the parent "Processor has a child "

                    }

                }
            }

            parseNodes(cur_node->children, pScene);
        }

    }
}

float ArdourSessionParser::absTodB(float gain) {
    if (gain == 0) {
        return -600;
    } else
        return 20 * log10(gain);
}

float ArdourSessionParser::dBToCC(float gain) {
    //Make up gain to make highest MIDI CC return +6db, and 99 return 0dB
    static float makeUpGain = 6 / log10(127 / (float) 99.0);
    return 99*pow(10,(gain/makeUpGain));
}

char ArdourSessionParser::absToCC(float gain) {
    return (char) dBToCC(absTodB(gain));

}

void ArdourSessionParser::getTrackSends(xmlNode* a_node, Track * track, int trackId) {
    xmlNode *routeNode = NULL;
    static int sendCounter = 1;
    unsigned char *property;
    char gain;


    for (routeNode = a_node; routeNode; routeNode = routeNode->next) {
        if (routeNode->type == XML_ELEMENT_NODE) {
            if (strcmp((char*) routeNode->name, "Processor") == 0
                    && strcmp((char*) routeNode->parent->name, "Processor") == 0) {
                if (strcmp((char*) xmlGetProp(routeNode, BAD_CAST "name"), "Amp")
                        == 0) {
                    //Get the send gain
                    //Get the gain of the track from the Controllable attribute
                    property = xmlGetProp(xmlLastElementChild(routeNode),
                                          BAD_CAST "value");
                    if (property != NULL) {
                        //Convert gain to db, what we use internally
                        gain = absToCC(atof((char*) property));
                        track->sends.push_back(Send((char)trackId,(char)sendCounter,gain));
                    }
                }
            }
            getTrackSends(routeNode->children,track,trackId);
        }
    }

    sendCounter = 1;
}
