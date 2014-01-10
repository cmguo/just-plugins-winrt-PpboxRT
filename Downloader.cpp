// Downloader.cpp

#include <windows.h>

#include "Downloader.h"

using namespace PpboxRT;
using namespace Platform;

Downloader::Downloader()
    : downloader_(NULL)
{
    InitializeCriticalSectionEx(&mutex_, 100, 0);
}

Downloader::~Downloader()
{
    DeleteCriticalSection(&mutex_);
}

void Downloader::async_open(
	String ^ playlink, 
	String ^ config, 
    String ^ savefile, 
	Callback ^ callback)
{
	char playlinkc[MAX_PATH];
	char configc[MAX_PATH];
	char savefilec[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, playlink->Data(), -1, playlinkc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, config->Data(), -1, configc, MAX_PATH, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, savefile->Data(), -1, savefilec, MAX_PATH, 0, 0);

	callback_ = callback;
	downloader_ = 
        PPBOX_DownloadOpen(playlinkc, configc, savefilec, s_call_back);
    instances()[downloader_] = this;
}

Error Downloader::get_stat(
    DownloadStatistc * stat)
{
    PPBOX_DownloadStatistic cstat;
    Error ec = (Error)PPBOX_GetDownloadInfo(downloader_, &cstat);
    if (ec == Error::success) {
        stat->total_size = cstat.total_size;
        stat->finish_size = cstat.finish_size;
        stat->speed = cstat.speed;
    }
	return ec;
}

void Downloader::Close()
{
	PPBOX_DownloadClose(downloader_);
}

std::map<PP_handle, Downloader ^> & Downloader::instances()
{
    static std::map<PP_handle, Downloader ^> s_instances;
    return s_instances;
}

void Downloader::s_call_back(
	PP_handle downloader, 
	PP_err ec)
{
	Downloader ^ inst = instances()[downloader];
	instances().erase(downloader);

    Callback ^ callback = inst->callback_;
	inst->callback_ = nullptr;
	callback->Invoke((Error)ec);
}
