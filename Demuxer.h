#pragma once

#include "Format.h"

namespace PpboxRT
{

    public ref class Demuxer sealed
    {
    public:
        Demuxer();

        virtual ~Demuxer();

    public:
		void async_open(
			String ^ playlink, 
			String ^ config, 
			Callback ^ callback);

		Error get_media(
			Media ^* media);

		Error seek(
			uint64 time);

		Error get_sample(
			Sample ^* sample);

		void close();

	private:
		static void s_call_back(
			void * ctx, 
			long ec);

	private:
		Demuxer ^ self_ref_;
		Callback ^ callback_;
        CRITICAL_SECTION mutex_;
    };

}
