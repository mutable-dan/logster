//
// Created by gdan on 9/2/26.
//

#pragma once
#include <cstdint>
#include <string>
#include <tuple>
#include <unistd.h>

namespace logster
{
    constexpr size_t g_nMemPageCount = 2;
    const size_t g_pgSize = sysconf( _SC_PAGESIZE );
    const size_t g_lMemTotalSize = g_pgSize * g_nMemPageCount;

    using buffer_t = std::uint8_t*;
    using read_t   = std::tuple<bool, buffer_t>;

    class log_reader final
    {
        private:
            bool            m_bUseMemMap        = false;
            bool            m_bIsLogOpen        = false;
            int             m_fd                = 0;         // file desc
            uint8_t        *m_pBuffer           = nullptr;

            uint8_t        *m_pCurrentBuffer    = nullptr;

            bool        readLogBuffer();
            bool        readLogMemMap();
            read_t      readPage()          noexcept;
            void        fillBuffer();


        public:
            log_reader()                                = delete;
           ~log_reader();
            log_reader( const log_reader& )             = delete;
            log_reader( const log_reader&& )            = delete;
            log_reader& operator=( const log_reader& )  = delete;
            log_reader& operator=( const log_reader&& ) = delete;

            explicit log_reader( bool a_buUseMap = false );

            buffer_t getLine();
            bool     open( const std::string& strLogPath );
            bool     close();
            size_t   getBufferSize() const { return( g_lMemTotalSize ); };

    };
}
