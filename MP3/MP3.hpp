#pragma once

#include <string>

//MPEG audio frame header
//variables are declared in their serialized order
struct FrameHeader
{
  static const unsigned int SERIALIZED_SIZE = 4;

  //bitmasks for frame header fields grouped by byte
  static const unsigned char FRAMESYNC_FIRST_BYTEMASK  = 0b11111111;

  static const unsigned char FRAMESYNC_SECOND_BYTEMASK = 0b1110000;
  static const unsigned char AUDIO_VERSION_MASK        = 0b00011000;
  static const unsigned char LAYER_DESCRIPTION_MASK    = 0b00000110;
  static const unsigned char PROTECTION_BIT_MASK       = 0b00000001;

  static const unsigned char BITRATE_INDEX_MASK        = 0b11110000;
  static const unsigned char SAMPLERATE_INDEX_MASK     = 0b00001100;
  static const unsigned char PADDING_BIT_MASK          = 0b00000010;
  static const unsigned char PRIVATE_BIT_MASK          = 0b00000001;

  static const unsigned char CHANNEL_MODE_MASK         = 0b11000000;
  static const unsigned char MODE_EXTENTION_MASK       = 0b00110000;
  static const unsigned char COPYRIGHT_BIT_MASK        = 0b00001000;
  static const unsigned char ORIGINAL_BIT_MASK         = 0b00000100;
  static const unsigned char EMPHASIS_MASK             = 0b00000011;

  char FrameSyncByte;
  bool FrameSyncBits: 3;

  //indicates MPEG standard version
  enum class AudioVersionID : unsigned
  {
    MPEG_2_5 = 0b00,
    INVALID  = 0b01,
    MPEG_2   = 0b10,
    MPEG_1   = 0b11,
  } AudioVersion : 2;

  //indicates which audio layer of the MPEG standard
  enum class LayerID : unsigned
  {
    INVALID = 0b00,
    LAYER_3 = 0b01,
    LAYER_2 = 0b10,
    LAYER_1 = 0b11,
  } Layer : 2;

  //indicates whether theres a 16 bit crc checksum following the header
  bool Protection : 1;

  //sample & bitrate indexes meaning differ depending on MPEG version
  //use GetBitrate() and GetSamplerate() 
  bool BitrateIndex : 4;
  bool SampleRateIndex : 2;

  //indicates whether the audio data is padded with 1 extra byte (slot)
  bool Padding : 1;

  //this is only informative
  bool Private : 1;

  //indicates channel mode
  enum class ChannelModeID : unsigned
  {
    STEREO = 0b00,
    JOINT  = 0b01, //joint stereo
    DUAL   = 0b10, //dual channel (2 mono channels)
    SINGLE = 0b11, //single channel (mono)
  } ChannelMode : 2;

  //Only used in joint channel mode. Meaning differ depending on audio layer
  //Use GetExtentionMode()
  bool ExtentionMode : 2;

  //indicates whether the audio is copyrighted
  bool Copyright : 1;

  //indicates whether the frame is located on the original media or a copy 
  bool Original : 1;

  //indicates to the decoder that the file must be de-emphasized, ie the 
  //decoder must 're-equalize' the sound after a Dolby-like noise supression. 
  //It is rarely used.
  enum class EmphasisID : unsigned
  {
    NONE     = 0b00,
    MS_50_15 = 0b01,
    INVALID  = 0b10,
    CCIT_J17 = 0b10,
  } Emphasis : 2;

  enum SpecialBitrate 
  {
    INVALID = -8000,
    ANY = 0,
  };

  signed int GetBitrate() const
  {
    //version, layer, bit index
    static signed char rateTable[4][4][16] = 
    {
      //version[00] = MPEG_2_5
      {
        //layer[00] = INVALID
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        //layer[01] = LAYER_3
        { 0,  1,  2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 18, 20, -1},
        //layer[10] = LAYER_2
        { 0,  1,  2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 18, 20, -1},
        //layer[11] = LAYER_1
        { 0,  4,  6,  7,  8, 10, 12, 14, 16, 18, 20, 22, 24, 28, 32, -1},
      },

      //version[01] = INVALID
      {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      },

      //version[10] = MPEG_2 
      {
        //layer[00] = INVALID
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        //layer[01] = LAYER_3
        { 0,  1,  2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 18, 20, -1},
        //layer[10] = LAYER_2
        { 0,  1,  2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 18, 20, -1},
        //layer[11] = LAYER_1
        { 0,  4,  6,  7,  8, 10, 12, 14, 16, 18, 20, 22, 24, 28, 32, -1},
      },

      //version[11] = MPEG_1
      {
        //layer[00] = INVALID
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        //layer[01] = LAYER_3
        { 0,  4,  5,  6,  7,  8, 10, 12, 14, 16, 20, 24, 28, 32, 40, -1},
        //layer[10] = LAYER_2
        { 0,  4,  6,  7,  8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, -1},
        //layer[11] = LAYER_1
        { 0,  4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, -1}, 
      },
    };

    return rateTable[AudioVersion][Layer][BitrateIndex] * 8000;
  }

  enum SpecialSampleRate
  {
    RESERVED = 0,
  };

  unsigned short GetSampleRate() const
  {
    //version, sample rate index 
    static unsigned short rateTable[4][4] =
    {
      //version[00] = MPEG_2_5
      {11025, 12000,  8000,  0},
      //version[01] = INVALID
      {  0,     0,       0,  0},
      //version[10] = MPEG_2
      {22050, 24000, 16000,  0},
      //version[11] = MPEG_1
      {44100, 48000, 32000,  0},
    };

    return rateTable[AudioVersion][SampleRateIndex];
  }

  std::string GetSampleRateStr() const
  {
    auto rate = GetSampleRate();
    return rate == SpecialSampleRate::RESERVED ? "INVALID" : std::to_string(rate);
  }

  std::string GetBitrateStr() const
  {
    auto rate = GetBitrate();

    return rate == SpecialBitrate::ANY      ? "ANY" :
           rate == SpecialBitrate::INVALID  ? "INVALID" :
           std::to_string(rate);
  }

  std::string GetVersionStr() const
  {
    return AudioVersion == AudioVersionID::MPEG_1   ? "1" :
           AudioVersion == AudioVersionID::MPEG_2   ? "2" :
           AudioVersion == AudioVersionID::MPEG_2_5 ? "2.5" : "INVALID";
  }

  std::string GetLayerStr() const
  {
    return Layer == LayerID::LAYER_1 ? "1" :
           Layer == LayerID::LAYER_2 ? "2" :
           Layer == LayerID::LAYER_3 ? "3" : "INVALID";
  }

};

