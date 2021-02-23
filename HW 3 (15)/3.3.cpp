#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>
#include <iomanip>

bool is_equal(double a, double b)
{
	return std::abs(a - b) <= 10 * std::numeric_limits <double> ::epsilon();
}

double random()
{
	double result = std::rand();
	return result += 1.0 / std::rand();
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

class Int_Double
{
private:
	friend struct Int_Double_Hash;
	friend struct Int_Double_Equal;

public:
	Int_Double(const int i, const double d) :
		m_int(i), m_double(d)
	{}

	~Int_Double() noexcept = default;

public:
	friend std::ostream& operator << (std::ostream& stream, const Int_Double& i_d)
	{
		return (stream << i_d.m_int << ", " << i_d.m_double);
	}

private:
	int m_int;
	double m_double;
};

struct Int_Double_Hash
{
	std::size_t operator() (const Int_Double& i_d) const noexcept
	{
		return hash_value(i_d.m_int, i_d.m_double);
	}
};

struct Int_Double_Equal
{
	bool operator() (const Int_Double& lhs, const Int_Double& rhs) const noexcept
	{
		return (lhs.m_int == rhs.m_int) && is_equal(lhs.m_double,rhs.m_double);
	}
};

int main(int argc, char** argv)
{
	std::unordered_set < Int_Double, Int_Double_Hash, Int_Double_Equal > i_ds;
	std::size_t N = 2048U;

	i_ds.rehash(N);

	std::cout << "N = " << N << "\n\n" << std::left << std::setw(15) << std::setfill(' ') << "Elements"
		<< std::setw(15) << std::setfill(' ') << "Collisions" << "\n\n";

	std::size_t collisions = 0U;

	for (auto i = 0U; i < N*i_ds.max_load_factor(); ++i)
	{
		if (i % 10 == 0)
		{
			std::cout << std::left << std::setw(15) << std::setfill(' ') << i
				<< std::setw(15) << std::setfill(' ') << collisions << "\n";
		}
		if (i_ds.bucket_size(i_ds.bucket(*(i_ds.insert(Int_Double(std::rand(), random())).first))) > 1)
		{
			collisions++;
		}
	}

	return EXIT_SUCCESS;
}