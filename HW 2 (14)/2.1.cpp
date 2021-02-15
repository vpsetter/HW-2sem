#include<vector>
#include<iostream>


int main()
{
	std::vector<int> v1;
	std::cout << "number of elements after creating a vector by default: "
		<< v1.capacity() << '\n'; // 0
	v1.push_back(0);
	std::cout << "number of elements after addition an elemenemt to an empty vector: " 
		<< v1.capacity() << '\n'; // 1

	std::vector<int> v2({ 0 });
	std::cout << "number of elements after creating a vector from a singleton array: " 
		<< v2.capacity() << '\n'; // 1
	v2.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a singleton vector: " 
		<< v2.capacity() << '\n'; // 2

	std::vector<int> v3({ 0, 0 });
	std::cout << "number of elements after creating a vector from a two-element array: " 
		<< v3.capacity() << '\n'; // 2
	v3.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a two-element vector: " 
		<< v3.capacity() << '\n'; // 3
	v3.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a three-element vector: " 
		<< v3.capacity() << '\n'; // 4 = the integer part of 1.5*3
	v3.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a four-element vector: " 
		<< v3.capacity() << '\n'; // 6 = 1.5*4
	

	std::vector<int> v4({ 0,0,0,0,0,0 /* 6 */ });
	std::cout << "number of elements after creating a vector from a six-element array: " 
		<< v4.capacity() << '\n'; // 6

	// initial size of vector is the smallest possible

	v4.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a six-element vector: " 
		<< v4.capacity() << '\n'; // 9 = 1.5*6

	std::vector<int> v5(9, 0);
	std::cout << "number of elements after creating a vector from a size of array = 9 and a common initial value: " 
		<< v5.capacity() << '\n'; // 9
	v5.push_back(0);
	std::cout << "number of elements after addition an elemenemt to a nine-element vector: " 
		<< v5.capacity() << '\n'; // 13 = the integer part of 1.5*9

	v5.reserve(100);
	std::cout << "number of elements after v5.reserve(100): " 
		<< v5.capacity() << '\n'; // 100
	v5.reserve(50);
	std::cout << "number of elements after v5.reserve(50) (50 < v5.capacity() = 100): "
		<< v5.capacity() << '\n'; // 100

	auto required_size = v5.capacity();

	while(required_size == v5.capacity())
	{
		required_size *= 2;
		try
		{
			v5.reserve(required_size);
		}
		catch(...){}
	}

	std::cout << "size of v5 before resizing by v5.reserve(): "
		<< required_size/2 << '\n';
	std::cout << "required_size: "
		<< required_size << '\n';
	std::cout << "size of v5 after unsuccessful attempt of v5.reserve(): " 
		<< v5.capacity() << '\n'; // size of v5 wasn't changed

	while (true)
	{
		try 
		{
			v5.push_back(0);
		}
		catch (...) { break; }
	}
	
	std::cout << "final size of v5 if we add elements by v5.push_back(): " << v5.capacity() << '\n'; // or x1.5, or nothing

	return 0;
}
