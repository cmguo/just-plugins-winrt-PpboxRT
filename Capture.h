#pragma once

#include "Format.h"

#include <Windows.Phone.Media.Capture.Native.h>
using namespace Windows::Phone::Media::Capture;

namespace PpboxRT
{

    class CameraCapturePreviewSink;
    class CameraCaptureSampleSink;

    ref class Capture;

    public delegate void PreviewFrameAvailableHandler(Capture ^ sender, Sample ^ s);

    public ref class Capture sealed
    {
    public:
        Capture();

        virtual ~Capture();

    public:
		Error open(
			AudioVideoCaptureDevice ^ device, 
			String ^ dest);

		Error set_media(
			Media ^ media);

		void close();

    public:
        event PreviewFrameAvailableHandler ^ preview_frame_available;

	private:
        HRESULT setup_preview_sink();

        HRESULT setup_sample_sink(
            StreamType type, 
            uint32 index);

        static void on_preview_sample(
            Capture ^ capture, 
            DXGI_FORMAT format,
            UINT width,
            UINT height,
            BYTE* pixels);

	private:
        CRITICAL_SECTION mutex_;
        PPBOX_HANDLE capture_;

    private:
        Windows::Phone::Media::Capture::AudioVideoCaptureDevice ^pAudioVideoCaptureDevice;
        ICameraCaptureDeviceNative* pCameraCaptureDeviceNative;
        IAudioVideoCaptureDeviceNative* pAudioVideoCaptureDeviceNative;
        CameraCapturePreviewSink* pCameraCapturePreviewSink;
        CameraCaptureSampleSink* pCameraCaptureSampleSinkVideo;
        CameraCaptureSampleSink* pCameraCaptureSampleSinkAudio;
    };

}
