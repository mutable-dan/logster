//
// Created by gdan on 9/2/26.
//

#pragma once
#include <string>

#include <unistd.h>

namespace logster
{
    class log_reader
    {
        private:
            std::string m_strLogPath;
            bool        m_bUseMemMap = false;

            bool    readLogPosix();
            bool    readLogMemMap();
            int     m_fd;               // file desc


        public:
            explicit log_reader( const std::string strLogPath ) : m_strLogPath( strLogPath ) {};
            ~log_reader();
            log_reader( const log_reader& ) = delete;
            log_reader( const log_reader&& ) = delete;
            log_reader& operator=( const log_reader& ) = delete;

            void setLogPath( const std::string& strLogPath );
            void useMemMap( );
            bool readLog();

    };
}