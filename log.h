//
// class log
//
// This class encapsulates the log file functionality
//

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
		static const int MAX_LOG_STRING = 1024;
		static const string DEFAULT_LOG_FILE_NAME;
		// Put the next line in log.cc:
		// const string log::DEFAULT_LOG_FILE_NAME = "log.txt";
		static const int SUCCESS = 1;
		static const int FAILURE = 0;

		string logfilename;     // Log file name
		fstream logF;           // Log file variable
		string getTimeStamp(); // Get the timestamp value

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

		// Write a string to the log
		int writeLogRecord(string s);
}; // class log

#endif
