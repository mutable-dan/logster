//
// Created by gdan on 9/2/26.
//

#pragma once
#include <cstdint>
#include <string>
#include <unistd.h>

namespace logster
{
    const size_t g_pgSize = sysconf( _SC_PAGESIZE );

    class log_reader final
    {
        private:
            bool            m_bUseMemMap        = false;
            bool            m_bIsLogOpen        = false;
            int             m_fd                = 0;         // file desc
            uint8_t        *m_pBuffer           = nullptr;


            bool    readLogPosix();
            bool    readLogMemMap();
            bool    readPage();


        public:
            log_reader()                                = delete;
           ~log_reader();
            log_reader( const log_reader& )             = delete;
            log_reader( const log_reader&& )            = delete;
            log_reader& operator=( const log_reader& )  = delete;
            log_reader& operator=( const log_reader&& ) = delete;

            explicit log_reader( bool a_buUseMap = false );

            bool     getLine();
            bool     open( const std::string& strLogPath );
            bool     close();

    };
}
