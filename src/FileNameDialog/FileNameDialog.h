/*
 * FileNameDialog.h
 *
 *  Created on: 2013-03-09
 *      Author: jamie
 */

#ifndef FILENAMEDIALOG_H_
#define FILENAMEDIALOG_H_

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

using namespace std;

class FileNameDialog: public Gtk::Dialog {
	string fileName;
public:
	FileNameDialog();
	virtual ~FileNameDialog();
	string getFileName();
protected:
	Gtk::Box box;
	Gtk::Label label;
	Gtk::Entry entry;
};

#endif /* FILENAMEDIALOG_H_ */
