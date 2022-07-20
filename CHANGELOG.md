# Changelog

* v1.2.5:
	* Improved reliability of counting pulses with hx616 coin acceptor
	* Fix for non-integer coin increment value
	* Disinhibit DG600F after initialization
* v1.2.4:
	* Better initialization state handling
* v1.2.3:
	* Fix getInt/Short/Float config value when set to empty-string
* v1.2.2:
	* Added spiffsFormatted config flag
	* Return spiffsInitialized flag in getinfo JSON-RPC command
	* Send response for restart JSON-RPC command
* v1.2.1:
	* Smaller log file size
* v1.2.0:
	* Use SPIFFS file system to store logs
	* JSON-RPC via serial - echo, getconfig, setconfig, getlogs, deletelogs, spiffs_reformat, restart
	* Move coin/button build flags to configuration options
	* Button delay no longer blocks the application loop
