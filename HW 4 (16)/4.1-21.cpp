#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <limits>
#include <array>

constexpr int integer_sqrt(int n)
{
	int left = 1, right = n - 1;
	int middle = left + ((right - left) / 2);
	while (left != right)
	{
		middle = left + ((right - left) / 2);
		n / middle < middle ? right = middle : left = middle + 1;
	}
	return right;
}

template< int N >
constexpr bool is_prime(int n, const std::array <int, N>& arr, int elem_num)
{
	for (auto i = 0; (i < elem_num) && arr[i] < integer_sqrt(n); ++i)
	{
		if (n % arr[i] == 0)
		{
			return false;
		}
	}
	return true;
}

template< int N > //N least primes
constexpr std::array<int, N> primes()
{
	std::array<int, N> primes{};
	primes[0] = 2;
	for (auto current_number = 3, index = 1; index < N; ++index, current_number += 2)
	{
		for (; !(is_prime(current_number, primes, index)); current_number += 2);
		primes[index] = current_number;
	}
	return primes;
}

int main()
{

	std::vector<int> v1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }; //#1

	std::copy(std::istream_iterator < int >(std::cin), std::istream_iterator < int >(),	std::back_inserter(v1)); //#2

	std::default_random_engine dre(std::chrono::system_clock().now().time_since_epoch().count());

	std::shuffle(std::begin(v1), std::end(v1), dre); //#3

	std::sort(std::begin(v1), std::end(v1));
	v1.erase(std::unique(std::begin(v1), std::end(v1)), v1.end()); //#4

	auto odds = std::count_if(std::begin(v1), std::end(v1), [](auto x) {return x % 2; }); //#5

	std::cout << "odds: " << odds << '\n';

	auto [min, max] = std::minmax_element(v1.begin(), v1.end()); //#6

	std::cout << "min: " << *min << ", max: " << *max << '\n';

	auto primes10000 = primes<10'000>();
	auto it = std::find_if(std::begin(v1), std::end(v1), 
		[&primes10000](auto x) {return std::find(std::begin(primes10000), std::end(primes10000), x) != std::end(primes10000); });
	if (it != std::end(v1))
	{
		std::cout << "prime: " << *it << '\n';
	}//#7

	std::for_each(std::begin(v1), std::end(v1), [](auto& x) {x = x * x; }); //#8

	std::uniform_int_distribution <> uid(0, 1000);
	const std::size_t N = v1.size();
	std::vector<int> v2(N, 0);

	std::generate_n(std::begin(v2), N, [&dre, &uid]() {return uid(dre); }); //#9

	auto sum = std::accumulate(std::begin(v2), std::end(v2), 0); //#10

	std::fill_n(std::begin(v2), 4, 1); //#11

	std::vector<int> v3(N, 0);

	std::transform(std::begin(v1), std::end(v1), std::begin(v2), std::begin(v3), std::minus()); //#12

	std::replace_if(std::begin(v3), std::end(v3), [](auto x) {return x < 0; }, 0); //#13

	v3.erase(std::remove_if(std::begin(v3), std::end(v3), [](auto x) {return x == 0; }), std::end(v3)); //#14

	std::reverse(std::begin(v3), std::end(v3)); //#15
	
	
	const std::size_t n = v3.size();
	if (n > 2)
	{
		std::vector<int> vtop(3U, 0);
		std::nth_element(std::begin(v3), std::next(std::begin(v3), n - 3), std::end(v3));
		std::copy(std::next(std::begin(v3), n - 3), std::end(v3), std::begin(vtop));
		if (vtop[1] > vtop[2])
		{
			std::swap(vtop[1], vtop[2]);
		}
		std::cout << "top 3: " << vtop[2] << ' ' << vtop[1] << ' ' << vtop[0] << '\n';
	} //#16

	std::sort(std::begin(v1), std::end(v1));
	std::sort(std::begin(v2), std::end(v2)); //#17

	std::vector <int> v4 = v1;
	std::copy(std::begin(v3), std::end(v3), std::back_inserter(v4)); //#18

	std::sort(std::begin(v4), std::end(v4));
	auto range = std::equal_range(std::begin(v4), std::end(v4), 1); //#19

	std::cout << "v1: ";
	std::copy(v1.cbegin(), v1.cend(), std::ostream_iterator < int >(std::cout, " "));
	std::cout << '\n';

	std::cout << "v2: ";
	std::copy(v2.cbegin(), v2.cend(), std::ostream_iterator < int >(std::cout, " "));
	std::cout << '\n';

	std::cout << "v3: ";
	std::copy(v3.cbegin(), v3.cend(), std::ostream_iterator < int >(std::cout, " "));
	std::cout << '\n';

	std::cout << "v4: ";
	std::copy(v4.cbegin(), v4.cend(), std::ostream_iterator < int >(std::cout, " "));
	std::cout << '\n'; //#20

	const std::size_t size = 1'000'000U;
	std::vector <int> v5(size, 0);

	for (auto i = 0; i < N; ++i)
	{
		v5[i] = i;
	}

	std::vector <int> v6 = v5;

	auto sort_comp = 0U, nth_comp = 0U;
	std::sort(std::begin(v5), std::end(v5), [&sort_comp](int a, int b) {sort_comp++; return a > b; });
	std::nth_element(std::begin(v6), std::next(std::begin(v6), 500'000), std::end(v6), [&nth_comp](int a, int b) {nth_comp++; return a > b; });

	std::cout << "sort: " << sort_comp << ", nth: " << nth_comp << '\n';	

	return 0;
}