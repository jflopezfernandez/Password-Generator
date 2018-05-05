
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
 *      4. Explore using other probability distributions
 *      5. Implement program options                           - IN PROGRESS
 *      6. Pass in password charset rule (regex)
 *      7. Time generation tracker
 *      8. Setup version tracker                               - DONE
 *      9. Create program configuration class/manager?
 *     10. Customize PRNG
 *     11. Implement logger class (?)
 *     12. Parse configuration file
 *     13. Parse environment
 *     14. Generate password from mask                         - CRITICAL
 *     15. Calculate password entropy                          - CRITICAL
 *     16. Write a parser for regexp->string generation        
 *     17. Generate passwords with random words
 *     18. Modify words in password Tr0ub&dour style
 *     19. Accept restricted character set to avoid
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

#include <gsl/gsl>

#include "version.h"

namespace Options = boost::program_options;

using OptsDescription = boost::program_options::options_description;

const std::string charset(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "1234567890"
    "!@#$%^&*()`~-_=+[{]}\\|;:'\",<.>/? "
);

std::string restrictedCharset = "";

boost::random::ranlux48 gen;
boost::random::uniform_int_distribution<> indexDistribution(0, gsl::narrow<std::size_t, int>(charset.size()) - 1);

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

/** Password Mask rules:
 *
 *  * - any character
 *  l - lowercase letter
 *  L - capital letter
 *  d - digit
 *  s - symbol (no whitespace)
 *  ? - any symbol
 *
 */

// Complying with Warning C26451 - (https://docs.microsoft.com/en-us/visualstudio/code-quality/c26451)
const char LOWERCASE_START = gsl::narrow<char>('a');
const char UPPERCASE_START = gsl::narrow<char>('A');
const char ALPHA_RANGE     = gsl::narrow<char>('z' - 'a');
const char ONE             = gsl::narrow<char>(1);

char getNextCharacter(const char characterMask) {
    char newCharacter = '\0';

    do {
        newCharacter = [=](const char mask) -> char {
            switch (mask) {
                case '*': {
                    return charset[indexDistribution(gen)];
                } break;

                case 'l': {
                    return charset[(indexDistribution(gen) % ALPHA_RANGE) + LOWERCASE_START - ONE];
                } break;

                case 'L': {
                    // TODO: Fix
                    return charset[indexDistribution(gen)];
                }

                case 'd': {
                    // TODO: Fix
                    return charset[indexDistribution(gen)];
                }

                case 's': {
                    // TODO: Fix
                    return charset[indexDistribution(gen)];
                }

                case '?': {
                    // TODO: Fix
                    return charset[indexDistribution(gen)];
                }

                default:
                    return '0';
            }
        }(characterMask);

    // TODO: Check the new character is not on the restricted list
    } while (newCharacter == '\0');

    return '0';
}

int main(int argc, const char const *argv[])
{
    size_t n = 1;
	size_t length = 8;

	std::string mask = "********";

    boost::optional<size_t> entropy;
    boost::optional<std::string> inputMask;
    boost::optional<std::string> restrictedCharacters;
	boost::optional<std::string> configFile;

	try {
		OptsDescription generic("Generic options");
		generic.add_options()
			("help,h", "Display help dialog")
			("version,v", "Display program version")
			("password-length,l", Options::value<size_t>(&length)->default_value(8), "Password length")
			("passwords,n", Options::value<size_t>(&n)->default_value(1), "Number of passwords to generate")
            ("entropy", Options::value(&entropy), "Minimum password entropy tolerance")
            ("mask,m", Options::value(&inputMask), "Password mask (pattern)")
            ("restricted,r", Options::value(&restrictedCharacters), "Restricted characters")
            ("configuration-file,f", Options::value(&configFile), "Configuration file to use")
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

            if (inputMask) {
                if ((*inputMask).size() != length) {
                    std::cerr << "\n[Error]: password length and mask size are not equal.\n";
                    std::cerr << "\tMask [" << *inputMask << "]\n";
                    std::cerr << "\tSize: " << (*inputMask).size() << "\n";
                    std::cerr << "\tPassword Length: " << length << "\n\n";

                    return EXIT_FAILURE;
                }

                mask = *inputMask;
            }
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

        if (restrictedCharacters) {
            restrictedCharset = *restrictedCharacters;
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

	std::cout << "\n\n";

	for (auto count = 0; count < n; count++) {
        std::ostringstream newPassword { "" };

		for (auto position = 0; position < length; position++) {
            newPassword << getNextCharacter(mask[position]);
		}

		std::cout << newPassword.str() << "\n";
	}

	std::cout << "\n\n";

	return EXIT_SUCCESS;
}
