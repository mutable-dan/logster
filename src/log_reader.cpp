//
// Created by gdan on 9/2/26.
//

#include "../include/log_reader.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <ostream>
#include <format>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string.h>

using namespace std;

logster::log_reader::log_reader( bool a_bUseMemMap )
{
    m_bUseMemMap = a_bUseMemMap;
    m_pBuffer   = nullptr;
}

logster::log_reader::~log_reader()
{
    // i know this has no impact on per, since likely called once
    if( m_fd >= 0 )  [[unlikely]]
    {
        this->close();
    }
}

/**
 *
 * @param a_nPages set page count where each page is of size m_pgSize
 * @return true if set, false if mem has alreaded been alloc
 */
bool logster::log_reader::setPages( int32_t a_nPages ) noexcept
{
    if( false == m_bIsMemAlloc )
    {
        m_nMemPageCount = a_nPages;
        m_lMemTotalSize = a_nPages * m_pgSize;
        return true;
    }
    return false;
}

bool logster::log_reader::close()
{
    if( m_fd >= 3 )  [[likely]]
    {
        m_bIsFileOpen = false;
        if( m_bUseMemMap )
        {
            munmap(  m_pBuffer, m_pgSize );
            m_pBuffer = nullptr;
        } else
        {
            if( m_pBuffer != nullptr )
            {
                free( m_pBuffer );
                m_pBuffer = nullptr;
            }
        }
        if( nullptr != m_ppCurrentBuffer )
        {
            delete[] m_ppCurrentBuffer;
            m_ppCurrentBuffer = nullptr;
        }
        if( 0 == ::close( m_fd ) )
        {
            m_fd = -1;
            return true;
        }
    }
    return false;
}



/**
 * @brief opens file and eithr alloc aligned buffer or assign ptr to memmap
 * @param strLogPath
 * @return bool if file was opened: true, if failed to open or open: false
 */
bool logster::log_reader::open( const std::string& strLogPath )
{
    if( false == m_bIsFileOpen )
    {
        m_fd = ::open( strLogPath.c_str(), O_RDONLY );
        if( m_fd >= 0 )  [[likely]]
        {
            // likely not 0, 1, 2
            m_bIsFileOpen = true;

            if( m_bUseMemMap )
            {
                // consider: MAP_HUGE_1GB
                m_pBuffer = static_cast<uint8_t*>( mmap( NULL, m_pgSize, PROT_READ , MAP_PRIVATE, m_fd, 0 ) );
                m_bIsMemAlloc = true;
            } else
            {
                if( 0 != posix_memalign( reinterpret_cast<void**>( &m_pBuffer ), m_pgSize, m_lMemTotalSize ) )
                {
                    return false;
                }
                m_bIsMemAlloc = true;
            }
            // m_ppCurrentBuffer = static_cast<uint8_t**>( malloc( m_nMemPageCount * sizeof(uint8_t*) ) );
            // for( int16_t ndx = 0; ndx < (int16_t)m_nMemPageCount; ++ndx )
            // {
            //     m_ppCurrentBuffer[ndx] = m_pBuffer + ndx * m_pgSize * sizeof(uint8_t);
            // }

            m_ppCurrentBuffer = new uint8_t*[ m_nMemPageCount ];
            for( int16_t ndx = 0; ndx < (int16_t)m_nMemPageCount; ++ndx )
            {
                m_ppCurrentBuffer[ndx] = m_pBuffer + ndx * m_pgSize * sizeof(uint8_t);
            }

            return true;
        } else
        {
            cerr << std::format( "ERROR: {} {} opening log {}", strerror( errno ), errno, strLogPath ) << endl;
        }
    }
    return false;
}


/**
 * @brief read asignle line from bufffer. line ends with CR or LF
 * @return
 */
logster::buffer_t logster::log_reader::getLine()
{
    if( false == m_bIsFileOpen )
    {
        return nullptr;
    }

    if( m_bUseMemMap )
    {
        return nullptr;
    } else
    {
        auto [bRes, pBuff] = readPage();
        if( bRes )
        {
            return pBuff;
        } else
        {
            return nullptr;
        }
    }
}

logster::read_t logster::log_reader::readPage() noexcept
{
    fillBuffer();
    // if( m_bUseMemMap )
    // {
    // } else
    // {
    //     [[maybe_unused]] ssize_t nRead = ::read( m_fd, *m_ppBuffer, m_pgSize );
    //     if( m_pCurrentBuffer == nullptr )
    //     {
    //         m_pCurrentBuffer = m_pBuffer;
    //     } else
    //     {
    //         m_pCurrentBuffer = m_pCurrentBuffer + m_pgSize;
    //         if( m_pCurrentBuffer > m_pBuffer + m_pgSize )
    //         {
    //             m_pCurrentBuffer = m_pBuffer;
    //         }
    //     }
    //     return { true, m_pCurrentBuffer };
    // }
    return { false, nullptr };
}


bool logster::log_reader::readLogBuffer()
{
    return  false;

}

bool logster::log_reader::readLogMemMap()
{
    return false;
}


/**
 * @brief thread keeps buffer filled by page. clsoes when EOF
 * @details for a buffer of size b, split into p pages. if page size is
 * when first runs, it tries to fill all of the pages with log data
 * every time the page index increments, it fills the empty buffer with data
 */
void logster::log_reader::fillBuffer()
{
    size_t lIndex = 0;
    

    bool bEof = false;
    for( uint16_t ndx = 0; ndx < (uint16_t)m_nMemPageCount; ++ndx )
    {
        memset( (void*)( m_pBuffer + ndx*m_pgSize*sizeof(uint8_t) ), '0', m_pgSize*sizeof(uint8_t) );
        ssize_t nRead = ::read( m_fd, (m_pBuffer + ndx*m_pgSize*sizeof(uint8_t) ), m_pgSize );
        if( nRead == 0 )
        {
            break;
            bEof = true;
        }
    }
    if( true == bEof)
    {
        return;
    }

    while( lIndex < m_nMemPageCount )
    {

    }
    // unique_lock< std::mutex > condLock( m_muxBufferLock );
    // m_cvBuffer.wait( condLock,  [this]() { return m_bConditPred; } );


}