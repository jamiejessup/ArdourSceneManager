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

#include <iostream>
#include "stdio.h"
#include "ASMView.h"
#include <gtkmm/application.h>
#include <pthread.h>

using namespace std;

/* Mutex for MIDI data to be sent to Ardour */
pthread_mutex_t txMutex;

/* Mutex for MIDI data being rx'd from Ardour */
pthread_mutex_t rxMutex;


/* time stuff to make sure I get the timing right */
clock_t begin, end;
double time_spent;

int main(int argc, char *argv[]) {
	// Init mutexes
	pthread_mutex_init(&txMutex, NULL);
	pthread_mutex_init(&rxMutex, NULL);

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,
			"org.gtkmm.example");

	ASMView mainWindow;

	//Shows the window and returns when it is closed.
	return app->run(mainWindow);
}
