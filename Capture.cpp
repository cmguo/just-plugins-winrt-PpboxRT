// Capture.cpp

#include <implements.h>
#include <agile.h>

#include "Capture.h"
#include "CameraCapturePreviewSink.h"
#include "CameraCaptureSampleSink.h"

using namespace PpboxRT;
using namespace Platform;
using namespace Microsoft::WRL;

Capture::Capture()
    : capture_(NULL)
{
    InitializeCriticalSectionEx(&mutex_, 100, 0);
}

Capture::~Capture()
{
    DeleteCriticalSection(&mutex_);
}

Error Capture::open(
    AudioVideoCaptureDevice ^ device, 
    String ^ dest)
{
    char namec[MAX_PATH];
    char destc[MAX_PATH];

    pAudioVideoCaptureDevice = device;

    WideCharToMultiByte(CP_ACP, 0, device->ToString()->Data(), -1, namec, MAX_PATH, 0, 0);
    WideCharToMultiByte(CP_ACP, 0, dest->Data(), -1, destc, MAX_PATH, 0, 0);

    capture_ = PPBOX_CaptureCreate(namec, destc);
    if (capture_ == NULL) {
        return (Error)PPBOX_GetLastError();
    }

    return Error::success;
}

Error Capture::set_media(
    Media ^ media)
{
    PPBOX_CaptureConfigData config;
    config.stream_count = media->streams->Length;
    config.get_sample_buffers = NULL;
    config.free_sample = NULL;
    PP_err ec = PPBOX_CaptureInit(capture_, &config);

    if (ec != ppbox_success) {
        return (Error)ec;
    }

    setup_preview_sink();

    for (uint32 i = 0; i < media->streams->Length; i++)
    {
        Stream ^ stream = media->streams[i];
        setup_sample_sink(stream->type, i);
        PPBOX_StreamInfo info;
        stream->to_info(info);
        ec = PPBOX_CaptureSetStream(capture_, i, &info);
        if (ec != ppbox_success)
            break;
    }

    return (Error)ec;
}

void Capture::close()
{
    PPBOX_CaptureDestroy(capture_);
}

HRESULT Capture::setup_preview_sink()
{
    ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
    HRESULT hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);

    // Save the pointer to the native interface
    pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;

    pCameraCaptureDeviceNative->SetPreviewFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);	

    // Create the sink
    MakeAndInitialize<CameraCapturePreviewSink>(&pCameraCapturePreviewSink);
    pCameraCapturePreviewSink->init(on_preview_sample, this);
    pCameraCaptureDeviceNative->SetPreviewSink(pCameraCapturePreviewSink);

    return hr;
}

HRESULT Capture::setup_sample_sink(
    StreamType type, 
    uint32 index)
{
    HRESULT hr = S_OK;

    if (pAudioVideoCaptureDeviceNative == NULL) {
        // Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
        IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
        hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);
        if (FAILED(hr)) {
            return hr;
        }
        // Save the pointer to the IAudioVideoCaptureDeviceNative native interface
        pAudioVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;
    }

    if (type == StreamType::video) {
        // Initialize and set the CaptureSampleSink class as sink for captures samples
        MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSinkVideo);
        if (SUCCEEDED(hr)) {
            pCameraCaptureSampleSinkVideo->init(capture_, index);
            pAudioVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSinkVideo);
        }
    } else {
        hr = MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSinkAudio);
        if (SUCCEEDED(hr)) {
            pCameraCaptureSampleSinkAudio->init(capture_, index);
            hr = pAudioVideoCaptureDeviceNative->SetAudioSampleSink(pCameraCaptureSampleSinkAudio);
        }
    }

    return hr;
}

void Capture::on_preview_sample(
    Capture ^ capture, 
    DXGI_FORMAT format,
    UINT width,
    UINT height,
    BYTE* pixels)
{
    PPBOX_Sample sample;
    sample.size = width * height * 4;
    sample.buffer = pixels;
    capture->preview_frame_available(capture, ref new Sample(sample));
}
