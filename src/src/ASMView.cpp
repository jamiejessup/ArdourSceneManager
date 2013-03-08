/*
 * ASMView.cpp
 *
 *  Created on: 2013-02-12
 *      Author: jamie
 */

#include "ASMView.h"
#include "JackMIDI/jackMIDI.h"

MainWindow::MainWindow() :
		jack(this), myScene(&jack), topLevelBox(Gtk::ORIENTATION_VERTICAL), topBox(
				Gtk::ORIENTATION_HORIZONTAL, 10), bottomBox(
				Gtk::ORIENTATION_HORIZONTAL, 10), frameBox(
				Gtk::ORIENTATION_VERTICAL, 10), loadButton("Load Scene"), saveButton(
				"Save Scene"), saveAsButton("Save Scene As .."), closeButton(
				"Close"), newButton("New Scene From Ardour Session"), detailedViewButton(
				"Detailed View"), nameInfoLabel("Name:"), numTracksInfoLabel(
				"Number of Tracks:"), numTracksLabel("0"), updatedTotalInfoLabel(
				"Tracks Updated: "), updatedTotalLabel("0"), sceneFrame(
				"Current Scene Details") {

	jack.activate();
	saveButton.set_sensitive(false);
	saveAsButton.set_sensitive(false);

	// Set title and border of the window
	set_title("Ardour Scene Manager");
	set_border_width(0);

	// Add outer box to the window (because the window
	// can only contain a single widget)
	add(topLevelBox);

	//Put the inner boxes and the separator in the outer box:
	topLevelBox.pack_start(topBox);
	topLevelBox.pack_start(seperator);
	topLevelBox.pack_start(bottomBox);

	// Set the inner boxes' borders
	bottomBox.set_border_width(10);
	topBox.set_border_width(10);

	//Add info to the detail grid
	detailGrid.add(nameInfoLabel);
	nameEntry.set_hexpand(true);
	detailGrid.add(nameEntry);
	detailGrid.attach_next_to(numTracksInfoLabel, nameInfoLabel,
			Gtk::POS_BOTTOM, 1, 1);
	detailGrid.attach_next_to(numTracksLabel, nameEntry, Gtk::POS_BOTTOM, 1, 1);
	detailGrid.attach_next_to(updatedTotalInfoLabel, numTracksInfoLabel,
			Gtk::POS_BOTTOM, 1, 1);
	detailGrid.attach_next_to(updatedTotalLabel, numTracksLabel,
			Gtk::POS_BOTTOM, 1, 1);
	/* We will include a detailed view button in the next version
	 detailGrid.attach_next_to(detailedViewButton, updatedTotalLabel,
	 Gtk::POS_BOTTOM, 1, 1);
	 */
	detailGrid.set_column_spacing(30);
	detailGrid.set_row_spacing(5);
	detailGrid.set_sensitive(false);

	//Put the label in the scene
	sceneFrame.add(detailGrid);
	// Put the scene frame in box 1
	topBox.pack_start(sceneFrame);

	// Put buttons in Box2:
	bottomBox.pack_start(newButton);
	bottomBox.pack_start(loadButton);
	bottomBox.pack_start(saveButton);
	bottomBox.pack_start(saveAsButton);
	bottomBox.pack_start(closeButton);

	// Make the button the default widget
	closeButton.set_can_default();
	closeButton.grab_default();

	// Connect the clicked signals of buttons
	closeButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_close_button_clicked));
	loadButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_load_button_clicked));
	saveButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_save_button_clicked));
	saveAsButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_save_as_button_clicked));
	newButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_new_button_clicked));

	// Show all children of the window
	show_all_children();

}

MainWindow::~MainWindow() {
}

void MainWindow::on_close_button_clicked() {
	hide(); //to close the application.
}

