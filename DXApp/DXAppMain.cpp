#include "pch.h"
#include "DXAppMain.h"
#include "Engine.h"
#include <sstream>

using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Platform;
using namespace MAPS;

void DXApp::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated += ref new TypedEventHandler
		<CoreApplicationView^, IActivatedEventArgs^>(this, &DXApp::OnActivated);

	windowClosed_ = false;
	pointerPressed_ = false;
}

void DXApp::SetWindow(CoreWindow^ window)
{
	window->KeyDown += ref new TypedEventHandler
		<CoreWindow^, KeyEventArgs^>(this, &DXApp::OnKeyDown);

	window->PointerWheelChanged += ref new TypedEventHandler
		<CoreWindow^, PointerEventArgs^>(this, &DXApp::OnPointerWheelChanged);

	window->PointerPressed += ref new TypedEventHandler
		<CoreWindow^, PointerEventArgs^>(this, &DXApp::OnPointerPressed);

	window->PointerReleased += ref new TypedEventHandler
		<CoreWindow^, PointerEventArgs^>(this, &DXApp::OnPointerReleased);

	window->PointerMoved += ref new TypedEventHandler
		<CoreWindow^, PointerEventArgs^>(this, &DXApp::OnPointerMoved);

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

void DXApp::OnPointerWheelChanged(CoreWindow^ window, PointerEventArgs^ args)
{
	auto wheelData = args->CurrentPoint->Properties->MouseWheelDelta;
	if (wheelData > 0)
	{
		engine_.MoveCamera(Engine::MoveDirection::Forward);
	}
	else // wheelData < 0
	{
		engine_.MoveCamera(Engine::MoveDirection::Backward);
	}
}

void DXApp::OnPointerPressed(CoreWindow^ window, PointerEventArgs^ args)
{
	auto point = args->CurrentPoint;
	if (point->Properties->IsLeftButtonPressed)
	{
		lastPointerPosX_ = point->Position.X;
		lastPointerPosY_ = point->Position.Y;
		pointerPressed_ = true;
	}
}

void DXApp::OnPointerReleased(CoreWindow^ window, PointerEventArgs^ args)
{
	pointerPressed_ = false;
}

void DXApp::OnPointerMoved(CoreWindow^ window, PointerEventArgs^ args)
{
	if (pointerPressed_)
	{
		auto pos = args->CurrentPoint->Position;
		auto dx = pos.X - lastPointerPosX_;
		auto dy = pos.Y - lastPointerPosY_;

		// rotate the camera
		engine_.RotateCamera(dx, dy);

		lastPointerPosX_ = pos.X;
		lastPointerPosY_ = pos.Y;
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