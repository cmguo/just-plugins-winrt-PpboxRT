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

    setup_preview_sink();

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
    config.ordered = false;
    config.get_sample_buffers = NULL;
    config.free_sample = NULL;
    PP_err ec = ppbox_success;
    ec = PPBOX_CaptureInit(capture_, &config);

    if (ec != ppbox_success) {
        return (Error)ec;
    }

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

    media_ = media;

    return (Error)ec;
}

#define SAFE_RELEASE(p) if (p) { p->Release(); p = NULL; }
#define SAFE_CLOSE_RELEASE(p) if (p) { p->close(); p->Release(); p = NULL; }

void Capture::close()
{
    PPBOX_CaptureDestroy(capture_);

    SAFE_CLOSE_RELEASE(pCameraCaptureSampleSinkAudio);
    SAFE_CLOSE_RELEASE(pCameraCaptureSampleSinkVideo);

    //pCameraCaptureDeviceNative->SetPreviewSink(NULL);

    //SAFE_CLOSE_RELEASE(pCameraCapturePreviewSink);
    SAFE_RELEASE(pCameraCapturePreviewSink);
    SAFE_RELEASE(pCameraCaptureDeviceNative);
    SAFE_RELEASE(pAudioVideoCaptureDeviceNative);

    pAudioVideoCaptureDevice = nullptr;
    media_ = nullptr;
}

void Capture::get_preview_pixels(
    Platform::WriteOnlyArray<int32> ^ out)
{
    memcpy(out->Data, pCameraCapturePreviewSink->pixels(), out->Length * 4);
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
            pCameraCaptureSampleSinkVideo->init(on_capture_first_sample, this, index);
            pAudioVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSinkVideo);
        }
    } else {
        hr = MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSinkAudio);
        if (SUCCEEDED(hr)) {
            pCameraCaptureSampleSinkAudio->init(on_capture_first_sample, this, index);
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
    //PPBOX_Sample sample;
    //sample.size = width * height * 4;
    //sample.buffer = pixels;
    capture->preview_frame_available(capture);
}

void Capture::on_capture_first_sample(
    Capture ^ capture, 
    UINT itrack,
    ULONGLONG hnsPresentationTime,
    ULONGLONG hnsSampleDuration,
    DWORD cbSample,
    BYTE* pSample)
{
    Stream ^ stream = capture->media_->streams[itrack];
    if (stream->type == StreamType::video) {
        capture->pCameraCaptureSampleSinkVideo->init(on_capture_sample, capture, itrack);
        BYTE * p = (BYTE *)memchr(pSample, 0x67, cbSample);
        BYTE * b = p - 4;
        p = (BYTE *)memchr(pSample, 0x65, cbSample);
        BYTE * e = p - 4;
        stream->codec_data = ArrayReference<PP_ubyte>(b, e - b);
        PPBOX_StreamInfo info;
        stream->to_info(info);
        PPBOX_CaptureSetStream(capture->capture_, itrack, &info);
    } else {
        capture->pCameraCaptureSampleSinkAudio->init(on_capture_sample, capture, itrack);
        uint32 frequency_table[] = 
        {
            96000, 88200, 64000, 48000, 
            44100, 32000, 24000, 22050, 
            16000, 12000, 11025, 8000,  
            7350,  
        };
        PP_ubyte object_type = 2; // AAC LC
        PP_ubyte frequency_index = std::find(frequency_table, frequency_table + 13, stream->audio.sample_rate) - frequency_table;
        PP_ubyte channel_count = stream->audio.channel_count;
        PP_ubyte AacConfig[2] = {
            (object_type << 3) | (frequency_index >> 1), 
            (frequency_index << 7) | (channel_count << 3)
        };
        stream->codec_data = ArrayReference<PP_ubyte>(AacConfig, sizeof(AacConfig));
        PPBOX_StreamInfo info;
        stream->to_info(info);
        PPBOX_CaptureSetStream(capture->capture_, itrack, &info);
    }
    on_capture_sample(capture, itrack, hnsPresentationTime, hnsSampleDuration, cbSample, pSample);
}

void Capture::on_capture_sample(
    Capture ^ capture, 
    UINT itrack,
    ULONGLONG hnsPresentationTime,
    ULONGLONG hnsSampleDuration,
    DWORD cbSample,
    BYTE* pSample)
{
	PPBOX_Sample sample;
    memset(&sample, 0, sizeof(sample));
    sample.itrack = itrack;
    sample.decode_time = hnsPresentationTime;
    sample.duration = (PP_uint)hnsSampleDuration;
    sample.size = cbSample;
    sample.buffer = pSample;
    Stream ^ stream = capture->media_->streams[itrack];
    if (stream->type == StreamType::video) {
        BYTE * p = (BYTE *)memchr(pSample, 0x65, cbSample < 50 ? cbSample : 50);
        if (p && *(p - 1) == 0x01) {
            sample.flags |= PPBOX_SampleFlag_sync;
        }
    } else {
    }
    PPBOX_CapturePutSample(capture->capture_, &sample);
}

