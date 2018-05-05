
/** TODO:
 *  1. Pass in length of password
 *  2. Pass in number of passwords to generate
 *  3. Improve random number generation
 *  4. Explore using other distributions
 *  5. Implement program options
 *
 */

#include <iostream>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>


int main(int argc, char *argv[])
{
	size_t passwordLength = 8;

	const std::string charset(
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"1234567890"
		"!@#$%^&*()"
		"`~-_=+[{]}\\|;:'\",<.>/? "
	);

	boost::random::random_device rng;
	boost::random::uniform_int_distribution<> indexDistribution(0, charset.size() - 1);

	for (auto position = 0; position < passwordLength; position++) {
		std::cout << charset[indexDistribution(rng)];
	}
}
