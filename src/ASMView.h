/*
 * ASMView.h
 *
 *  Created on: 2013-02-12
 *      Author: jamie
 */

#ifndef GTKMM_EXAMPLE_RADIOBUTTONS_H
#define GTKMM_EXAMPLE_RADIOBUTTONS_H

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
#include "Scene/Scene.h"
#include "SceneParser/SceneParser.h"
#include "ArdourSessionParser/ArdourSession.h"
#include "JackMIDI/jackMIDI.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <dirent.h>

class MainWindow: public Gtk::Window {
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
	ArdourSession sessionHandler;

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
	Gtk::Label updatedTotalInfoLabel;
	Gtk::Label updatedTotalLabel;
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
	ModelColumns m_Columns;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

	//Tree view filler
	void scanAvailableSceneFiles();

public:
	MainWindow();
	virtual ~MainWindow();

	//Shows scene details
	void showSceneDetails();
};

#endif //GTKMM_EXAMPLE_RADIOBUTTONS_H
