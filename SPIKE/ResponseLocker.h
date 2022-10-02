#pragma once
#include<optional>
#include<sstream>
#include"HttpException.h"
class ResponseLocker
{
private:
	std::optional<std::source_location> locked;
public:
	void Lock(std::source_location location)
	{
		if (locked)
		{
			std::stringstream stream;
			stream << "Response already locked at \nfile:" << std::filesystem::path(locked->file_name()).filename()
				<< "line:" << locked->line();
			throw HttpException{ stream.str() , location};
		}
		locked = location;
	}
};