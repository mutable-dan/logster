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
}

logster::log_reader::~log_reader()
{
    // i know this has no impact on per, since likely called once
    if( m_fd >= 0 )  [[unlikely]]
    {
        this->close();
    }
}

bool logster::log_reader::close()
{
    if( m_fd >= 3 )  [[likely]]
    {
        m_bIsLogOpen = false;
        if( m_bUseMemMap )
        {
            munmap(  m_pBuffer, g_pgSize );
            m_pBuffer = nullptr;
        } else
        {
            if( m_pBuffer != nullptr )
            {
                free( m_pBuffer );
                m_pBuffer = nullptr;
            }
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
 *
 * @param strLogPath
 * @return bool if file was opened: true, if failed to open or open: false
 */
bool logster::log_reader::open( const std::string& strLogPath )
{
    if( false == m_bIsLogOpen )
    {
        // m_lBufferSize = g_pgSize * ;
        m_fd = ::open( strLogPath.c_str(), O_RDONLY );
        if( m_fd >= 0 )  [[likely]]
        {
            // likely not 0, 1, 2
            m_bIsLogOpen = true;

            if( m_bUseMemMap )
            {
                // consider: MAP_HUGE_1GB
                m_pBuffer = static_cast<uint8_t*>( mmap( NULL, g_pgSize, PROT_READ , MAP_PRIVATE, m_fd, 0 ) );
            } else
            {
                posix_memalign( reinterpret_cast<void**>(&m_pBuffer), g_pgSize, g_lMemSize );
            }
            return true;
        } else
        {
            cerr << std::format( "ERROR: {} {} opening log {}", strerror( errno ), errno, strLogPath ) << endl;
        }
    }
    return false;
}


logster::buffer_t logster::log_reader::getLine()
{
    if( false == m_bIsLogOpen )
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
    if( m_bUseMemMap )
    {
    } else
    {
        [[maybe_unused]] ssize_t nRead = ::read( m_fd, m_pBuffer, g_pgSize );
        if( m_pCurrentBuffer == nullptr )
        {
            m_pCurrentBuffer = m_pBuffer;
        } else
        {
            m_pCurrentBuffer = m_pCurrentBuffer + g_pgSize;
            if( m_pCurrentBuffer > m_pBuffer + g_pgSize )
            {
                m_pCurrentBuffer = m_pBuffer;
            }
        }
        return { true, m_pCurrentBuffer };
    }
    return { false, nullptr };
    ;
}


bool logster::log_reader::readLogBuffer()
{
    return  false;

}

bool logster::log_reader::readLogMemMap()
{
    return false;
}
