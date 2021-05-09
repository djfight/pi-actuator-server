/**
 * A simple logger class.
 * @author Jarred Light <jlightd@gmail.com>
 */

#include <string.h>
#include <string>
#include <ctime>
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
        log::writeLogRecord("BEGIN");
		
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
    log::writeLogRecord("END");
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
       logF << log::getTimeStamp();
       logF << s << endl;
       return log::SUCCESS;
    }
	else
	{
       return log::FAILURE;
    }
}

//get the current timestamp
string log::getTimeStamp()
{
    time_t result = time(0);
    char* currentTime = ctime(&result);
	
    return currentTime;
}