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

#include "MSWav.class.hpp"

namespace MusicReader::Files
{
  void printMSWavFileMetaData(const std::size_t &i, const MSWavFileMetaData &md, Logger &logger)
  {
  	logger.append(i).append(": Start MSWav Meta: ").print();

		logger.append("- Chunk ID: ").append(std::string(md.m_ChunkID, 4)).print();
		logger.append("- Size: ").append(md.m_Size).print();

		if (md.m_Data != nullptr)
		{
			logger.append("- Data: ").append(std::string(md.m_Data, md.m_Size)).print();
		}

		logger.append("End MSWav Meta").print();
  }

	MSWav::MSWav(const char *m_File): m_File(m_File)
	{
		this->m_FileStream.open(this->m_File);

		if (!this->m_FileStream.is_open())
			throw std::runtime_error(
				std::string("Could not open file, no suck file or directory: ") + 
				this->m_File);
	}

	MSWav::~MSWav()
	{
		this->m_FileStream.close();
	}

	uint32_t MSWav::getPlayingTime()
	{
		return this->m_PlayingTime;
	}

	MSWav &MSWav::readHeaders()
	{
		Logger logger("ReadHeadersDebug", LoggerLevel::L_DEBUG);

		// ====
		// Verifies the file type, if not throw runtime_error ;)
		// ====

		char buffer[5];
		buffer[4] = '\0';

		this->m_FileStream.read(buffer, 4);
		if (memcmp(buffer, "RIFF", 4) != 0)
			throw std::runtime_error(
				std::string("Specified file is not of type RIFF: ") + buffer);

		// ====
		// Reads the headers
		// ====

		this->m_FileStream.seekg(0, this->m_FileStream.beg);
		this->m_FileStream.read(reinterpret_cast<char *>(&this->m_Headers), sizeof (m_Headers));

		// Calculates some stuff, hey ladies ;) ...
		this->m_FrameSize = (static_cast<uint32_t>(this->m_Headers.h_BitsPerSample) / 8) * this->m_Headers.h_NumChannels;

		// ====
		// Reads the METADATA
		// ====

		// Checks if we need to proceed
		if (memcmp(this->m_Headers.h_SubChunk2ID, "LIST", 4) != 0) return *this;

		// Note: subtracting sizeof (char *) in order to remove the memory address of the data, and size ( due to list ), and cause issues
		MSWavFileMetaData initialChunk{};
		this->m_FileStream.read(reinterpret_cast<char *>(&initialChunk), sizeof (MSWavFileMetaData) - sizeof (char *) - sizeof (uint32_t));
		printMSWavFileMetaData(0, initialChunk, logger);

		// Calculates the len
		this->m_PlayingTime = this->calculateLengthSeconds();

		// Checks if it is info
		if (memcmp(initialChunk.m_ChunkID, "INFO", 4) != 0)
		{
			// Returns
			return *this;
		}

		return *this;

		// Reads the info data
		initialChunk.m_Data = reinterpret_cast<char *>(malloc(sizeof (char) * this->m_Headers.h_SubChunk2Size));
		
		// Starts looping over the data
		std::size_t totalSize = this->m_Headers.h_SubChunk2Size - 4;
		while (totalSize > 0)
		{
			// Creates the meta data
			MSWavFileMetaData c{};

			// Reads the METADATA without the data itself
			this->m_FileStream.read(reinterpret_cast<char *>(&c), sizeof (MSWavFileMetaData) - sizeof (char *));
			// Allocates the memory for the data itself based on METADATA header
			c.m_Data = reinterpret_cast<char *>(malloc(sizeof (char) * c.m_Size));
			// Reads the METADATA data
			this->m_FileStream.read(reinterpret_cast<char *>(c.m_Data), c.m_Size);
			std::cout << c.m_Size << std::endl;

			// Checks if it is data
			if (memcmp(c.m_ChunkID, "data", 4) == 0)
			{
				// Stops from looping
				break;
			}

			printMSWavFileMetaData(1, c, logger);

			// Removes the current size in bytes from total available
			totalSize -= static_cast<std::size_t>(c.m_Size);
		}

		return *this;
	}

  void MSWav::gotoDataSegmentStart()
  {
  	this->m_FileStream.seekg(sizeof (MSWavFileHeader) + 200);
  }

	void MSWav::audioHandlerCallback(uint8_t *buff, uint32_t frameCount, void *ud)
	{
		reinterpret_cast<MSWav *>(ud)->readChunks(buff, frameCount);
	}

	void MSWav::readChunks(uint8_t *buff, uint32_t frameCount)
	{
		this->m_FileStream.read(reinterpret_cast<char *>(buff), frameCount * this->m_FrameSize);
	}

	MSWav &MSWav::printHeaders(Logger &logger)
	{
		logger.append("Start MSWav Header: ").print();

		logger.append("- Magic: ").append(std::string(this->m_Headers.h_Magic, 4)).print();
		logger.append("- Chunk size: ").append(this->m_Headers.h_ChunkSize).print();
		logger.append("- Format: ").append(std::string(this->m_Headers.h_Format, 4)).print();
		logger.append("- SubChunk 1 ID: ").append(std::string(this->m_Headers.h_SubChunk1ID, 4)).print();
		logger.append("- SubChunk 1 Size: ").append(this->m_Headers.h_SubChunk1Size).print();
		logger.append("- Audio Format: ").append(this->m_Headers.h_AudioFormat).print();
		logger.append("- Sample Rate: ").append(this->m_Headers.h_SampleRate).print();
		logger.append("- Byte Rate: ").append(this->m_Headers.h_ByteRate).print();
		logger.append("- Block Align: ").append(this->m_Headers.h_BlockAlign).print();
		logger.append("- Bits per sample: ").append(this->m_Headers.h_BitsPerSample).print();
		logger.append("- SubChunk 2 ID: ").append(std::string(this->m_Headers.h_SubChunk2ID, 4)).print();
		logger.append("- SubChunk 2 Size: ").append(this->m_Headers.h_SubChunk2Size).print();

		logger.append("End MSWav Header").print();

		return *this;
	}

	/**
	 * Calculates the file length
	 */
	uint32_t MSWav::calculateLengthSeconds()
	{
		// Stores the old pos
		std::size_t old = this->m_FileStream.tellg();

		// Goes to the file end
		this->m_FileStream.seekg(0, this->m_FileStream.end);
		// Calculates the len
		uint32_t len = m_FileStream.tellg() / this->m_Headers.h_ByteRate;
		
		// Restores the original value
		this->m_FileStream.seekg(old, this->m_FileStream.beg);

		return len;
	}
}