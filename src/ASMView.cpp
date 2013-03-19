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
				Gtk::ORIENTATION_HORIZONTAL, 10), middleBox(
				Gtk::ORIENTATION_HORIZONTAL, 10), bottomBox(
				Gtk::ORIENTATION_HORIZONTAL, 10), frameBox(
				Gtk::ORIENTATION_VERTICAL, 10), loadButton("Load Scene"), saveToFolButton(
				"Save To Scene Folder"), saveCurButton("Save Current Scene"), saveAsButton(
				"Save Scene As .."), closeButton("Close"), newButton(
				"New Scene From Ardour Session"), detailedViewButton(
				"Detailed View"), nameInfoLabel("Name:"), numTracksInfoLabel(
				"Number of Tracks:"), numTracksLabel("0"), updatedTotalInfoLabel(
				"Tracks Updated: "), updatedTotalLabel("0"), sceneFrame(
				"Current Scene Details") {

	jack.activate();
	saveToFolButton.set_sensitive(false);
	saveCurButton.set_sensitive(false);
	saveAsButton.set_sensitive(false);
	loadButton.set_sensitive(false);

	// Set title and border of the window
	set_title("Ardour Scene Manager");
	set_border_width(0);

	// Add outer box to the window (because the window
	// can only contain a single widget)
	add(topLevelBox);

	//Add the TreeView, inside a ScrolledWindow
	m_ScrolledWindow.add(m_TreeView);
	m_ScrolledWindow.set_size_request(10, 150);
	m_ScrolledWindow.set_sensitive(false);

	//Put the inner boxes and the separator in the outer box:
	topLevelBox.pack_start(topBox);
	topLevelBox.pack_start(seperator);
	topLevelBox.pack_start(middleBox);
	topLevelBox.pack_start(seperator2);
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

	/*
	 * Add the tree view to the window with all scene files in the session's folder
	 */
	//Only show the scrollbars when they are necessary:
	m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	//Add the scrolled window
	middleBox.pack_start(m_ScrolledWindow);
	//Add few black rows for now
	//Add the TreeView's view columns:
	m_TreeView.append_column("Scene File Name",
			m_Columns.fileNamePresentationColumn);

	// Put buttons in the bottom Box:
	bottomBox.pack_start(newButton);
	bottomBox.pack_start(loadButton);
	bottomBox.pack_start(saveCurButton);
	bottomBox.pack_start(saveToFolButton);
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
	saveCurButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_save_button_clicked));
	saveAsButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_save_as_button_clicked));
	newButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_new_button_clicked));
	saveToFolButton.signal_clicked().connect(
			sigc::mem_fun(*this, &MainWindow::on_save_to_fol_button_clicked));

	//Connect the signals of rows in the tree view for double click and enter
	m_TreeView.signal_row_activated().connect(
			sigc::mem_fun(*this, &MainWindow::on_scene_file_activated));

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

		/**
		 * If a directory to put scenes isn't already there, create it
		 */

		scenesDir = dialog.get_filename();
		//get the session directory (remove session file)
		size_t found = scenesDir.find_last_of("/");
		//Remove the session file name
		for (unsigned int i = found + 1; i < scenesDir.size(); i++) {
			scenesDir.erase(i);
		}
		//Add "Scenes to the path"
		scenesDir.append("Scenes/");
		//Make the directory if not already there
		struct stat st = { 0 };

		if (stat(scenesDir.c_str(), &st) == -1) {
			mkdir(scenesDir.c_str(), 0700);
		}

		/*
		 * Parse the file
		 */
		sessionHandler.init(dialog.get_filename(), &myScene);

		//Enable save and save as buttons
		saveToFolButton.set_sensitive(true);
		saveAsButton.set_sensitive(true);
		saveCurButton.set_sensitive(false);
		loadButton.set_sensitive(true);

		/*
		 * Start listening for Ardour MIDI signals
		 */
		jack.setScene(&myScene);

		/*
		 * Update the GUI layout
		 */

		detailGrid.set_sensitive(true);
		showSceneDetails();
		m_ScrolledWindow.set_sensitive(true);
		scanAvailableSceneFiles();
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
	loadNewSceneFile();
}

void MainWindow::on_save_button_clicked() {
	//Get the scene name
	myScene.setName(nameEntry.get_text());
	/**
	 * Save the scene to the file
	 */
	sceneParser.saveSceneToFile(&myScene, sceneFileName);
	showSceneDetails();
	scanAvailableSceneFiles();
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
		sceneParser.saveSceneToFile(&myScene, sceneFileName);
		//Enable the save button
		saveCurButton.set_sensitive(true);
		showSceneDetails();
		scanAvailableSceneFiles();
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

void MainWindow::on_save_to_fol_button_clicked() {
	//Get the scene name
	myScene.setName(nameEntry.get_text());
	/*
	 * Run a dialog to get a file name to save the file with
	 */
	//Add response buttons the the dialog:
	fileNameDialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fileNameDialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	bool ok = false;
	string fileName = scenesDir;
	do {
		int result = fileNameDialog.run();

		fileNameDialog.hide();

		//Handle the response:
		switch (result) {
		case (Gtk::RESPONSE_OK): {
			string temp = fileNameDialog.getFileName();
			if (temp.size() != 0) {
				fileName.append(temp);
				ok = true;
			} else {
				fileNameDialog.label.set_text("Be sure to specify a file name");
			}
			break;
		}
		case (Gtk::RESPONSE_CANCEL): {
			ok = true;
			break;
		}
		default: {
			ok = true;
			break;
		}
		}
	} while (!ok);

	/**
	 * Save the scene to the scene folder
	 */
	sceneParser.saveSceneToFile(&myScene, fileName);

	//Make the current scene the one we just saved
	sceneFileName = fileName;
	showSceneDetails();
	scanAvailableSceneFiles();
	saveCurButton.set_sensitive(true);
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
	nameEntry.set_text(myScene.getName());

}

void MainWindow::scanAvailableSceneFiles() {
	//Create the Tree model:
	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_TreeView.set_model(m_refTreeModel);
	sceneFiles.clear();

	DIR *d;
	struct dirent *dir;
	d = opendir(scenesDir.c_str());
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			//Add the scene files to the directory
			string currentFile = dir->d_name;
			//Is the file a scene file?
			size_t found = currentFile.find(".scene");
			if (found != std::string::npos) {
				//Add the files to the scene file vector
				sceneFiles.push_back(currentFile);
			}
		}

		closedir(d);
	}

	//m_refTreeModel->clear();
	for (unsigned int i = 0; i < sceneFiles.size(); i++) {
		size_t found = sceneFiles[i].find(".scene");
		string withoutExt = sceneFiles[i].substr(0, found);
		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
		row[m_Columns.fileNamePresentationColumn] = withoutExt;
		row[m_Columns.fileNameColumn] = sceneFiles[i];
	}
}

void MainWindow::on_scene_file_activated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn*) {
	loadNewSceneFile();
}

void MainWindow::loadNewSceneFile() {
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
			m_TreeView.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		//Get the file name to load from the row
		string newSceneFile = scenesDir;
		newSceneFile += row.get_value(m_Columns.fileNameColumn);
		sceneFileName = newSceneFile;

		/*
		 * Parse the file
		 */
		sceneParser.loadSceneFromFile(&myScene, sceneFileName);

		/*
		 * And then send it to Ardour
		 */
		myScene.sendSceneToArdour();

		/*
		 * Start listening for Ardour MIDI signals if we haven't already
		 */
		jack.setScene(&myScene);
		showSceneDetails();
	}
}
