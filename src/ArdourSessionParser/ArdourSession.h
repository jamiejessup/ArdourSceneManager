/*
 * ArdourSession.h
 *
 *  Created on: 2013-02-17
 *      Author: jamie
 */

#ifndef ARDOURSESSION_H_
#define ARDOURSESSION_H_

#include "../Scene/Scene.h"
#include <iostream>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cmath>

using namespace std;

class ArdourSession {
	void parseNodes(xmlNode *, Scene*);
	float absTodB(float);
	float dBToCC(float gain);
	char absToCC(float gain);
public:
	ArdourSession();
	/**
	 * Initialize a scene using an Ardour session file.
	 *
	 * Registers all tracks and busses from the session to the scene to save and load their states later
	 */
	void init(string, Scene*);
	virtual ~ArdourSession();
};

#endif /* ARDOURSESSION_H_ */
