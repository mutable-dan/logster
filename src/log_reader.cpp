//
// Created by gdan on 9/2/26.
//

#include "../include/log_reader.h"


logster::log_reader::~log_reader()
{

}

void logster::log_reader::setLogPath( const std::string& strLogPath )
{
    m_strLogPath = strLogPath;
}

void logster::log_reader::useMemMap( )
{
    m_bUseMemMap = true;
}

bool logster::log_reader::readLog()
{
    if( m_bUseMemMap )
    {
        return readLogMemMap();
    } else
    {
        return readLogPosix();
    }
}


bool logster::log_reader::readLogPosix()
{
   return true;
}

bool logster::log_reader::readLogMemMap()
{
    return false;
}