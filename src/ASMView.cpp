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


#include "ASMView.h"
#include "JackMIDI/jackMIDI.h"
#include "OSCServer.h"

ASMView::ASMView() :
    jack(this), oscServer(jack.controllerBuffer),
    myScene(&jack), topLevelBox(Gtk::ORIENTATION_VERTICAL),
    topBox(Gtk::ORIENTATION_HORIZONTAL, 10), middleBox(Gtk::ORIENTATION_HORIZONTAL, 10),
    bottomBox(Gtk::ORIENTATION_HORIZONTAL, 10),
    frameBox(Gtk::ORIENTATION_VERTICAL, 10), loadButton("Load Scene"),
    saveButton("Save Scene"), closeButton("Close"),
    newButton("New Scene From Ardour Session"), removeButton("Remove Scene"),
    nameInfoLabel("Name:"), numTracksInfoLabel("Number of Tracks:"),
    numTracksLabel("0"), updatedTracksInfoLabel("Tracks Updated: "),
    updatedTracksLabel("0"), numBussesInfoLabel("Number of Busses"),
    numBussesLabel("0"),updatedBussesInfoLabel("Busses Updated"),
    updatedBussesLabel("0"),sceneFrame("Current Scene Details") {

    // create an instance of a ringbuffer that will hold up to 20 integers,
    // let the pointer point to it
    sceneUpdateBuffer = jack_ringbuffer_create( 200 * sizeof(MidiEvent));

    // lock the buffer into memory, this is *NOT* realtime safe, do it before
    // using the buffer!
    int res = jack_ringbuffer_mlock(sceneUpdateBuffer);

    // check if we've locked the memory successfully
    if ( res ) {
        std::cout << "Error locking memory!" << std::endl;
    }

    // create an instance of a ringbuffer that will hold up to 20 integers,
    // let the pointer point to it
    controllerUpdate = jack_ringbuffer_create( 200 * sizeof(ControllerUpdateEvent));

    // lock the buffer into memory, this is *NOT* realtime safe, do it before
    // using the buffer!
    res = jack_ringbuffer_mlock(controllerUpdate);

    // check if we've locked the memory successfully
    if ( res ) {
        std::cout << "Error locking memory!" << std::endl;
    }

    jack.sceneUpdateBuffer = sceneUpdateBuffer;
    oscServer.controllerUpdate = controllerUpdate;
    jack.activate();

    saveButton.set_sensitive(false);
    loadButton.set_sensitive(false);
    removeButton.set_sensitive(false);

    Glib::signal_idle().connect( sigc::mem_fun(*this, &ASMView::idleFunction) );

    // Set title and border of the window
    set_title("Ardour Scene Manager");
    set_border_width(0);

    // Add outer box to the window (because the window
    // can only contain a single widget)
    add(topLevelBox);

    //Add the TreeView, inside a ScrolledWindow
    scrolledWindow.add(treeView);
    scrolledWindow.set_size_request(10, 150);
    scrolledWindow.set_sensitive(false);


    //Make the middle box the one that expands on resize
    topBox.set_vexpand_set(false);
    middleBox.set_vexpand_set(true);
    bottomBox.set_vexpand_set(false);
    seperator.set_vexpand_set(false);
    seperator2.set_vexpand_set(false);

    //Put the inner boxes and the separator in the outer box:
    topLevelBox.pack_start(topBox, false, true, false);
    topLevelBox.pack_start(seperator, false, true, false);
    topLevelBox.pack_start(middleBox, true, true, false);
    topLevelBox.pack_start(seperator2, false, true, false);
    topLevelBox.pack_start(bottomBox, false, true, false);

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
    detailGrid.attach_next_to(updatedTracksInfoLabel, numTracksInfoLabel,
                              Gtk::POS_BOTTOM, 1, 1);
    detailGrid.attach_next_to(updatedTracksLabel, numTracksLabel,
                              Gtk::POS_BOTTOM, 1, 1);
    detailGrid.attach_next_to(numBussesInfoLabel, updatedTracksInfoLabel,
                              Gtk::POS_BOTTOM, 1, 1);
    detailGrid.attach_next_to(numBussesLabel, updatedTracksLabel,
                              Gtk::POS_BOTTOM, 1, 1);
    detailGrid.attach_next_to(updatedBussesInfoLabel, numBussesInfoLabel,
                              Gtk::POS_BOTTOM, 1, 1);
    detailGrid.attach_next_to(updatedBussesLabel, numBussesLabel,
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
    scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //Add the scrolled window
    middleBox.pack_start(scrolledWindow);
    //Add few black rows for now
    //Add the TreeView's view columns:
    treeView.append_column("Scene File Name",
                           scenesList.fileNamePresentationColumn);

    // Put buttons in the bottom Box:
    bottomBox.pack_start(newButton);
    bottomBox.pack_start(loadButton);
    bottomBox.pack_start(saveButton);
    bottomBox.pack_start(removeButton);
    bottomBox.pack_start(closeButton);

    // Make the button the default widget
    closeButton.set_can_default();
    closeButton.grab_default();

    // Connect the clicked signals of buttons
    closeButton.signal_clicked().connect(
                sigc::mem_fun(*this, &ASMView::on_close_button_clicked));
    loadButton.signal_clicked().connect(
                sigc::mem_fun(*this, &ASMView::on_load_button_clicked));
    saveButton.signal_clicked().connect(
                sigc::mem_fun(*this, &ASMView::on_save_button_clicked));
    newButton.signal_clicked().connect(
                sigc::mem_fun(*this, &ASMView::on_new_button_clicked));
    removeButton.signal_clicked().connect(
                sigc::mem_fun(*this, &ASMView::on_remove_button_clicked));


    //Connect the signals of rows in the tree view for double click and enter
    treeView.signal_row_activated().connect(
                sigc::mem_fun(*this, &ASMView::on_scene_file_activated));
    //Connect the signals of rows in the tree view changed selection
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
            treeView.get_selection();
    refTreeSelection->signal_changed().connect(
                sigc::mem_fun(*this, &ASMView::on_scene_file_selection_changed));

    // Show all children of the window
    show_all_children();

}

ASMView::~ASMView() {
}

void ASMView::on_close_button_clicked() {
    hide(); //to close the application.
}

void ASMView::on_new_button_clicked() {
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
        struct stat st;

        if (stat(scenesDir.c_str(), &st) == -1) {
            mkdir(scenesDir.c_str(), 0700);
        }

        /*
         * Parse the file
         */
        sessionHandler.init(dialog.get_filename(), &myScene);

        char trackIds[8];
        char busIds[4];
        //Set the Id's for the OSC controller
        unsigned range;
        if(myScene.tracks.size() > 8){
            range = 8;
        }
        else
            range =myScene.tracks.size();
        for(unsigned i =0; i<range; i++){
            trackIds[i] = myScene.tracks[i].getId();
        }
        if(myScene.busses.size() > 4){
            range = 4;
        }
        else
            range =myScene.busses.size();
        for(unsigned i=0; i<range; i++) {
            busIds[i] = myScene.busses[i].getId();
        }
        oscServer.setIds(trackIds,busIds);

        /*
         * Update the GUI layout
         */
        saveButton.set_sensitive(true);

        detailGrid.set_sensitive(true);
        showSceneDetails();
        scrolledWindow.set_sensitive(true);
        scanAvailableSceneFiles();
        break;
    }
    case (Gtk::RESPONSE_CANCEL): {
        //Well nothing
        //std::cout << "Cancel clicked." << std::endl;
        break;
    }
    default: {
        //haha nothing
        break;
    }
    }

}

