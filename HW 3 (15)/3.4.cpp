#include <functional>
#include <iostream>
#include <string>
#include <set>
#include <iomanip>
#include <random>
#include <chrono>
#include <vector>
#include <map>

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

unsigned int RSHash(const char* str, unsigned int length)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = hash * a + (*str);
		a = a * b;
	}

	return hash;
}

unsigned int JSHash(const char* str, unsigned int length)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash ^= ((hash << 5) + (*str) + (hash >> 2));
	}

	return hash;
}

unsigned int PJWHash(const char* str, unsigned int length)
{
	const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	const unsigned int ThreeQuarters = (unsigned int)((BitsInUnsignedInt * 3) / 4);
	const unsigned int OneEighth = (unsigned int)(BitsInUnsignedInt / 8);
	const unsigned int HighBits =
		(unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash << OneEighth) + (*str);

		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return hash;
}

unsigned int ELFHash(const char* str, unsigned int length)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash << 4) + (*str);

		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
		}

		hash &= ~x;
	}

	return hash;
}

unsigned int BKDRHash(const char* str, unsigned int length)
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash * seed) + (*str);
	}

	return hash;
}

unsigned int SDBMHash(const char* str, unsigned int length)
{
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (*str) + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

unsigned int DJBHash(const char* str, unsigned int length)
{
	unsigned int hash = 5381;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = ((hash << 5) + hash) + (*str);
	}

	return hash;
}

unsigned int DEKHash(const char* str, unsigned int length)
{
	unsigned int hash = length;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
	}

	return hash;
}

unsigned int APHash(const char* str, unsigned int length)
{
	unsigned int hash = 0xAAAAAAAA;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) :
			(~((hash << 11) + ((*str) ^ (hash >> 5))));
	}

	return hash;
}

struct Hash_details
{
	Hash_details() = default;
	Hash_details(std::function<unsigned int(const char*, unsigned int)>func) : m_func(func) {}

	std::size_t m_collisions = 0U;
	std::set < unsigned int > m_set;
	std::function<unsigned int(const char*, unsigned int)> m_func;
};

enum class Functions
{
	RSHash,
	JSHash,
	PJWHash,
	ELFHash,
	BKDRHash,
	SDBMHash,
	DJBHash,
	DEKHash,
	APHash
};

int main()
{
	const std::size_t N = 1'000'000U;

	std::cout << std::left << std::setw(11) << std::setfill(' ') << "Elements"
		<< std::setw(8) << std::setfill(' ') << "RSCol"
		<< std::setw(8) << std::setfill(' ') << "JSCol" 
		<< std::setw(8) << std::setfill(' ') << "PJWCol"
		<< std::setw(8) << std::setfill(' ') << "ELFCol"
		<< std::setw(8) << std::setfill(' ') << "BKDRCol"
		<< std::setw(8) << std::setfill(' ') << "SDBMCol"
		<< std::setw(8) << std::setfill(' ') << "DJBCol"
		<< std::setw(8) << std::setfill(' ') << "DEKCol"
		<< std::setw(8) << std::setfill(' ') << "APCol"
		<< "\n\n";

	auto strings = make_random_words(N, 10U);

	std::map<Functions, Hash_details> hash_details;
	hash_details[Functions::RSHash].m_func = RSHash;
	hash_details[Functions::JSHash].m_func = JSHash;
	hash_details[Functions::PJWHash].m_func = PJWHash;
	hash_details[Functions::ELFHash].m_func = ELFHash;
	hash_details[Functions::BKDRHash].m_func = BKDRHash;
	hash_details[Functions::SDBMHash].m_func = SDBMHash;
	hash_details[Functions::DJBHash].m_func = DJBHash;
	hash_details[Functions::DEKHash].m_func = DEKHash;
	hash_details[Functions::APHash].m_func = APHash;

	auto it = strings.begin();

	for (std::size_t i = 0U; i < N; ++i, ++it)
	{
		if (i % 1000 == 0)
		{
			std::cout << std::left << std::setw(11) << std::setfill(' ') << i;
			for (auto j = 0U; j < hash_details.size(); ++j)
			{
				std::cout << std::setw(8) << std::setfill(' ') << hash_details[static_cast<Functions>(j)].m_collisions;
			}
			std::cout << '\n';
		}
		for (auto j = 0U; j < hash_details.size(); ++j)
		{
			if (!(hash_details[static_cast<Functions>(j)].m_set.insert
			(hash_details[static_cast<Functions>(j)].m_func(it->c_str(), it->length())).second))
			{
				hash_details[static_cast<Functions>(j)].m_collisions++;
			}
		}
	}

	return 0;
}