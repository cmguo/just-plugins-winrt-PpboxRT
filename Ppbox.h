#pragma once

#include "Format.h"

#include <map>

namespace PpboxRT
{

    public ref class Ppbox sealed
    {
    public:
		static Error start(
			String ^ gid, 
			String ^ pid, 
			String ^ auth);

		static Error stop();
    };

}
