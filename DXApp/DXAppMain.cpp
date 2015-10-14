#include "pch.h"
#include "DXAppMain.h"
#include "Engine.h"

using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Platform;

void DXApp::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated += ref new TypedEventHandler
		<CoreApplicationView^, IActivatedEventArgs^>(this, &DXApp::OnActivated);

	windowClosed_ = false;
}

void DXApp::SetWindow(CoreWindow^ window)
{
	window->KeyDown += ref new TypedEventHandler
		<CoreWindow^, KeyEventArgs^>(this, &DXApp::OnKeyDown);

	window->Closed += ref new TypedEventHandler
		<CoreWindow^, CoreWindowEventArgs^>(this, &DXApp::Closed);
}

void DXApp::Load(String^ entryPoint) {};
	
void DXApp::Run()
{
	engine_.Initialize();

	auto window = CoreWindow::GetForCurrentThread();

	while (!windowClosed_)
	{
		window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

		// Run the graphics code here...
		engine_.Update();
		engine_.Render();
	}
};
	
void DXApp::Uninitialize() {};

void DXApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	auto window = CoreWindow::GetForCurrentThread();
	window->Activate();
}

void DXApp::OnKeyDown(CoreWindow^ window, KeyEventArgs^ args)
{
	if (args->VirtualKey == VirtualKey::Escape)
	{
		Closed(window, nullptr);
	}
	else if (args->VirtualKey == VirtualKey::W)
	{
		engine_.MoveCamera(Engine::MoveDirection::Forward);
	}
	else if (args->VirtualKey == VirtualKey::S)
	{
		engine_.MoveCamera(Engine::MoveDirection::Backward);
	}
	else if (args->VirtualKey == VirtualKey::A)
	{
		engine_.MoveCamera(Engine::MoveDirection::Left);
	}
	else if (args->VirtualKey == VirtualKey::D)
	{
		engine_.MoveCamera(Engine::MoveDirection::Right);
	}
}

void DXApp::Closed(CoreWindow^ window, CoreWindowEventArgs^ args)
{
	windowClosed_ = true;
}

[MTAThread]
int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new DXAppSource());
	return 0;
}