#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Filesystem.hpp"
#include "Exception.hpp"

/**
 * Implement:
 * 		> Maybe file upload https://tttapa.github.io/ESP8266/Chap12%20-%20Uploading%20to%20Server.html
 * 
 * The Webserver is only used to get the Website, maybe make uploads or authentications but not for other
 * commands/interactions with the user because he disconnects from the client every time he sent the website.
 */

class Webserver: public ESP8266WebServer {
private:
	String workingDirPath;
	Filesystem& filesystem;

public:
	Webserver(Filesystem& _filesystem, int port = 80);
	virtual ~Webserver() { }

	// !!! It checks not if the directory _path is valid/exists !!!
	void setWorkingDir(String _path);

protected:
	/*
		https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
		The File-Type we need to pass if we stream a File to the client
	*/
	String fileType(String pathToFile) const;


	// Cant be a member function of Webserver-Class because then I cant pass the function at this->notFound(). Therefore I use 
	// the std::function with Lambda like described in this article: https://www.c-plusplus.net/forum/topic/282024/zeiger-auf-gebundene-funktion-als-funktionsparameter
	std::function<void(void)> handleWebServer = [&](void) {
		String URL = this->uri();

		// We add these to pathes together. we remove the slash because we have added it previously at the working directory!
		// https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
		if(URL[0] == '/') {
			URL = URL.substring(1);
		}

		// Adding the working directory to the path which was requested
		String pathToFile = this->workingDirPath + URL;

		// Add the index.html if we are only going into a directory
		if(pathToFile.endsWith("/")) {
			pathToFile += "index.html";
		}

		if(filesystem.exists(pathToFile)) {
			File file = filesystem.open(pathToFile, "r");
			this->streamFile(file, this->fileType(pathToFile));
			file.close();
		} else {
			// https://developer.mozilla.org/de/docs/Web/HTTP/Status
			this->send(404, "text/plain", "404: Not Found");
		}

		switch (this->method()) {
			case HTTP_POST: {
				// An action
				break;
			}		
			default:{
				break;
			}
		}
	};
};



// Implementation
String Webserver::fileType(String pathToFile) const {
	if		(pathToFile.isEmpty())				return "";
	else if	(pathToFile.endsWith(".html"))		return "text/html";
	else if	(pathToFile.endsWith(".css"))		return "text/css";
	else if (pathToFile.endsWith(".js"))		return "text/javascript";
	else if	(pathToFile.endsWith(".ico"))		return "image/vnd.microsoft.icon";
	else if (pathToFile.endsWith(".gz"))		return "application/gzip";
	else if (pathToFile.endsWith(".jpeg"))		return "image/jpeg";
	else if (pathToFile.endsWith("jpg"))		return "image/jpeg";
	else										return "text/plain";
}

Webserver::Webserver(Filesystem& _filesystem, int port): filesystem(_filesystem), ESP8266WebServer(port) {
	// https://www.c-plusplus.net/forum/topic/282024/zeiger-auf-gebundene-funktion-als-funktionsparameter
	this->onNotFound(this->handleWebServer);
}

void Webserver::setWorkingDir(String _path) {
	// Working dir must to be a directory - a directory ends wit a slash
	if(! _path.endsWith("/")) {
		workingDirPath = _path + "/";
	} else {
		workingDirPath = _path;
	}
}
