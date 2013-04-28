// CameraCaptureSampleSink.cpp

#include "Format.h"
#include "CameraCaptureSampleSink.h"

using namespace PpboxRT;

void CameraCaptureSampleSink::init(
    PPBOX_HANDLE capture, 
    UINT32 itrack)
{
    capture_ = capture;
    itrack_ = itrack;
}

// Called each time a captured frame is available	
void CameraCaptureSampleSink::OnSampleAvailable(
	ULONGLONG hnsPresentationTime,
	ULONGLONG hnsSampleDuration,
	DWORD cbSample,
	BYTE* pSample)
{
	PPBOX_Sample sample;
    memset(&sample, 0, sizeof(sample));
    sample.itrack = itrack_;
    sample.decode_time = hnsPresentationTime;
    sample.duration = (PP_uint32)hnsSampleDuration;
    sample.size = cbSample;
    sample.buffer = pSample;
    PPBOX_CapturePutSample(capture_, &sample);
}
