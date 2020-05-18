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

#pragma once

#include <stdint.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace MusicReader
{
  typedef enum : uint8_t
  {
    L_DEBUG = 0,
    L_INFO,
    L_WARNING,
    L_ERROR,
    L_FATAL
  } LoggerLevel;

  /**
   * Instance for MPEGReader Timer instance
   */
  class Timer
  {
  public:
    Timer(const char *t_TimerPrefix);
    ~Timer();
  private:
    const char *t_TimerPrefix;
    const uint64_t t_StartTime;
  };

  /**
   * Instance for MPEGReader Logger instance
   */
  class Logger
  {
  public:
    Logger(const char *l_Prefix, const LoggerLevel &l_Level);
    ~Logger();

    template <typename T>
    Logger &append(T val)
    {
      this->l_Buffer << val;
      return *this;
    }

    Logger &print();
  private:
    const char *l_Prefix;
    LoggerLevel l_Level;
    std::ostringstream l_Buffer;
  };
}
