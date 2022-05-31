# Changelog

* v1.2.1:
	* Smaller log file size
* v1.2.0:
	* Use SPIFFS file system to store logs
	* JSON-RPC via serial - echo, getconfig, setconfig, getlogs, deletelogs, spiffs_reformat, restart
	* Move coin/button build flags to configuration options
	* Button delay no longer blocks the application loop
