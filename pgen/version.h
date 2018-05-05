#ifndef PGEN_VERSION_H_
#define PGEN_VERSION_H_

#include <sstream>
#include <string>

struct Version {
	static constexpr auto major = 0;
	static constexpr auto minor = 0;
	static constexpr auto build = 3;

	static std::string value() noexcept {
		std::stringstream oss;

		oss << major << "." << minor << "." << build;

		return oss.str();
	}
};

inline void PrintVersion() {
	std::cout << "\n";
	std::cout << "          PGen - " << "Version: " << Version::value() << "\n";
	std::cout << "        Jose Fernando Lopez Fernandez" << "\n\n";
}

#endif // PGEN_VERSION_H_
