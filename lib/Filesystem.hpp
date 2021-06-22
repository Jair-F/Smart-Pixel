#include <FS.h>

#include "Exception.hpp"

#pragma once

/* SPIFFS ist
 FS SPIFFS = FS(FSImplPtr(new spiffs_impl::SPIFFSImpl(
							 FS_PHYS_ADDR,
							 FS_PHYS_SIZE,
							 FS_PHYS_PAGE,
							 FS_PHYS_BLOCK,
							 SPIFFS_MAX_OPEN_FILES)));
*/

// https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
class Filesystem : public FS {
public:
	Filesystem();
	virtual ~Filesystem() { SPIFFS.end(); }

	/**
		File open(_path, _mode)
		@param mode
		r	Open text file for reading.  The stream is positioned at the
			beginning of the file.

		r+	Open for reading and writing.  The stream is positioned at the
			beginning of the file.

		w	Truncate file to zero length or create text file for writing.
			The stream is positioned at the beginning of the file.

		w+	Open for reading and writing.  The file is created if it does
			not exist, otherwise it is truncated.  The stream is
			positioned at the beginning of the file.

		a	Open for appending (writing at end of file).  The file is
			created if it does not exist.  The stream is positioned at the
			end of the file.

		a+	Open for reading and appending (writing at end of file).  The
			file is created if it does not exist.  The initial file
			position for reading is at the beginning of the file, but
			output is always appended to the end of the file.
	*/
};

Filesystem::Filesystem(): FS(SPIFFS) {
	SPIFFSConfig config;
	// Normally the spiffs format itself if he faild to mount
	// Definiert hier, dass er sich bei Fehlschlag beim einbinden(mounten) nicht formatieren soll
	// We define here, that he should not format itself if he failed to mount
	config.setAutoFormat(false);
	this->setConfig(config);
	if(! this->begin()) {
		throw filesystem_error("Failed to start Filesystem");
	}
}