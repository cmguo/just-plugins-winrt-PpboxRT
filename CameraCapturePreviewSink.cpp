// CameraCapturePreviewSink.cpp

#include "CameraCapturePreviewSink.h"

using namespace PpboxRT;

void CameraCapturePreviewSink::init(
    Callback_OnFrameAvailable callback, 
    Capture ^ capture)
{
    callback_ = callback;
    capture_ = capture;
}

// Called each time a preview frame is available
void CameraCapturePreviewSink::OnFrameAvailable(
	DXGI_FORMAT format,
	UINT width,
	UINT height,
	BYTE* pixels)
{
	// Insert your own code to process each captured frame here
    callback_(capture_, format, width, height, pixels);
}