void ASMView::on_load_button_clicked() {
    loadNewSceneFile();
}

void ASMView::on_save_button_clicked() {
    //Get the scene name
    myScene.setName(nameEntry.get_text());

    //bool ok = false;
    string fileName = scenesDir;

    //append the name entry to the file name
    fileName += myScene.getName();

    /*
     * Save to the scene foldr
     */
    sceneParser.saveSceneToFile(&myScene, fileName);

    //Make the current scene the one we just saved
    sceneFileName = fileName;
    showSceneDetails();
    scanAvailableSceneFiles();
}

void ASMView::on_remove_button_clicked() {
    /*
     * Remove the selected file
     */
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
            treeView.get_selection();
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        //Get the file name to remove from the row
        string sceneFileToRemove = scenesDir;
        sceneFileToRemove += row.get_value(scenesList.fileNameColumn);

        //Remove the file
        std::remove(sceneFileToRemove.c_str());
    }

    /*
     * Re-scan the available scene files
     */
    scanAvailableSceneFiles();

}

void ASMView::showSceneDetails() {
    char num[5];
    char updated[5];
    //get num of tracks
    sprintf(num, "%d",
            (int) myScene.tracks.size());
    numTracksLabel.set_text(num);

    //get num of busses
    sprintf(num, "%d", (int) myScene.busses.size() +1 /*Always a master bus*/);
    numBussesLabel.set_text(num);
    //Get the number of updated tracks
    sprintf(updated, "%d", myScene.getUpdatedTracks());
    updatedTracksLabel.set_text(updated);

    //get num of updated busses
    sprintf(updated, "%d", myScene.getUpdatedBusses());
    updatedBussesLabel.set_text(updated);


    nameEntry.set_text(myScene.getName());

}

