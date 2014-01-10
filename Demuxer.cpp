// Demuxer.cpp

#include <windows.h>

#include "Demuxer.h"

using namespace PpboxRT;
using namespace Platform;

Demuxer::Demuxer()
{
    InitializeCriticalSectionEx(&mutex_, 100, 0);
}

Demuxer::~Demuxer()
{
    DeleteCriticalSection(&mutex_);
}

void Demuxer::async_open(
	String ^ playlink, 
	String ^ config, 
	Callback ^ callback)
{
	char playlinkc[MAX_PATH];
	char configc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, playlink->Data(), -1, playlinkc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, config->Data(), -1, configc, MAX_PATH, 0, 0);

	self_ref_ = this;
	callback_ = callback;
	PPBOX_AsyncOpenEx(playlinkc, configc, &self_ref_, s_call_back);
}

Error Demuxer::get_media(
	Media ^* media)
{
	PP_ulong d = PPBOX_GetDuration();
	PP_uint n = PPBOX_GetStreamCount();
	Array<Stream ^> ^ streams = ref new Platform::Array<Stream ^>(n);
	for (PP_uint i = 0; i < n; ++i) {
		PPBOX_StreamInfo info;
		PPBOX_GetStreamInfo(i, &info);
		Stream ^ s = ref new Stream(info);
		streams->set(i, s);
	}
	*media = ref new Media(d * 10000, streams);
	return Error::success;
}

Error Demuxer::seek(
	uint64 time)
{
    EnterCriticalSection(&mutex_);
	PP_err ec = PPBOX_Seek((PP_uint)(time / 10000));
    LeaveCriticalSection(&mutex_);
	return (Error)ec;
}

Error Demuxer::get_sample(
	Sample ^* sample)
{
    EnterCriticalSection(&mutex_);
	PPBOX_Sample s;
	PP_err ec = PPBOX_ReadSample(&s);
	if (ec == 0) {
		*sample = ref new Sample(s);
	}
    LeaveCriticalSection(&mutex_);
	return (Error)ec;
}

void Demuxer::Close()
{
	PPBOX_Close();
}

void Demuxer::s_call_back(
	PP_context ctx, 
	PP_err ec)
{
	Demuxer ^ inst = *(Demuxer ^ *)ctx;
	inst->self_ref_ = nullptr;

    Callback ^ callback = inst->callback_;
	inst->callback_ = nullptr;
	callback->Invoke((Error)ec);
}
