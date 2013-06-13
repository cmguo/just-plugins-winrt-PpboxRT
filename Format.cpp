﻿// Demuxer.cpp

#include <windows.h>

#include "Format.h"

using namespace PpboxRT;
using namespace Platform;

Stream::Stream()
{
}

Stream::Stream(
	PPBOX_StreamInfo & info)
{
	type_ = (StreamType)info.type;
	sub_type_ = (StreamSubType)info.sub_type;
    time_scale_ = info.time_scale;
    bitrate_ = info.bitrate;
	if (type_ == StreamType::video) {
		video_.width = info.video_format.width;
		video_.height = info.video_format.height;
        video_.frame_rate_num = info.video_format.frame_rate_num;
        video_.frame_rate_den = info.video_format.frame_rate_den;
	} else {
		audio_.channel_count = info.audio_format.channel_count;
		audio_.sample_size = info.audio_format.sample_size;
        audio_.sample_rate = info.audio_format.sample_rate;
        audio_.sample_per_frame = info.audio_format.sample_per_frame;
    }
    format_type = (FormatType)info.format_type;
	codec_data_ = ref new Platform::Array<uint8>((uint8 *)info.format_buffer, info.format_size);
}

void Stream::to_info(
    PPBOX_StreamInfo & info)
{
    info.type = (PP_uint)type_;
    info.sub_type = (PP_uint)sub_type_;
    info.time_scale = time_scale_;
    info.bitrate = bitrate_;
	if (type_ == StreamType::video) {
		info.video_format.width = video_.width;
		info.video_format.height = video_.height;
        info.video_format.frame_rate_num = video_.frame_rate_num;
        info.video_format.frame_rate_den = video_.frame_rate_den;
	} else {
		info.audio_format.channel_count = audio_.channel_count;
        info.audio_format.sample_size = audio_.sample_size;
        info.audio_format.sample_rate = audio_.sample_rate;
        info.audio_format.sample_per_frame = audio_.sample_per_frame;
	}
    info.format_type = (PP_uint)format_type;
    info.format_size = codec_data_->Length;
    PP_ubyte * buffer = new PP_ubyte[codec_data_->Length];
    memcpy(buffer, codec_data_->Data, codec_data_->Length);
    info.format_buffer = buffer;
}

Media::Media()
{
}

Media::Media(
	uint64 duration, 
	Array<Stream ^> ^ streams)
	: duration_(duration)
	, streams_(streams)
{
}

Sample::Sample()
{
}

Sample::Sample(
	PPBOX_Sample & sample)
{
	index_ = sample.itrack;
	flag_ = 0;
	if (sample.flags & PPBOX_SampleFlag_sync) {
		flag_ |= sync;
	}
	if (sample.flags & PPBOX_SampleFlag_discontinuity) {
		flag_ |= discontinue;
	}
	time_ = sample.decode_time + sample.composite_time_delta;
	data_ = Platform::ArrayReference<uint8>((uint8 *)sample.buffer, sample.size);
}
