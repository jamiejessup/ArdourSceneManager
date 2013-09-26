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

#ifndef ARDOURSESSION_H_
#define ARDOURSESSION_H_

#include "../Scene/Scene.h"
#include <iostream>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cmath>

using namespace std;

class ArdourSessionParser {
	void parseNodes(xmlNode *, Scene*);
	float absTodB(float);
	float dBToCC(float gain);
	char absToCC(float gain);
    void getTrackSends(xmlNode *, Track *, int trackId);
    void getMasterSends(xmlNode *, Master *);
public:
	ArdourSessionParser();
	/**
	 * Initialize a scene using an Ardour session file.
	 *
	 * Registers all tracks and busses from the session to the scene to save and load their states later
	 */
	void init(string, Scene*);
	virtual ~ArdourSessionParser();
};

#endif /* ARDOURSESSION_H_ */
