/**
 * A simple logger class.
 * @author Jarred Light <jlightd@gmail.com>
 */

// Multiple-include protection
#ifndef LOG_H
#define LOG_H

// Uses C++ strings for convenience
#include <string>
// For the log file output
#include <fstream>
using namespace std;

//
// class log:
//    Opens a text log file, timestamp at beginning,
//       writes string entries, timestamp at end
//
class log 
{
	private:
		// Some constants
		static const int MAX_LOG_STRING = 2048;
		static const string DEFAULT_LOG_FILE_NAME;
		// Put the next line in log.cc:
		// const string log::DEFAULT_LOG_FILE_NAME = "log.txt";
		static const int SUCCESS = 1;
		static const int FAILURE = 0;

		string logfilename;     // Log file name
		fstream logF;           // Log file variable
		string getTimestamp(); // Get the timestamp value
		// converts a log message into a json readable format
		string toJsonMessage(string logLevel, string message, string timestamp);
		// Write a string to the log file
		int writeLogRecord(string s);

	public:
		// Constructors
		// Default constructor: set the default file name
		log();

		// Overloaded char* constructor
		log(char* lname);

		// Overloaded string class constructor
		log(string lname);

		// Setters
		// set the log file name
		void setLogfileName(string cname);

		// Getters
		// Get the log file name
		string getLogfileName();

		// Get the default log file name
		static string getDefaultLogfileName();

		// Log functions
		// Return SUCCESS or FAILURE
		//
		// open the log with timestamp
		int open();

		// close the log with timestamp
		int close();

		// writes an informational message to the terminal and file
		int writeInfoMessage(string message);

		// writes a message that warrants attention
		int writeWarningMessage(string message);

		// write a message that warrants attention immediately
		int writeErrorMessage(string message);
}; // class log

#endif
