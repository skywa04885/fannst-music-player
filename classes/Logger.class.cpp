/*
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Logger.class.hpp"

namespace MusicReader
{
	namespace LoggerColor
	{
	  const char *red = "\033[31m";
	  const char *yellow = "\033[33m";
	  const char *blue = "\033[34m";
	  const char *pink = "\033[35m";
	  const char *def = "\033[0m";
		const char *green = "\033[32m";
	}

	Logger::Logger(const char *l_Prefix, const LoggerLevel &l_Level): l_Prefix(l_Prefix), l_Level(l_Level)
	{}

	Logger::~Logger()
	{}

	Logger &Logger::print()
	{
		std::ostringstream result;

		// Appends the level
		switch (this->l_Level)
		{
			case LoggerLevel::L_DEBUG:
			{
				result << LoggerColor::green << "[DEBUG]: " << LoggerColor::def;
				break;
			}
			case LoggerLevel::L_INFO:
			{
				result << LoggerColor::pink << "[INFO]: " << LoggerColor::def;
				break;
			}
			case LoggerLevel::L_WARNING:
			{
				result << LoggerColor::yellow << "[WARN]: " << LoggerColor::def;
				break;
			}
			case LoggerLevel::L_ERROR:
			{
				result << LoggerColor::red << "[ERROR]: " << LoggerColor::def;
				break;
			}
			case LoggerLevel::L_FATAL:
			{
				result << LoggerColor::red << "[FATAL]: " << LoggerColor::def;
				break;
			}
		}

		result << this->l_Buffer.str();
		std::cout << result.str() << std::endl;
		
		this->l_Buffer.str("");
		this->l_Buffer.clear();
		return *this;
	}
}