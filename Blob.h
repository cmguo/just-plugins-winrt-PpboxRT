#pragma once

#include "Format.h"

#include <map>

namespace PpboxRT
{

    public ref class Blob sealed
    {
    public:
		static Error insert(
			String ^ key, 
			Array<uint8> const ^ data, 
			bool remove_on_get);

		static Error get(
            String ^ key, 
			Array<uint8> ^* data);

		static Error remove(
            String ^ key);
    };

}
