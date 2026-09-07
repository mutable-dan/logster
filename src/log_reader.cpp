//
// Created by gdan on 9/2/26.
//

#include "../include/log_reader.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

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
    if( m_fd >= 0 )  [[likely]]
    {
        if( m_bUseMemMap )
        {
            munmap(  m_pBuffer, g_pgSize );
            m_pBuffer = nullptr;
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
        m_fd = ::open( strLogPath.c_str(), O_RDONLY );
        if( m_fd >= 0 )  [[likely]]
        {
            // likely not 0, 1, 2
            m_bIsLogOpen = true;
            posix_memalign( reinterpret_cast<void**>(&m_pBuffer), g_pgSize, g_pgSize );

            if( m_bUseMemMap )
            {
                // consider: MAP_HUGE_1GB
                m_pBuffer = static_cast<uint8_t*>( mmap( NULL, g_pgSize, PROT_READ , MAP_PRIVATE, m_fd, 0 ) );
            } else
            {
                m_pBuffer = new uint8_t[ g_pgSize ];
            }

            return true;
        }
    }
    return false;
}


bool inline logster::log_reader::getLine()
{
    if( false == m_bIsLogOpen )
    {
        return false;
    }

    if( m_bUseMemMap )
    {
        return readLogMemMap();
    } else
    {
        return readLogPosix();
    }
}

bool logster::log_reader::readPage()
{
    if( m_bUseMemMap )
    {
        [[maybe_unused]] ssize_t nRead = ::read( m_fd, m_pBuffer, g_pgSize );
        //(void)nRead;
    } else
    {

    }
    return true
    ;
}


bool logster::log_reader::readLogPosix()
{
   return true;
}

bool logster::log_reader::readLogMemMap()
{
    return false;
}