void ASMView::scanAvailableSceneFiles() {
    //Create the Tree model:
    refTreeModel = Gtk::ListStore::create(scenesList);
    treeView.set_model(refTreeModel);
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
        Gtk::TreeModel::Row row = *(refTreeModel->append());
        row[scenesList.fileNamePresentationColumn] = withoutExt;
        row[scenesList.fileNameColumn] = sceneFiles[i];
    }
}

void ASMView::on_scene_file_activated(const Gtk::TreeModel::Path &,
                                      Gtk::TreeViewColumn*) {
    loadNewSceneFile();
}

void ASMView::on_scene_file_selection_changed() {
    /*
     * If something is selected enable load and remove
     */
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
            treeView.get_selection();
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if (iter) {
        loadButton.set_sensitive(true);
        removeButton.set_sensitive(true);
    } else {
        loadButton.set_sensitive(false);
        removeButton.set_sensitive(false);
    }
}

void ASMView::loadNewSceneFile() {
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
            treeView.get_selection();
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        //Get the file name to load from the row
        string newSceneFile = scenesDir;
        newSceneFile += row.get_value(scenesList.fileNameColumn);
        sceneFileName = newSceneFile;

        /*
         * Parse the file
         */
        sceneParser.loadSceneFromFile(&myScene, sceneFileName);

        /*
         * And then send it to Ardour
         */
        myScene.sendSceneToArdour();

        saveButton.set_sensitive(true);
        showSceneDetails();
    }
}

Scene *ASMView::getScene(){
    return &myScene;
}

bool ASMView::idleFunction() {

    //Check if there is anything from the jack client to update the scene with
    unsigned availableRead = jack_ringbuffer_read_space(sceneUpdateBuffer);

    if ( availableRead >= sizeof(MidiEvent) ) {

        char init[3] ={0};
        MidiEvent midiEvent(init);
        jack_ringbuffer_read(sceneUpdateBuffer,(char*)&midiEvent,sizeof(MidiEvent));

        //Do some scene updating logic
        sceneUpdateHandler(midiEvent);

    }


    /*

    availableRead = jack_ringbuffer_read_space(controllerUpdate);


    if ( availableRead >= sizeof(ControllerUpdateEvent) ) {
        //read what the controller wants to be send back
        ControllerUpdateEvent updateReq("",0);
        jack_ringbuffer_read(controllerUpdate,(char*)&updateReq,sizeof(ControllerUpdateEvent));

        std::string path;
        if(updateReq.what == "trackBank"){
            unsigned startIndex = updateReq.bankIndex*8;
            char ids[8];
            for(unsigned i=startIndex; i<startIndex+8; i++){
                if(!(i<myScene.tracks.size())){
                    break;
                }
                //send to the controller
                path = "/controller/track/fader/"+std::to_string(myScene.tracks[i].getId());
                ids[i] = myScene.tracks[i].getId();
                oscServer.sendToController(path, (float) myScene.tracks[i].getGain());
            }
            oscServer.setTrackIds(ids);


        } else if(updateReq.what == "busBank"){
            unsigned startIndex = updateReq.bankIndex*4;
            char ids[4];
            for(unsigned i=startIndex; i<startIndex+4; i++){
                if(!(i<myScene.busses.size())){
                    break;
                }
                //send to the controller
                path = "/controller/bus/fader/"+std::to_string(myScene.busses[i].getId());
                ids[i]= myScene.busses[i].getId();
                oscServer.sendToController(path, (float) myScene.busses[i].getGain());
            }
            oscServer.setBusIds(ids);
        }


    }
    */


    return true;
}

