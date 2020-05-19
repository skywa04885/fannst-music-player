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

#include "AudioHandler.class.hpp"

namespace MusicReader
{
	AudioHandler::AudioHandler(const uint32_t &a_SampleRate, 
		const uint16_t &a_NumChannels): a_SampleRate(a_SampleRate), a_NumChannels(a_NumChannels)
	{
		int rc;

		// ====
		// Initializes alsa
		// ====

		// Opens an playback stream
		rc = snd_pcm_open(&this->a_Handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

		// Checks if the stream was openend successfully
		if (rc < 0)
			throw std::runtime_error(std::string("Could not open playback stream ;(") + snd_strerror(rc));

		// ====
		// Configures Alsa
		// ====

		// Allocates memory for the parameters
		snd_pcm_hw_params_alloca(&this->a_Params);
		// Gets the default ALSA values
		snd_pcm_hw_params_any(this->a_Handle, this->a_Params);

		// Sets interleaved mode
		snd_pcm_hw_params_set_access(this->a_Handle, this->a_Params, SND_PCM_ACCESS_RW_INTERLEAVED);
		// Sets the format we're using
		switch (this->a_NumChannels)
		{
			case 2:
			{
				snd_pcm_hw_params_set_format(this->a_Handle, this->a_Params, SND_PCM_FORMAT_S16_LE);
				break;
			}
			default: throw std::runtime_error(std::string("Num channels not supported: ") + std::to_string(static_cast<int>(this->a_NumChannels)));
		}
		// Sets the num channels
		snd_pcm_hw_params_set_channels(this->a_Handle, this->a_Params, this->a_NumChannels);
		// Sets the sample rate
		snd_pcm_hw_params_set_rate_near(this->a_Handle, this->a_Params, &this->a_SampleRate, &this->a_Dir);
		// Sets the period size to 32 frames
		this->a_Frames = 128;
		snd_pcm_hw_params_set_period_size_near(this->a_Handle, this->a_Params, &this->a_Frames, &this->a_Dir);

		// Writes the parameters to the driver
		rc = snd_pcm_hw_params(this->a_Handle, this->a_Params);
		if (rc < 0)
			throw std::runtime_error(std::string("Could not write parameters to driver: ") + snd_strerror(rc));
	}

	void AudioHandler::play(std::atomic<bool> &shouldRun, 
		void (*dataCallback)(uint8_t *, uint32_t, void *), void *ud, const uint32_t& playLenS)
	{
		int rc;
		uint32_t periodTime, bufferSize, frameCount;
		std::size_t loops;
		short shouldUpdate = 0;
		uint8_t *buffer;

		// ====
		// Prepares to play
		// ====

		// Gets the period size
		snd_pcm_hw_params_get_period_size(this->a_Params, &this->a_Frames, &this->a_Dir);
		// Gets the buffer size
		// TODO: Make 4 adjustable based on the file
		frameCount = static_cast<uint32_t>(this->a_Frames);
		bufferSize = frameCount * 4;
		// Allocates memory for the buffer
		buffer = reinterpret_cast<uint8_t *>(malloc(bufferSize));

		// Gets the period time
		snd_pcm_hw_params_get_period_time(this->a_Params, &periodTime, &this->a_Dir);
		// Calculates the amount of loops
		loops = (static_cast<std::size_t>(playLenS) * 1000000) / periodTime;

		// Creates the progress bar 
		ProgressBar<std::size_t> progressBar(loops, "Hello World This Is an Test");

		// ====
		// Starts playing
		// ====

		// Starts the looping
		for (std::size_t i = 0; i < loops; i++)
		{
			// Increments should update
			if (shouldUpdate >= 200)
			{
				shouldUpdate = 0;
				
				// Updates the progress bar
				progressBar.update(i);
			} else shouldUpdate++;

			// Requests the data to the buffer
			dataCallback(buffer, frameCount, ud);

			// Writes to the output
			rc = snd_pcm_writei(this->a_Handle, buffer, this->a_Frames);
			// Checks if write was successful
			if (rc == -EPIPE)
			{
				// Prints the message
				std::cerr << "Underrun occured ! ;(" << std::endl;
				// Prepares the buffer
				snd_pcm_prepare(this->a_Handle);
			} else if (rc < 0)
			{
				// Free's the memory
				free(buffer);

				// Throws the error
				throw std::runtime_error(std::string("Error occured: ") + snd_strerror(rc));
			} else if (rc != static_cast<uint8_t>(this->a_Frames))
			{
				// Prints the error message
				std::cerr << "Short write occured ;(" << std::endl;
			}
		}

		// Free's the buffer
		free(buffer);
	}

	AudioHandler::~AudioHandler()
	{
		snd_pcm_drain(this->a_Handle);
		snd_pcm_close(this->a_Handle);
	}
}