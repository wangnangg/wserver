#include "option.hpp"
#include <getopt.h>
#include <stdexcept>

void parseArg(int argc, char* argv[], CmdOption& option)
{
    int c;
    struct option long_options[] = {{"ip", required_argument, 0, 'i'},
                                    {"port", required_argument, 0, 'p'},
                                    {0, 0, 0, 0}};
    while (1)
    {
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "i:p:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) break;

        switch (c)
        {
            case 'i':
                option.ipv4_addr = optarg;
                break;

            case 'p':
                option.port = optarg;
                break;

            case '?':
                throw std::invalid_argument(
                    "unknown option or missing argument.");
                break;

            default:
                abort();
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        throw std::invalid_argument("unused arguments found.");
    }
}
