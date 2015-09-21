#include "pch.h"
#include "Engine.h"

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
	bool m_windowClosed;
	Engine m_engine;

public:
	virtual void Initialize(CoreApplicationView^ applicationView)
	{
		applicationView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &DXApp::OnActivated);

		m_windowClosed = false;
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
		m_engine.Initialize();

		auto window = CoreWindow::GetForCurrentThread();

		while (!m_windowClosed)
		{
			window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			// Run the graphics code here...
			m_engine.Update();
			m_engine.Render();
		}
	};
	
	virtual void Uninitialize() {};

	void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
	{
		auto window = CoreWindow::GetForCurrentThread();
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
		m_windowClosed = true;
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