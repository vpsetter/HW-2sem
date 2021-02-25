#include <functional>
#include <iostream>
#include <string>
#include <set>
#include <iomanip>
#include <random>
#include <chrono>
#include <vector>

std::set < std::string > make_random_words(std::size_t N, std::size_t length)
{
	std::uniform_int_distribution <> letter(97, 122);
	std::default_random_engine e(static_cast <std::size_t> 
		(std::chrono::system_clock::now().time_since_epoch().count()));

	std::set < std::string > words;

	for (std::string s(length, '_'); std::size(words) < N; words.insert(s))
	{
		for (auto& c : s)
		{
			c = letter(e);
		}
	}

	return words;
}

template < typename T >
void hash_combine(std::size_t& seed, const T& value) noexcept
{
	seed ^= std::hash < T >()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template < typename T >
void hash_value(std::size_t& seed, const T& value) noexcept
{
	hash_combine(seed, value);
}

template < typename T, typename ... Types >
void hash_value(std::size_t& seed, const T& value, const Types& ... args) noexcept
{
	hash_combine(seed, value);
	hash_value(seed, args...);
}

template < typename ... Types >
std::size_t hash_value(const Types& ... args) noexcept
{
	std::size_t seed = 0;
	hash_value(seed, args...);
	return seed;
}

int main(int argc, char** argv)
{
	const std::size_t N = 1'000'000U;

	std::cout << "N = " << N << "\n\n" << std::left << std::setw(15) << std::setfill(' ') << "Elements"
		<< std::setw(15) << std::setfill(' ') << "Collisions" << "\n\n";

	std::vector<int> ints(N, 0);
	std::vector<double> doubles(N, 0.0);

	for (std::size_t i = 0U; i < N; ++i)
	{
		ints[i] = i;
		doubles[i] = static_cast<double>(i) / static_cast<double>(N);
	}

	std::shuffle(ints.begin(), ints.end(), std::default_random_engine(static_cast <std::size_t>(std::chrono::system_clock::now().time_since_epoch().count())));
	std::shuffle(ints.begin(), ints.end(), std::default_random_engine(static_cast <std::size_t>(std::chrono::system_clock::now().time_since_epoch().count())));
	auto strings = make_random_words(N, 10U);

	std::set < std::size_t > hashes;

	auto it = strings.begin();
	auto collisions = 0U;

	for (std::size_t i = 0U; i < N; ++i)
	{
		if (i % 1000 == 0)
		{
			std::cout << std::left << std::setw(15) << std::setfill(' ') << i
				<< std::setw(15) << std::setfill(' ') << collisions << "\n";
		}
		if (!(hashes.insert(hash_value(*it++, ints[i], doubles[i])).second))
		{
			collisions++;
		}
	}

	return 0;
}