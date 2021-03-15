#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


int main()
{
	std::fstream fin("test.txt", std::ios::in | std::ios::binary);
	std::fstream fout("output 6.1.txt", std::ios::out | std::ios::trunc | std::ios::binary);

	std::istreambuf_iterator < char > istreambuf_iterator(fin);
	std::istreambuf_iterator < char > end;
	std::ostreambuf_iterator < char > ostreambuf_iterator(fout);

	int n = 0;

	char current_symbol = 'a';

	while (istreambuf_iterator != end)
	{
		while ((istreambuf_iterator != end) && (*istreambuf_iterator != '/') && (*istreambuf_iterator != '"'))
		{
			*ostreambuf_iterator++ = *istreambuf_iterator++;
		}
		if ((istreambuf_iterator != end) && (*istreambuf_iterator == '"'))
		{
			char prev_symbol = *istreambuf_iterator;
			*ostreambuf_iterator++ = *istreambuf_iterator++;
			while ((istreambuf_iterator != end) && ((*istreambuf_iterator != '"') || (prev_symbol == '\\')))
			{
				prev_symbol = *istreambuf_iterator;
				*ostreambuf_iterator++ = *istreambuf_iterator++;		
			}
			*ostreambuf_iterator++ = *istreambuf_iterator++;
		}
		if ((istreambuf_iterator != end) && (*istreambuf_iterator == '/'))
		{
			++istreambuf_iterator;
			if ((istreambuf_iterator != end) && (*istreambuf_iterator == '*'))
			{
				++istreambuf_iterator;
				for (; (istreambuf_iterator != end) && (*istreambuf_iterator != '*'); ++istreambuf_iterator);
				for (auto i = 0; (i < 2) && (istreambuf_iterator != end); ++i, ++istreambuf_iterator);
			}
			else
			{
				for (; (istreambuf_iterator != end) && (*istreambuf_iterator != '\n') && (*istreambuf_iterator != '\r'); ++istreambuf_iterator);
				if (istreambuf_iterator != end)
				{
					*ostreambuf_iterator++ = *istreambuf_iterator++;
				}
			}
		}
	}

	fin.close();
	fout.close();

	return 0;
}