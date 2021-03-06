﻿#pragma once

using namespace Platform;
using namespace Windows::Foundation::Collections;

#include <just/just/IPpboxBoostTypes.h>
#define JUST_DISABLE_AUTO_START
#include <just/just/IPpboxRuntime.h>

namespace PpboxRT
{

	public enum class Error: int
	{
		success = 0, 
		not_start, 
		already_start, 
		not_open, 
		already_open, 
		operation_canceled, 
		would_block, 
		stream_end, 
		logic_error, 
		network_error, 
		demux_error, 
		certify_error, 
		other_error = 1024, 
	};

	public enum class StreamType: int
    {
        video = JUST_StreamType::VIDE, 
        audio = JUST_StreamType::AUDI, 
    };

	public enum class StreamSubType: int
    {
        video_avc = JUST_VideoSubType::AVC1, 
        audio_aac = JUST_AudioSubType::MP4A, 
        audio_mp3 = JUST_AudioSubType::MP3, 
        audio_wma = JUST_AudioSubType::WMA2,
        video_wmv = JUST_VideoSubType::WMV3, 
    };

	public enum class FormatType: int
    {
        none                    = JUST_FormatType::none, 
        video_avc_packet        = JUST_FormatType::video_avc_packet, 
        video_avc_byte_stream   = JUST_FormatType::video_avc_byte_stream, 
        audio_raw               = JUST_FormatType::audio_raw, 
        audio_adts              = JUST_FormatType::audio_adts,
    };

    public value struct VideoInfo
    {
        uint32 width;
		uint32 height;
        uint32 frame_rate_num;
        uint32 frame_rate_den;
    };

    public value struct AudioInfo
    {
        uint32 channel_count;
        uint32 sample_size;
        uint32 sample_rate;
        uint32 sample_per_frame;
    };

	public delegate void Callback(Error);

	public ref class Stream sealed
	{
	public:
		property StreamType type { 
            StreamType get() { return type_; } 
            void set(StreamType t) { type_ = t; } 
        }

		property StreamSubType sub_type { 
            StreamSubType get() { return sub_type_; } 
            void set(StreamSubType t) { sub_type_ = t; } 
        }

		property uint32 time_scale { 
            uint32 get() { return time_scale_; } 
            void set(uint32 t) { time_scale_ = t; } 
        }

		property uint32 bitrate { 
            uint32 get() { return bitrate_; } 
            void set(uint32 t) { bitrate_ = t; } 
        }

		property VideoInfo video { 
            VideoInfo get() { return video_; } 
            void set(VideoInfo t) { video_ = t; } 
        }

		property AudioInfo audio { 
            AudioInfo get() { return audio_; } 
            void set(AudioInfo t) { audio_ = t; } 
        }

		property FormatType format_type { 
            FormatType get() { return format_type_; } 
            void set(FormatType t) { format_type_ = t; } 
        }

		property Array<uint8> ^ codec_data { 
            Array<uint8> ^ get() { return codec_data_; } 
            void set(Array<uint8> const ^ t) { codec_data_ = ref new Array<uint8>(t); } 
        }

	public:
		Stream();

	internal:
		Stream(
			JUST_StreamInfo & stream);

        void to_info(
			JUST_StreamInfo & stream);

	private:
		StreamType type_;
		StreamSubType sub_type_;
        uint32 time_scale_;
		uint32 bitrate_;

		VideoInfo video_;
		AudioInfo audio_;

        FormatType format_type_;
		Array<uint8> ^ codec_data_;
	};

	public ref class Media sealed
	{
	public:
		property uint64 duration { 
            uint64 get() { return duration_; }
            void set(uint64 t) { duration_ = t; } 
        }

		property Array<Stream ^> ^ streams { 
            Array<Stream ^> ^ get() { return streams_; }
            void set(Array<Stream ^> const ^ t) { streams_ = ref new Array<Stream ^>(t); } 
        }

	public:
		Media();

	internal:
		Media(
			uint64 duration, 
			Array<Stream ^> ^ streams);

	private:
		uint64 duration_;
		Array<Stream ^> ^ streams_;
	};

	public ref class Sample sealed
	{
	public:
		property uint32 index { uint32 get() { return index_; } }

		property uint64 time { uint64 get() { return time_; } }

		property bool is_sync { bool get() { return (flag_ & sync) != 0; } }

		property bool is_discontinue { bool get() { return (flag_ & discontinue) != 0; } }

		property Array<uint8> ^ data { 
            Array<uint8> ^ get() { return data_; } 
        }

	public:
		Sample();

	internal:
		Sample(
			JUST_Sample & sample);

	private:
		enum
		{
			sync = 1, 
			discontinue = 2, 
		};
		
        uint32 index_;
        uint32 flag_;
        uint64 time_;
		Array<uint8> ^ data_;
	};

}
