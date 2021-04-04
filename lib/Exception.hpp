#include <exception>
#include <stdexcept>

#ifndef _EXCEPTION_HPP_INCLUDED_
#define _EXCEPTION_HPP_INCLUDED_

class ESPException: public std::runtime_error {
public:
	ESPException(const char* _msg, bool _restart_ESP = false): restart_ESP(_restart_ESP), std::runtime_error(_msg) { }
	ESPException(const ESPException&) = default;
	virtual ~ESPException() { }
	ESPException& operator=(const ESPException&) = default;

	virtual bool restartESP() { return restart_ESP; }
private:
	const char* msg;
	bool restart_ESP;
};

class WiFi_Exception: public ESPException {
public:
	WiFi_Exception(const char* _msg, bool _restartESP = false): WiFi_Exception(_msg, _restartESP) { }
	WiFi_Exception& operator=(WiFi_Exception&) = default;
	WiFi_Exception(const WiFi_Exception&) = default;
	virtual ~WiFi_Exception() { }
};

class Spiffs_Exception: public ESPException{
public:
	Spiffs_Exception(const char* _msg, bool _restartESP = false): ESPException(_msg, _restartESP) { }
	Spiffs_Exception& operator=(Spiffs_Exception&) = default;
	Spiffs_Exception(const Spiffs_Exception&) = default;
	virtual ~Spiffs_Exception() { }
};

#endif // _EXCEPTION_HPP_INCLUDED_