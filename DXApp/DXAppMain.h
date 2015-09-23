#pragma once

#include "pch.h"
#include "Engine.h"

using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Platform;

ref class DXApp sealed : public IFrameworkView
{
public:
	virtual void Initialize(CoreApplicationView^);
	virtual void SetWindow(CoreWindow^);
	virtual void Load(String^);
	virtual void Run();
	virtual void Uninitialize();
	void OnActivated(CoreApplicationView^, IActivatedEventArgs^);
	void OnKeyDown(CoreWindow^, KeyEventArgs^);
	void Closed(CoreWindow^, CoreWindowEventArgs^);

private:
	bool windowClosed_;
	Engine engine_;
};

ref class DXAppSource sealed : IFrameworkViewSource
{
public:
	inline virtual IFrameworkView^ CreateView()
	{
		return ref new DXApp();
	}
};
