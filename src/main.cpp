
#include <iostream>
#include <string>

#include <cxxopts.hpp>

#include "log_reader.h"
#include <iostream>

using namespace std;

int main( const int argc, const char *argv[] )
{
    cxxopts::Options options( "reasoncode", "parse reason codes" );

    options.add_options()
        ( "l,log", "path to log file", cxxopts::value<std::string>() )
        ( "h,help", "Usage"                   );

    try
    {
        string strLog_file;

        options.allow_unrecognised_options();
        auto result = options.parse( argc, argv );
        if( result.count( "help" ) )
        {
            std::cout << options.help() << std::endl;
            return -1;
        } else
        {
            if( result.count( "log" ) )
            {
                strLog_file = result["log"].as<std::string>();
            }
        }

        // check if buffer can be spit intwo
        if( (logster::g_pgSize % 2) != 0 )
        {
            cerr << "ERROR: buffer size must be a multiple of 2" << endl;
            return -1;
        }

        logster::log_reader log2( false );
        logster::log_reader log( false );
        log.open( strLog_file );

        log.getLine();

        log.close();

        ;

    } catch( exception& e )
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}