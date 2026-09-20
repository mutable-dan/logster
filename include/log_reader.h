//
// Created by gdan on 9/2/26.
//

#pragma once
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>
#include <thread>
#include <unistd.h>

namespace logster
{
    using buffer_t = std::uint8_t*;
    using read_t   = std::tuple<bool, buffer_t>;

    class log_reader final
    {
        private:
            size_t          m_pgSize        = sysconf( _SC_PAGESIZE );
            size_t          m_nMemPageCount = 2;    // page size default to sytem page, the count is the number of pages used
            size_t          m_lMemTotalSize = m_pgSize * m_nMemPageCount;
            int             m_fd            = 0;            // file desc
            buffer_t        m_pBuffer       = nullptr;      // size of page * page count, all total buffer used
            bool            m_bUseMemMap    = false;        // false - poxic hheap alloc, else mempmap
            bool            m_bIsFileOpen   = false;        // false if path or other error and file not open
            bool            m_bIsMemAlloc   = false;        // true if mem was alloc on heap or memmap

            std::jthread                m_jThread;
            std::mutex                  m_muxBufferLock;
            std::condition_variable_any m_cvBuffer;
            bool                        m_bConditPred       = false;
            uint8_t                   **m_ppCurrentBuffer   = nullptr;

            bool        readLogBuffer();
            bool        readLogMemMap();
            read_t      readPage()          noexcept;
            void        fillBuffer();

            struct page_t
            {
                buffer_t   pPage;
                bool        bAvailable;
            };


        public:
            log_reader()                                = delete;
           ~log_reader();
            log_reader( const log_reader& )             = delete;
            log_reader( const log_reader&& )            = delete;
            log_reader& operator=( const log_reader& )  = delete;
            log_reader& operator=( const log_reader&& ) = delete;

            explicit log_reader( bool a_buUseMap = false );

            bool     setPages( int32_t a_nPages ) noexcept;
            int32_t  getPages() const noexcept { return m_nMemPageCount; }
            void     setpPageSize( int32_t a_nPageSize ) noexcept { m_pgSize = a_nPageSize; }
            buffer_t getLine();
            bool     open( const std::string& strLogPath );
            bool     close();
            size_t   getBufferSize() const { return( m_lMemTotalSize ); };

    };
}
