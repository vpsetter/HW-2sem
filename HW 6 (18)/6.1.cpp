#include <iostream>
#include <fstream>
#include <sstream>


int main()
{
	std::fstream fin("test.txt", std::ios::in | std::ios::binary);
	std::fstream fout("output 6.1.txt", std::ios::out | std::ios::trunc | std::ios::binary);

	std::istreambuf_iterator < char > istreambuf_iterator(fin);
	std::istreambuf_iterator < char > end;
	std::ostreambuf_iterator < char > ostreambuf_iterator(fout);

	while (istreambuf_iterator != end)
	{
		for (; (istreambuf_iterator != end) && (*istreambuf_iterator != '/') && (*istreambuf_iterator != '"');
			*ostreambuf_iterator++ = *istreambuf_iterator++);

		if ((istreambuf_iterator != end) && (*istreambuf_iterator == '"'))
		{
			auto prev_symbol = *istreambuf_iterator;
			*ostreambuf_iterator++ = *istreambuf_iterator++;

			for (; (istreambuf_iterator != end) && ((*istreambuf_iterator != '"') || (prev_symbol == '\\'));
				prev_symbol = *istreambuf_iterator, *ostreambuf_iterator++ = *istreambuf_iterator++);

			*ostreambuf_iterator++ = *istreambuf_iterator++;
		}

		if ((istreambuf_iterator != end) && (*istreambuf_iterator == '/'))
		{
			auto symbol = *istreambuf_iterator++;
			if (istreambuf_iterator != end)
			{
				switch (*istreambuf_iterator)
				{
				case '*':
					++istreambuf_iterator;
					for (; (istreambuf_iterator != end) && (*istreambuf_iterator != '*'); ++istreambuf_iterator);
					for (auto i = 0; (i < 2) && (istreambuf_iterator != end); ++i, ++istreambuf_iterator);
					break;
				case '/':
					for (; (istreambuf_iterator != end) && (*istreambuf_iterator != '\n') && (*istreambuf_iterator != '\r'); ++istreambuf_iterator);
					if (istreambuf_iterator != end)
					{
						*ostreambuf_iterator++ = *istreambuf_iterator++;
					}
					break;
				default:
					*ostreambuf_iterator++ = symbol;
					break;
				}
			}
		}
	}

	return 0;
}