#include "pch.h"

using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Platform;

ref class DXApp sealed : public IFrameworkView
{
private:
	bool windowClosed;
public:
	virtual void Initialize(CoreApplicationView^ applicationView)
	{
		applicationView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &DXApp::OnActivated);

		windowClosed = false;
	}

	virtual void SetWindow(CoreWindow^ window)
	{
		window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &DXApp::OnKeyDown);

		window->Closed += ref new TypedEventHandler
			<CoreWindow^, CoreWindowEventArgs^>(this, &DXApp::Closed);
	}

	virtual void Load(String^ entryPoint) {};
	
	virtual void Run()
	{
		CoreWindow^ window = CoreWindow::GetForCurrentThread();

		while (!windowClosed)
		{
			window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			// Run game code here...
		}
	};
	
	virtual void Uninitialize() {};

	void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
	{
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		window->Activate();
	}

	void OnKeyDown(CoreWindow^ window, KeyEventArgs^ args)
	{
		if (args->VirtualKey == VirtualKey::Escape)
		{
			Closed(window, nullptr);
		}
	}

	void Closed(CoreWindow^ window, CoreWindowEventArgs^ args)
	{
		windowClosed = true;
	}
};

ref class DXAppSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView()
	{
		return ref new DXApp();
	}
};

[MTAThread]
int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new DXAppSource());
	return 0;
}