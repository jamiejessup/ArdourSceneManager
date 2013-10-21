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
#ifndef GTKMM_EXAMPLE_RADIOBUTTONS_H
#define GTKMM_EXAMPLE_RADIOBUTTONS_H

#define CC_MASK 0b10110000
#define CC_NIBBLE 0b1011
#define NOTE_ON_NIBBLE 0b1001
#define NOTE_OFF_NIBBLE 0b100

//Two defines because sometimes Ardour outputs on one less than the midi map
#define MASTER_CC 119
#define MASTER_CC2 118

#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/separator.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/stock.h>
#include <glibmm/main.h>
#include "Scene/Scene.h"
#include "SceneParser/SceneParser.h"
#include "ArdourSessionParser/ArdourSession.h"
#include "JackMIDI/jackMIDI.h"
#include "OSCServer.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <dirent.h>
#include "ControllerEvent.h"

class ASMView: public Gtk::Window {
	void loadNewSceneFile();

	//Signal handlers:
	void on_new_button_clicked();
	void on_load_button_clicked();
	void on_close_button_clicked();
	void on_save_button_clicked();
	void on_save_to_fol_button_clicked();
	void on_scene_file_activated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn*);
	void on_scene_file_selection_changed();
	void on_remove_button_clicked();

	//Jack MIDI Handler
	Jack jack;

    //An OSC server
    OSCServer oscServer;

    ControllerEvent controllerEvent;

    //A ring buffer ptr for the jack client to send us messages
    jack_ringbuffer_t *sceneUpdateBuffer;
    jack_ringbuffer_t *ardourOSCBuffer;

    //Idle function to poll for messages
    bool idleFunction();

    void sceneUpdateHandler(MidiEvent& midiEvent);

	//Scene stuff
	string sceneFileName;
	Scene myScene;

	//A scene parser
	SceneParser sceneParser;

	//A scene directory
	string scenesDir;
	//A List of scene files
	vector<string> sceneFiles;

	//Ardour Session Handler
	ArdourSessionParser sessionHandler;

	//Child widgets:


	//Boxes and organization stuff:
	Gtk::Box topLevelBox, topBox, middleBox,bottomBox, frameBox;
	Gtk::Separator seperator;
	Gtk::Separator seperator2;
	Gtk::Grid detailGrid;

	//Buttons:
	Gtk::Button loadButton;
	Gtk::Button saveButton;
	Gtk::Button closeButton;
	Gtk::Button newButton;
	Gtk::Button removeButton;

	//Information Text
	Gtk::Label nameInfoLabel;
	Gtk::Entry nameEntry;
	Gtk::Label numTracksInfoLabel;
    Gtk::Label numTracksLabel;
    Gtk::Label updatedTracksInfoLabel;
    Gtk::Label updatedTracksLabel;
    Gtk::Label numBussesInfoLabel;
    Gtk::Label numBussesLabel;
    Gtk::Label updatedBussesInfoLabel;
    Gtk::Label updatedBussesLabel;
	Gtk::Frame sceneFrame;

	//Tree model columns:
	class ModelColumns: public Gtk::TreeModel::ColumnRecord {
	public:

		ModelColumns() {
			add(fileNameColumn), add(fileNamePresentationColumn);
		}

		Gtk::TreeModelColumn<string> fileNamePresentationColumn;
		Gtk::TreeModelColumn<string> fileNameColumn;
	};

	//Tree view stuff to display the scene files that are in the folder
	ModelColumns scenesList;

	Gtk::ScrolledWindow scrolledWindow;
	Gtk::TreeView treeView;
	Glib::RefPtr<Gtk::ListStore> refTreeModel;

	//Tree view filler
	void scanAvailableSceneFiles();

public:
	ASMView();
	virtual ~ASMView();

	//Shows scene details
	void showSceneDetails();
	Scene *getScene();
};

#endif //GTKMM_EXAMPLE_RADIOBUTTONS_H
