
/** ***************************************************************************
 *
 *                             Password Generator
 *
 *  ***************************************************************************
 *
 *  Author: Jose Fernando Lopez Fernandez
 *
 *  Date:	05-May-2018
 *
 *  TODO:
 *      1. Pass in length of password                          - DONE
 *      2. Pass in number of passwords to generate             - DONE
 *      3. Improve random number generation
 *      4. Explore using other distributions
 *      5. Implement program options                           - IN PROGRESS
 *      6. Pass in password charset rule (regex)
 *      7. Time generation tracker
 *      8. Setup version tracker                               - DONE
 *      9. Create program configuration class/manager?
 *     10. Customize PRNG
 *     11. Implement logger class (?)
 *     12. Parse configuration file
 *     13. Parse environment
 *     14. Generate password from regex                        - CRITICAL
 *     15. Calculate password entropy                          - CRITICAL
 *
 *  **************************************************************************/

#ifdef UNICODE
#undef UNICODE
#endif

#include <iostream>
#include <sstream>
#include <string>

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/regex.hpp>

#include "version.h"

namespace Options = boost::program_options;

using OptsDescription = boost::program_options::options_description;

struct Constants {
    static constexpr auto FALSE = 0;
    static constexpr auto TRUE = 1;
};

struct Configuration {
    static constexpr auto verbose = Constants::FALSE;
};

inline void PrintHelp(const OptsDescription& description) {
	PrintVersion();

	std::cout << description << "\n";
}

int main(int argc, char *argv[])
{
	size_t n = 1;
	size_t length = 8;

	boost::regex mask;

    boost::optional<size_t> entropy;
	boost::optional<std::string> configFile;

	try {
		OptsDescription generic("Generic options");
		generic.add_options()
			("help,h", "Display help dialog")
			("version,v", "Display program version")
			("password-length,l", Options::value<size_t>(&length)->default_value(8), "Password length")
			("passwords,n", Options::value<size_t>(&n)->default_value(1), "Number of passwords to generate")
            ("entropy", Options::value(&entropy), "Minimum password entropy tolerance")
            ("configuration-file,f", Options::value(&configFile), "Configuration file to use")
			//("mask,m", Options::value<boost::regex>(&mask)->default_value("[a-zA-Z0-9]{8}"), "Password mask (pattern)")
			;

		Options::variables_map map;
		Options::store(Options::parse_command_line(argc, argv, generic), map);
		Options::notify(map);

		if (map.count("help")) {
			PrintHelp(generic);

			return EXIT_SUCCESS;
		}

		if (map.count("version")) {
			PrintVersion();

			return EXIT_SUCCESS;
		}

		if (map.count("password-length")) {
			std::cout << "\n";
			std::cout << "Password Length: " << length << "\n";
		}

		if (map.count("passwords")) {
			std::cout << "Number of passwords to generate: " << n << "\n";
		}

        if (entropy) {
            std::cout << "Minimum password entropy tolerance: " << *entropy << "\n";
        } else {
            if (Configuration::verbose) {
                std::cout << "<No minimum entropy tolerance set>\n";
            }
        }

		if (configFile) {
			std::cout << "Config file: " << *configFile << "\n";
		} else {
			if (Configuration::verbose) {
				std::cout << "<No configuration file>\n";
			}
		}

	} catch (std::exception& e) {
		std::cerr << "[Error]: " << e.what() << "\n";
	}

	const std::string charset(
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"1234567890"
		"!@#$%^&*()"
		"`~-_=+[{]}\\|;:'\",<.>/? "
	);

	boost::random::ranlux48 gen;
	boost::random::uniform_int_distribution<> indexDistribution(0, static_cast<int>(charset.size()) - 1);

	std::cout << "\n\n";

	for (auto count = 0; count < n; count++) {
		for (auto position = 0; position < length; position++) {
			std::cout << charset[indexDistribution(gen)];
		}

		std::cout << "\n";
	}

	std::cout << "\n\n";

	return EXIT_SUCCESS;
}
