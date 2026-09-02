
#include <iostream>
#include <string>

#include <cxxopts.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
    cxxopts::Options options( "reasoncode", "parse reason codes" );

    options.add_options()
        ( "l,log", "path to log file", cxxopts::value<std::string>() )
        ( "h,help", "Usage"                   );

    try
    {
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
                const string strLog_file = result["log"].as<std::string>();
            }
        }
    } catch( exception& e )
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}