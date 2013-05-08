// CameraCaptureSampleSink.cpp

#include "CameraCaptureSampleSink.h"

using namespace PpboxRT;

void CameraCaptureSampleSink::init(
    Callback_OnSampleAvailable callback, 
    Capture ^ capture, 
    UINT32 itrack)
{
    callback_ = callback;
    capture_ = capture;
    itrack_ = itrack;
}

void CameraCaptureSampleSink::close()
{
    callback_ = NULL;
    capture_ = nullptr;
}

// Called each time a captured frame is available	
void CameraCaptureSampleSink::OnSampleAvailable(
	ULONGLONG hnsPresentationTime,
	ULONGLONG hnsSampleDuration,
	DWORD cbSample,
	BYTE* pSample)
{
    callback_(capture_, itrack_, hnsPresentationTime, hnsSampleDuration, cbSample, pSample);
}
