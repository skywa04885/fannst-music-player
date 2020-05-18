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

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdexcept>
#include <alsa/asoundlib.h>
#include <string>
#include <atomic>
#include <iostream>
#include <malloc.h>
#include <random>

namespace MusicReader
{
	class AudioHandler
	{
	public:
		AudioHandler(const uint32_t &sampleRate, const uint16_t &numChannels);
		~AudioHandler();

		void play(std::atomic<bool> &shouldRun, void (*dataCallback)(uint8_t *, uint32_t, void *), void *ud);
	private:
		snd_pcm_t *a_Handle;
		snd_pcm_hw_params_t *a_Params;
		snd_pcm_uframes_t a_Frames;
		uint32_t a_SampleRate;
		uint16_t a_NumChannels;
		int a_Dir;
	};
}