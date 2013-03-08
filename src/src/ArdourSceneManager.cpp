//============================================================================
// Name        : ArdourSceneManager.cpp
// Author      : Jamie Jessup, jessup.jamie@gmail.com
// Version     : 0.1
// Copyright   : GPL
// Description : This is an application that manages scenes for use
//				 with the Ardour to save and recall settings at the push of
//				 a button.
//============================================================================

#include <iostream>
#include "stdio.h"
#include "lo/lo.h"
#include "ASMView.h"
#include <gtkmm/application.h>
#include <pthread.h>

using namespace std;

/* Mutex for MIDI data to be sent to Ardour */
pthread_mutex_t txMutex;

/* Mutex for MIDI data being rx'd from Ardour */
pthread_mutex_t rxMutex;

int main(int argc, char *argv[]) {
	// Init mutexes
	pthread_mutex_init(&txMutex, NULL);
	pthread_mutex_init(&rxMutex, NULL);

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,
			"org.gtkmm.example");

	MainWindow mainWindow;

	//Shows the window and returns when it is closed.
	return app->run(mainWindow);
}
