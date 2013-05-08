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

void CameraCapturePreviewSink::close()
{
    callback_ = NULL;
    capture_ = nullptr;
}

// Called each time a preview frame is available
void CameraCapturePreviewSink::OnFrameAvailable(
	DXGI_FORMAT format,
	UINT width,
	UINT height,
	BYTE* pixels)
{
	// Insert your own code to process each captured frame here
    pixels_ = pixels;
    callback_(capture_, format, width, height, pixels);
}
