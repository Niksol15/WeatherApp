#pragma once

class Application
{
public:
    Application(int argc, char* argv[]);

    // The program should look like the example below, if no arguments are written
    /**
     *   $ ./WeatherApp
     *   Main options:
     *   -h [ --help ]         Show help
     *   -c [ --city ] arg     Enter city (If the city name is divided into several 
     *                           words - use _ instead of space)
     *   -t [ --token ] arg    Enter token
     * */

    // A standart output should look like the example below
    /**
     *  $ ./WeatherApp --city=Kyiv
     *  City: Kyiv
     *  Temperature: 0°C
     *  Wind's speed: 2 meter/sec
     *  Wind's direction: 140°
     * */

    /**
     * @brief Parse command-line arguments ( Need to use boost::program_options ). Call Client
     * */
    int exec();
private:
    int argc;
    char** argv;
};