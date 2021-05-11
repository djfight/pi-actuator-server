/**
 * A simple logger class.
 * @author Jarred Light <jlightd@gmail.com>
 */

#include <string.h>
#include <string>
#include <ctime>
#include <iostream>
#include<sstream>

#ifndef LOG_H
#include "log.h"
#endif

const string log::DEFAULT_LOG_FILE_NAME = "log.txt";

//default constructor
log::log() 
{
    logfilename = log::DEFAULT_LOG_FILE_NAME;
}

//overload char* constructor
log::log(char* lname)
{
    logfilename = lname;
}

//overload string class constructor
log::log(string lname)
{
    logfilename = lname;
}

//setters
//set the log file name
void log::setLogfileName(string cname)
{
    logfilename = cname;
}

//getters
//get the log file name
string log::getLogfileName()
{
    return logfilename;
}

//get the default log file name
string log::getDefaultLogfileName()
{
    return DEFAULT_LOG_FILE_NAME;
}

//open the log with a timestamp
int log::open()
{
    logF.open(logfilename.c_str(), ios::out | ios::app);//open the file
    //if the file is open the return success else failure
    if(logF.is_open())
	{
        log::writeInfoMessage("BEGIN");
		
        return log::SUCCESS;
    }
	else
	{
        return log::FAILURE;
    }
}

//close the log file with a timestamp
int log::close()
{
    log::writeInfoMessage("END");
    logF.close();
    if(logF.is_open())
	{
        return log::FAILURE;
    }
	else
	{
        return log::SUCCESS;
    }
}

//write a string to the log with a timestamp
int log::writeLogRecord(string s)
{
    if(strlen(s.c_str()) <= log::MAX_LOG_STRING)
	{
       logF << s;

       return log::SUCCESS;
    }
	else
	{
       return log::FAILURE;
    }
}

int log::writeInfoMessage(string message) 
{
    string timestamp = log::getTimestamp();

    string jsonLogMessage = log::toJsonMessage("INFO", message, timestamp);

    std::cout << jsonLogMessage;

    return log::writeLogRecord(jsonLogMessage);
}

int log::writeWarningMessage(string message) 
{
    string timestamp = log::getTimestamp();

    string jsonLogMessage = log::toJsonMessage("WARNING", message, timestamp);

    std::cout << jsonLogMessage;

    return log::writeLogRecord(jsonLogMessage);
}

int log::writeErrorMessage(string message) 
{
    string timestamp = log::getTimestamp();

    string jsonLogMessage = log::toJsonMessage("ERROR", message, timestamp);

    std::cout << jsonLogMessage;

    return log::writeLogRecord(jsonLogMessage);
}

string log::toJsonMessage(string logLevel, string message, string timestamp)
{
    std::stringstream jsonOutputStream;
    jsonOutputStream << "{ \"logLevel\":\"" << logLevel << "\", \"timestamp\": \"" << timestamp << "\", \"message\": \"" << message << "\" }\n";
    return jsonOutputStream.str();
}

//get the current timestamp
string log::getTimestamp()
{
    time_t currentTime = time(0);
    tm* time = localtime(&currentTime);

    char buffer[256];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S-%z", time);
	
    return buffer;
}