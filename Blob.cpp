// Blob.cpp

#include <windows.h>

#include "Blob.h"

using namespace PpboxRT;
using namespace Platform;

Error Blob::insert(
	String ^ key, 
	Array<uint8> const ^ data, 
	bool remove_on_get)
{
	char keyc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, key->Data(), -1, keyc, MAX_PATH, 0, 0);

    PPBOX_ConstBuffer buf = {data->Data, data->Length};
    Error ec = (Error)PPBOX_InsertBlob(keyc, &buf, remove_on_get);
    return ec;
}

Error Blob::get(
	String ^ key, 
	Array<uint8> ^* data)
{
	char keyc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, key->Data(), -1, keyc, MAX_PATH, 0, 0);

    PPBOX_ConstBuffer buf = {0, 0};
    Error ec = (Error)PPBOX_GetBlob(keyc, &buf);
    if (ec == Error::success) {
        *data = ref new Array<uint8>((uint8 *)buf.data, buf.len);
    }
	return ec;
}

Error Blob::remove(
	String ^ key)
{
	char keyc[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, key->Data(), -1, keyc, MAX_PATH, 0, 0);

    Error ec = (Error)PPBOX_RemoveBlob(keyc);
	return ec;
}
