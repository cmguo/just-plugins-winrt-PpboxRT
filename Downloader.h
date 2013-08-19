#pragma once

#include "Format.h"

#include <map>

namespace PpboxRT
{

    public value struct DownloadStatistc
    {
		uint64 total_size;
        uint64 finish_size;
        uint32 speed;
    };

    public ref class Downloader sealed
    {
    public:
        Downloader();

        virtual ~Downloader();

    public:
		void async_open(
			String ^ playlink, 
			String ^ config, 
			String ^ savefile, 
			Callback ^ callback);

		Error get_stat(
			DownloadStatistc * stat);

		void close();

	private:
		static void s_call_back(
			PP_handle downloader, 
			PP_err ec);

	private:
        static std::map<PP_handle, Downloader ^> & instances();

	private:
        CRITICAL_SECTION mutex_;
		Callback ^ callback_;
        PP_handle downloader_;
    };

}
