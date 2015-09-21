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
public:
	virtual void Initialize(CoreApplicationView^ applicationView)
	{
		applicationView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &DXApp::OnActivated);
	}

	virtual void SetWindow(CoreWindow^ window)
	{
		window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &DXApp::OnKeyDown);
	}

	virtual void Load(String^ entryPoint) {};
	
	virtual void Run()
	{
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
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
			MessageDialog dialog("You have pressed the Escape button!");
			dialog.ShowAsync();
		}
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