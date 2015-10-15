#pragma once
#include "pch.h"

using namespace Platform;

namespace MAPS
{
	inline void OutputDebug(Platform::String^ msg)
	{
		auto data = std::wstring(msg->Data());
		OutputDebugString(data.c_str());
	}

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw Exception::CreateException(hr);
		}
	}
}
