/*
 * FileNameDialog.cpp
 *
 *  Created on: 2013-03-09
 *      Author: jamie
 */

#include "FileNameDialog.h"

FileNameDialog::FileNameDialog() :
		label("Please enter the file name to save the scene with") {
	set_title("Choose File Name");

	//Pack the dialog
	get_vbox()->pack_start(label);
	get_vbox()->pack_start(entry);

	//register signal handlers

	show_all_children();

}

FileNameDialog::~FileNameDialog() {
	// TODO Auto-generated destructor stub
}

string FileNameDialog::getFileName() {
	return entry.get_text();
}