void ASMView::sceneUpdateHandler(MidiEvent &midiEvent){
    char id = midiEvent.data[1];
    char gain = midiEvent.data[2];

    char statusByte = (unsigned char) midiEvent.data[0] >> 4;
    char channel = midiEvent.data[0] & 0x0F;
    std::string path;

    if (statusByte == CC_NIBBLE) {
        if(channel == 0) {
            //This is a message for master, a track, or a bus
            bool found = false;

            if (id == MASTER_CC || id == MASTER_CC2) {
                myScene.master.setGain(gain);
                myScene.master.setModified(true);
                found = true;
                showSceneDetails();
                path = "/controller/master/fader";
                //send to the controller
                oscServer.sendToController(path, (float) gain);
            }

            if(!found) {
                for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                    if(id == myScene.tracks[i].getId()) {
                        myScene.tracks[i].setGain(gain);
                        myScene.tracks[i].setModified(true);
                        found = true;
                        showSceneDetails();

                        //send to the controller
                        path = "/controller/track/fader/"+std::to_string((int)i+1);
                        oscServer.sendToController(path, (float) gain);
                        break;
                    }
                }
            }

            //Go through the busses
            if(!found) {
                for(unsigned int i =0; i<myScene.busses.size(); i++) {
                    if((char) id == (char) myScene.busses[i].getId()) {
                        myScene.busses[i].setGain(gain);
                        myScene.busses[i].setModified(true);
                        found = true;
                        showSceneDetails();

                        //send to the controller
                        path = "/controller/bus/fader/"+std::to_string((int)i+1);
                        oscServer.sendToController(path, (float) gain);
                        break;
                    }
                }
            }
        }
    }

    //a CC message on a channel other than 1 is for a send
    else {
        // std::cout<< "Got a message on channel" << (int) channel << "For track" <<  (int) id << "\n";
        //This is a message for a send on master(soon), a track, or a bus
        bool found = false;
        if(!found) {
            for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                if(id == myScene.tracks[i].getId()) {
                    if((unsigned) channel <= myScene.tracks[i].sends.size()) {
                        myScene.tracks[i].sends[channel-1].setGain(gain);
                        myScene.tracks[i].sends[channel-1].setModified(true);
                        found =true;
                        showSceneDetails();
                        break;
                    }
                }
            }
        }

        //Go through the busses
        if(!found) {
            for(unsigned int i =0; i<myScene.busses.size(); i++) {
                if(id == myScene.busses[i].getId()) {
                    if((unsigned) channel <= myScene.busses[i].sends.size()) {
                        myScene.busses[i].sends[channel-1].setGain(gain);
                        myScene.busses[i].sends[channel-1].setModified(true);
                        found =true;
                        showSceneDetails();
                        break;

                    }
                }
            }
        }


    }

    if (statusByte == NOTE_ON_NIBBLE) {
        if (channel == 0) {
            //mute a track, or bus
            //This is a message for master, a track, or a bus
            bool found = false;

            if (id == MASTER_CC || id == MASTER_CC2) {
                myScene.master.setMuted(true);
                myScene.master.setModified(true);
                found = true;
                showSceneDetails();
            }


            if(!found) {
                for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                    if(id == myScene.tracks[i].getId()) {
                        myScene.tracks[i].setMuted(true);
                        myScene.tracks[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }

            //Go through the busses
            if(!found) {
                for(unsigned int i =0; i<myScene.busses.size(); i++) {
                    if(id == myScene.busses[i].getId()) {
                        myScene.busses[i].setMuted(true);
                        myScene.busses[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }
        }

        if(channel == 1) {
            //solo a track or bus
            //This is a message for master, a track, or a bus
            bool found = false;
            if(!found) {
                for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                    if(id == myScene.tracks[i].getId()) {
                        myScene.tracks[i].setSoloed(true);
                        myScene.tracks[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }

            //Go through the busses
            if(!found) {
                for(unsigned int i =0; i<myScene.busses.size(); i++) {
                    if(id == myScene.busses[i].getId()) {
                        myScene.busses[i].setSoloed(true);
                        myScene.busses[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }
        }
    }

    if (statusByte == NOTE_OFF_NIBBLE) {
        if (channel == 0) {
            //mute a track, or bus
            //This is a message for master, a track, or a bus
            bool found = false;

            if (id == MASTER_CC || id == MASTER_CC2) {
                myScene.master.setMuted(false);
                myScene.master.setModified(true);
                found = true;
                showSceneDetails();
            }

            if(!found) {
                for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                    if(id == myScene.tracks[i].getId()) {
                        myScene.tracks[i].setMuted(false);
                        myScene.tracks[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }

            //Go through the busses
            if(!found) {
                for(unsigned int i =0; i<myScene.busses.size(); i++) {
                    if(id == myScene.busses[i].getId()) {
                        myScene.busses[i].setMuted(false);
                        myScene.busses[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }

        }

        if(channel == 1) {
            //solo a track or bus
            //This is a message for master, a track, or a bus
            bool found = false;
            if(!found) {
                for(unsigned int i =0; i<myScene.tracks.size(); i++) {
                    if(id == myScene.tracks[i].getId()) {
                        myScene.tracks[i].setSoloed(false);
                        myScene.tracks[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }

            //Go through the busses
            if(!found) {
                for(unsigned int i =0; i<myScene.busses.size(); i++) {
                    if(id == myScene.busses[i].getId()) {
                        myScene.busses[i].setSoloed(false);
                        myScene.busses[i].setModified(true);
                        found = true;
                        showSceneDetails();
                    }
                }
            }
        }
    }



}
