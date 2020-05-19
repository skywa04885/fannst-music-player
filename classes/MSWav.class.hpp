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

#include <fstream>
#include <string.h>
#include <malloc.h>
#include <string>
#include <iostream>
#include <vector>

#include "Logger.class.hpp"

namespace MusicReader::Files
{
  typedef struct __attribute__ ((packed))
  {
    char m_ChunkID[4];
    uint32_t m_Size;
    char *m_Data{nullptr};
  } MSWavFileMetaData;

  void printMSWavFileMetaData(const std::size_t &i, const MSWavFileMetaData &md, Logger &logger);

  typedef struct __attribute__ ((packed))
  {
    char h_Magic[4];
    uint32_t h_ChunkSize;
    char h_Format[4];
    char h_SubChunk1ID[4];
    uint32_t h_SubChunk1Size;
    uint16_t h_AudioFormat;
    uint16_t h_NumChannels;
    uint32_t h_SampleRate;
    uint32_t h_ByteRate;
    uint16_t h_BlockAlign;
    uint16_t h_BitsPerSample;
    char h_SubChunk2ID[4];
    uint32_t h_SubChunk2Size;
  } MSWavFileHeader;

  class MSWav
  {
  public:
    MSWav(const char *m_File);
    ~MSWav();

    MSWav &readHeaders();
    MSWav &printHeaders(Logger &logger);

    static void audioHandlerCallback(uint8_t *buff, uint32_t frameCount, void *ud);
    void readChunks(uint8_t *buff, uint32_t frameCount);
    void gotoDataSegmentStart();
    uint32_t calculateLengthSeconds();

    uint32_t getPlayingTime();

    MSWavFileHeader m_Headers;
    std::vector<MSWavFileMetaData> m_MetaData;
  private:
    const char *m_File;
    std::ifstream m_FileStream;
    std::size_t m_ReadChunkPos;
    uint32_t m_FrameSize, m_PlayingTime;
  };
}
