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
#include <gtkmm/stock.h>
#include "Scene/Scene.h"
#include "SceneParser/SceneParser.h"
#include "ArdourSessionParser/ArdourSession.h"
#include "JackMIDI/jackMIDI.h"
#include <iostream>
#include <stdlib.h>

class MainWindow : public Gtk::Window
{
	 //Signal handlers:
	  void on_new_button_clicked();
	  void on_load_button_clicked();
	  void on_save_as_button_clicked();
	  void on_close_button_clicked();
	  void on_save_button_clicked();

	  //Jack MIDI Handler
	  Jack jack;

	  //Scene stuff
	  string sceneFileName;
	  Scene myScene;

	  //A scene parser
	  SceneParser sceneParser;


	  //Ardour Session Handler
	  ArdourSession sessionHandler;

	  //Child widgets:

	  //Boxes and organization stuff:
	  Gtk::Box topLevelBox, topBox, bottomBox, frameBox;
	  Gtk::Separator seperator;
	  Gtk::Grid detailGrid;

	  //Buttons:
	  Gtk::Button loadButton;
	  Gtk::Button saveButton;
	  Gtk::Button saveAsButton;
	  Gtk::Button closeButton;
	  Gtk::Button newButton;
	  Gtk::Button detailedViewButton;


	  //Information Text
	  Gtk::Label nameInfoLabel;
	  Gtk::Entry nameEntry;
	  Gtk::Label numTracksInfoLabel;
	  Gtk::Label numTracksLabel;
	  Gtk::Label updatedTotalInfoLabel;
	  Gtk::Label updatedTotalLabel;
	  Gtk::Frame sceneFrame;
public:
  MainWindow();
  virtual ~MainWindow();

  //Shows scene details
  void showSceneDetails();
};

#endif //GTKMM_EXAMPLE_RADIOBUTTONS_H