void MainWindow::on_new_button_clicked() {
	//Start a file chooser dialog
	Gtk::FileChooserDialog dialog("Choose Ardour Session",
			Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:

	//.scene files
	Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Ardour Sessions");
	filter_text->add_pattern("*.ardour");
	dialog.add_filter(filter_text);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch (result) {
	case (Gtk::RESPONSE_OK): {
		/*
		 * Parse the file
		 */
		sessionHandler.init(dialog.get_filename(), &myScene);

		//Enable save and save as button
		saveAsButton.set_sensitive(true);
		saveButton.set_sensitive(false);

		/*
		 * Start listening for Ardour MIDI signals
		 */
		jack.setScene(&myScene);

		/*
		 * Update the GUI layout
		 */
		detailGrid.set_sensitive(true);
		showSceneDetails();
		break;
	}
	case (Gtk::RESPONSE_CANCEL): {
		//Well nothing
		//std::cout << "Cancel clicked." << std::endl;
		break;
	}
	default: {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
	}

}

void MainWindow::on_load_button_clicked() {
	//Start a file chooser dialog
	Gtk::FileChooserDialog dialog("Choose scene file",
			Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:

	//.scene files
	Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Scene files");
	filter_text->add_pattern("*.scene");
	dialog.add_filter(filter_text);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch (result) {
	case (Gtk::RESPONSE_OK): {
		//std::cout << "Open clicked." << std::endl;

		//Notice that this is a std::string, not a Glib::ustring.
		sceneFileName = dialog.get_filename();
		//Enable the save buttons
		saveButton.set_sensitive(true);
		saveAsButton.set_sensitive(true);

		/*
		 * Parse the file
		 */
		sceneParser.loadSceneFromFile(&myScene,sceneFileName);

		/*
		 * And then send it to Ardour
		 */
		myScene.sendSceneToArdour();

		/*
		 * Update Scene Details in the GUI
		 */
		nameEntry.set_text(myScene.getName());

		/*
		 * Start listening for Ardour MIDI signals if we haven't already
		 */
		jack.setScene(&myScene);
		showSceneDetails();
		break;
	}
	case (Gtk::RESPONSE_CANCEL): {
		//Well nothing
		//std::cout << "Cancel clicked." << std::endl;
		break;
	}
	default: {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
	}
}

void MainWindow::on_save_button_clicked() {
	//Get the scene name
	myScene.setName(nameEntry.get_text());
	/**
	 * Save the scene to the file
	 */
	sceneParser.saveSceneToFile(&myScene,sceneFileName);
	showSceneDetails();
}

void MainWindow::on_save_as_button_clicked() {
	//Open File chooser dialog
	//Start a file chooser dialog
	Gtk::FileChooserDialog dialog("Save scene file",
			Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:

	//.scene files
	Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Scene files");
	filter_text->add_pattern("*.scene");
	dialog.add_filter(filter_text);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	//Handle the response:
	switch (result) {
	case (Gtk::RESPONSE_OK): {
		//std::cout << "Open clicked." << std::endl;

		//Notice that this is a std::string, not a Glib::ustring.
		sceneFileName = dialog.get_filename();
		//Write the contents of the scene to file
		//Get the scene name
		myScene.setName(nameEntry.get_text());
		sceneParser.saveSceneToFile(&myScene,sceneFileName);
		//Enable the save button
		saveButton.set_sensitive(true);
		showSceneDetails();
		break;
	}
	case (Gtk::RESPONSE_CANCEL): {
		//Well nothing
		//std::cout << "Cancel clicked." << std::endl;
		break;
	}
	default: {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}
	}

}

void MainWindow::showSceneDetails() {
	char numTracks[5];
	char updatedTracks[5];
	sprintf(numTracks, "%d",
			(int) myScene.tracks.size()
					+ 1 /* Add 1 since there is ALWAYS A MASTER*/);
	numTracksLabel.set_text(numTracks);
	//Get the number of updated tracks
	sprintf(updatedTracks, "%d", myScene.getUpdatedTracks());
	updatedTotalLabel.set_text(updatedTracks);

}

