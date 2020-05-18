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

#include "main.hpp"

using namespace MusicReader;

void cb(uint8_t *buf, uint32_t frameCount)
{
  for (int i = 0; i < frameCount; i++)
  {
    buf[i] = rand() % 10 * 100;
  }
}

int main(const int argc, const char **argv)
{
  Logger console("Main", LoggerLevel::L_INFO);

  // Prints the initial message
  console
    .append<const char *>("Main started ...")
    .print();

  // Run code here
  Files::MSWav file("../audio/wav/faded.wav");
  file.readHeaders().printHeaders(console);

  std::atomic<bool> play(true);

  AudioHandler ah(file.m_Headers.h_SampleRate, file.m_Headers.h_NumChannels);
  file.gotoDataSegmentStart();
  ah.play(play, &file.audioHandlerCallback, &file);

  // Prints the final message
  console
    .append<const char *>("Main ended ...")
    .print();

  return 0;
}
