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

int main(const int argc, const char **argv)
{
  Logger console("Main", LoggerLevel::L_INFO);

  // Prints the initial message
  console.append("By Luke A.C.A. Rieff @ Fannst Software").print();

  // ====
  // Plays the file
  // ====

  std::atomic<bool> play(true);

  // Creates the file and reads the headers
  Files::MSWav file("../audio/wav/faded.wav");
  file.readHeaders().printHeaders(console);

  // Creates the audio handler
  AudioHandler ah(file.m_Headers.h_SampleRate, file.m_Headers.h_NumChannels);

  // Goes to the data segment
  file.gotoDataSegmentStart();

  // Starts playing
  ah.play(play, &file.audioHandlerCallback, &file, file.getPlayingTime());

  // Prints the final message
  console.append("Quitting").print();

  return 0;
}
