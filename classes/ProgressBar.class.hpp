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

#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>

#define PROGRESS_BAR_LABEL_MAX_LEN 25

namespace MusicReader
{
	template <typename T>
	class ProgressBar
	{
	public:
		ProgressBar(T p_MaxVal, const std::string &p_Label): p_MaxVal(p_MaxVal), p_Label(p_Label)
		{
			if (this->p_Label.length() > PROGRESS_BAR_LABEL_MAX_LEN)
			{
				this->p_UseLabelCropping = true;
				this->p_LabelIndex = 0;
			}
		}

		void update(T val)
		{
			std::ostringstream result;

			// ====
			// Gets the terminal size
			// ====

			struct winsize ws{};
			ioctl(0, TIOCGWINSZ, &ws);

			// ====
			// Calculates the update
			// ====

			// Calculates the percentage
			float percentage =  static_cast<float>(val) / static_cast<float>(this->p_MaxVal);

			// Calculates the current substring position
			if (this->p_UseLabelCropping)
			{
				// Checks if the index needs to be resetted
				if (this->p_LabelIndex > this->p_Label.length()) this->p_LabelIndex = 0;

				// Performs the string rotation
				result << this->p_Label.substr(this->p_LabelIndex, this->p_Label.length());
				result << " - " << this->p_Label.substr(0, this->p_LabelIndex);

				// Increments the label index
				this->p_LabelIndex++;
			} else result << this->p_Label;

			// Appends the percentage
			result << " | ";
			result << std::fixed << std::setprecision(2) << percentage << '%';
			result << " | ";

			// Calculates the available chars for the progress bar
			short progressBarCharLen = static_cast<short>(percentage * static_cast<float>(ws.ws_col - result.tellp()));
			progressBarCharLen--; // Removes one for the space for '>'

			// Creates the progress bar
			for (short i = 0; i < percentage * progressBarCharLen; i++)
			{
				result << '=';
			}

			// Adds the '>'
			result << '>';

			// ====
			// Performs the update
			// ====

			std::cout << result.str() << "\r" << std::flush;
		}
	private:
		std::string p_Label;
		std::size_t p_LabelIndex;
		bool p_UseLabelCropping;
		T p_MaxVal;
	};
}