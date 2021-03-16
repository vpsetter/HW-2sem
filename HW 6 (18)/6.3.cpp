#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T>
std::time_t to_time_t(T time_point)
{
	auto system_clock_time_point = std::chrono::time_point_cast<std::chrono::system_clock::duration>(time_point - T::clock::now()
		+ std::chrono::system_clock::now());
	return std::chrono::system_clock::to_time_t(system_clock_time_point);
}

void view_directory(const std::filesystem::path& path)
{
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
		std::cout << '\n' << std::left << std::setw(30) << std::setfill(' ') << "File name"
			<< std::setw(10) << std::setfill(' ') << "Extension" << "Last change time\n";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			auto file_name = entry.path().filename().string();
			auto extension = entry.path().extension().string();
			auto time_point = to_time_t(std::filesystem::last_write_time(path));
			auto last_change_time = std::asctime(std::localtime(&time_point));

			std::cout << std::left << std::setw(30) << std::setfill(' ') << file_name 
				<< std::left << std::setw(10) << std::setfill(' ') << extension
				<< last_change_time;
		}
	}
	else
	{
		std::cout << "Invalid directory\n";
	}
}

int main()
{
	system("chcp 1251");

	std::filesystem::path path;

	std::cin >> path;
	view_directory(path);

	return 0;
}