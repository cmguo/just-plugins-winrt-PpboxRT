#pragma once

using namespace Platform;
using namespace Windows::Foundation::Collections;

#include <plugins/ppbox/ppbox_runtime.h>

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
        video = PPBOX_StreamType_VIDE, 
        audio = PPBOX_StreamType_AUDI, 
    };

	public enum class StreamSubType: int
    {
        video_avc = PPBOX_VideoSubType_AVC1, 
        audio_aac = PPBOX_AudioSubType_MP4A, 
        audio_mp3 = PPBOX_AudioSubType_MP1A, 
        audio_wma = PPBOX_AudioSubType_WMA2,
        video_wmv = PPBOX_VideoSubType_WMV3, 
    };

	public enum class FormatType: int
    {
        none                    = PPBOX_FormatType_none, 
        video_avc_packet        = PPBOX_FormatType_video_avc_packet, 
        video_avc_byte_stream   = PPBOX_FormatType_video_avc_byte_stream, 
        audio_raw               = PPBOX_FormatType_audio_raw, 
        audio_adts              = PPBOX_FormatType_audio_adts,
    };

    public value struct VideoInfo
    {
        uint32 width;
		uint32 height;
        uint32 frame_rate;
    };

    public value struct AudioInfo
    {
        uint32 channel_count;
        uint32 sample_size;
        uint32 sample_rate;
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
			PPBOX_StreamInfo & stream);

        void to_info(
			PPBOX_StreamInfo & stream);

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
			PPBOX_Sample & sample);

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
