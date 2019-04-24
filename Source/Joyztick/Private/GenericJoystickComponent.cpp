// Fill out your copyright notice in the Description page of Project Settings.
#include "GenericJoystickComponent.h"

#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"

#include <joystickapi.h>


#define WINDOW_CLASS_NAME L"UE4GenericJoystick"
#define AXISLOW 0x0000
#define AXISHIGH 0xffff
#define JOYSTICK_BUTTON_MSG 0x3a2


DEFINE_LOG_CATEGORY(LogJoyztick);

//Forward Declaration (avoid linker bullsh1t)
HWND UGenericJoystickComponent::hWnd;
HINSTANCE UGenericJoystickComponent::hInstance;
TArray<UGenericJoystickComponent*> UGenericJoystickComponent::RegisteredComponents;
TMap<int, int> UGenericJoystickComponent::ButtonsMap;
TMap<int, int> UGenericJoystickComponent::ButtonsReleaseMap;
bool UGenericJoystickComponent::bCapturingJoystick;
//End Forward Declarations

// Sets default values for this component's properties
UGenericJoystickComponent::UGenericJoystickComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bJoyztickInputEnabled = true;

	// ...
}


// Called when the game starts
void UGenericJoystickComponent::BeginPlay()
{
	Super::BeginPlay();

	RegisteredComponents.AddUnique(this);

	if (!IsInitialized())
	{
		CreateWindowClass();
		InitButtonsMap();
	}
	
}

void UGenericJoystickComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	RegisteredComponents.Remove(this);
	FreeResources();
}

void UGenericJoystickComponent::CreateWindowClass()
{
	hInstance = GetModuleHandle(NULL);
	RegisterWindowClass();
	InitInstance();
}

void UGenericJoystickComponent::InitButtonsMap()
{
	ButtonsMap.Add(JOY_BUTTON1, 0);
	ButtonsMap.Add(JOY_BUTTON2, 1);
	ButtonsMap.Add(JOY_BUTTON3, 2);
	ButtonsMap.Add(JOY_BUTTON4, 3);
	ButtonsMap.Add(JOY_BUTTON5, 4);
	ButtonsMap.Add(JOY_BUTTON6, 5);
	ButtonsMap.Add(JOY_BUTTON7, 6);
	ButtonsMap.Add(JOY_BUTTON8, 7);
	ButtonsMap.Add(JOY_BUTTON9, 8);
	ButtonsMap.Add(JOY_BUTTON10, 9);
	ButtonsMap.Add(JOY_BUTTON11, 10);
	ButtonsMap.Add(JOY_BUTTON12, 11);
	ButtonsMap.Add(JOY_BUTTON13, 12);
	ButtonsMap.Add(JOY_BUTTON14, 13);
	ButtonsMap.Add(JOY_BUTTON15, 14);
	ButtonsMap.Add(JOY_BUTTON16, 15);
	ButtonsMap.Add(JOY_BUTTON17, 16);
	ButtonsMap.Add(JOY_BUTTON18, 17);
	ButtonsMap.Add(JOY_BUTTON19, 18);
	ButtonsMap.Add(JOY_BUTTON20, 19);
	ButtonsMap.Add(JOY_BUTTON21, 20);
	ButtonsMap.Add(JOY_BUTTON22, 21);
	ButtonsMap.Add(JOY_BUTTON23, 22);
	ButtonsMap.Add(JOY_BUTTON24, 23);
	ButtonsMap.Add(JOY_BUTTON25, 24);
	ButtonsMap.Add(JOY_BUTTON26, 25);
	ButtonsMap.Add(JOY_BUTTON27, 26);
	ButtonsMap.Add(JOY_BUTTON28, 27);
	ButtonsMap.Add(JOY_BUTTON29, 28);
	ButtonsMap.Add(JOY_BUTTON30, 29);
	ButtonsMap.Add(JOY_BUTTON31, 30);
	ButtonsMap.Add(JOY_BUTTON32, 31);
	
}

LRESULT CALLBACK UGenericJoystickComponent::WndProcGlobal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	//We don`t need to listen messages when there is no active components
	if (RegisteredComponents.Num() == 0 && message != WM_DESTROY)
	{
		PostMessageW(hWnd, WM_DESTROY, 0, 0);
		return NULL;
	}

	for (auto Joystick : RegisteredComponents)
	{
		Joystick->WndProc(hWnd, message, wParam, lParam);
	}
	switch (message)
	{
		case WM_CREATE:
		{
			if (bCapturingJoystick)
				break;

			if (auto error = joySetCapture(
				hWnd,
				JOYSTICKID1,
				(UINT)20,
				FALSE
			))
			{
				MessageBox(
					hWnd,
					(LPCWSTR)L"Error capturing the joystick",
					(LPCWSTR)L"Error",
					MB_ICONERROR
				);
			}
			else
			{
				bCapturingJoystick = true;
			}
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return NULL;
}

bool UGenericJoystickComponent::InitInstance()
{
	hWnd = CreateWindowW(WINDOW_CLASS_NAME, WINDOW_CLASS_NAME, WS_EX_WINDOWEDGE,
		CW_USEDEFAULT, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

	auto error = GetLastError();

	if (!hWnd)
	{
		return false;
	}

	UpdateWindow(hWnd);

	return true;
}

bool UGenericJoystickComponent::IsNotZero(FVector2D inVector)
{
	return !(inVector.X == .0f && inVector.Y == .0f);
}

UGenericJoystickComponent::~UGenericJoystickComponent()
{

	
}

bool UGenericJoystickComponent::IsInitialized()
{
	
	WNDCLASS wc;
	return GetClassInfo(hInstance, WINDOW_CLASS_NAME, &wc);
}

LRESULT CALLBACK UGenericJoystickComponent::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!bJoyztickInputEnabled)
		return NULL;


	switch (message)
	{
		case JOYSTICK_BUTTON_MSG:
		{
			if (OnKeyPress.IsBound())
			{

				TArray<int> PressedButtons;
				GetButtonsByMask((UINT8) wParam, ButtonsMap, PressedButtons);

				ButtonsPressedThisFrame = PressedButtons;


				for (auto button : PressedButtons)
				{
					OnKeyDown.Broadcast(button);
					if (!ButtonsPressedLastFrame.Contains(button))
						OnKeyPress.Broadcast(button);
				}
			}
			break;
		}
		case MM_JOY1MOVE:
		{
			if (OnJoystickMove.IsBound())
			{
				FVector2D Axis;
				Axis.X = LOWORD(lParam);
				Axis.Y = HIWORD(lParam);

				Axis = NormalizeJoyInput(Axis);

				if(IsNotZero(Axis) || bTriggerAxisMoveWhenAxisEqualsZero)
					OnJoystickMove.Broadcast(Axis);
			}
			break;
		}
		

	}
	return LRESULT();
}

ATOM UGenericJoystickComponent::RegisterWindowClass()
{
	WNDCLASS wc = {0};
	if (IsInitialized()) //Was registered already
	{
		return NULL;
	}
	WNDCLASSEXW wcex = {0};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.lpfnWndProc = WndProcGlobal;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	

	return RegisterClassExW(&wcex);
}


// Called every frame
void UGenericJoystickComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CheckReleasedButtons();
}

void  UGenericJoystickComponent::GetButtonsByMask(int Button, TMap<int, int> inHashTable, TArray<int>& PressedButtons)
{
	TArray<int> ButtonsMasks;
	inHashTable.GetKeys(ButtonsMasks);
	for (auto b : ButtonsMasks)
	{
		if (Button & b)
			PressedButtons.Add(*inHashTable.Find(b));
	}
}

FVector2D UGenericJoystickComponent::NormalizeJoyInput(FVector2D Input)
{
	FVector2D result = FVector2D::ZeroVector;
	if (Input.X == AXISHIGH)
		result.X = 1.f;
	else if (Input.X == AXISLOW)
		result.X = -1.f;

	if (Input.Y == AXISHIGH)
		result.Y = 1.f;
	else if (Input.Y == AXISLOW)
		result.Y = -1.f;


	return result;

}

void UGenericJoystickComponent::UnregisterAllComponents()
{
	RegisteredComponents.Empty();
}

void UGenericJoystickComponent::EnableJoyzInput()
{
	bJoyztickInputEnabled = true;
}

void UGenericJoystickComponent::DisableJoyzInput()
{
	bJoyztickInputEnabled = false;
}

void UGenericJoystickComponent::EnableJoyztickInput(AActor* Target)
{
	SetJoyztickInputStatus(true, Target);

}

void UGenericJoystickComponent::DisableJoyztickInput(AActor* Target)
{
	SetJoyztickInputStatus(false, Target);

}

void UGenericJoystickComponent::SetJoyztickInputStatus(bool bEnabled, AActor* Owner)
{
	if (!IsValid(Owner))
		return;

	auto joyztickComp = Owner->FindComponentByClass<UGenericJoystickComponent>();
	
	if(IsValid(joyztickComp))
		bEnabled ? joyztickComp->EnableJoyzInput() : joyztickComp->DisableJoyzInput();
}

void UGenericJoystickComponent::CheckReleasedButtons()
{
	TArray<int> ButtonsReleased;
	for (auto button : ButtonsPressedLastFrame)
	{
		if (!ButtonsPressedThisFrame.Contains(button))
			ButtonsReleased.AddUnique(button);
	}

	ButtonsPressedLastFrame = ButtonsPressedThisFrame;
	if (OnKeyRelease.IsBound())
		for(auto button : ButtonsReleased)
			OnKeyRelease.Broadcast(button);
}

void UGenericJoystickComponent::FreeResources()
{
	if (IsInitialized() && RegisteredComponents.Num() == 0)
	{
		UnregisterClassW(WINDOW_CLASS_NAME, hInstance);
		UE_LOG(LogJoyztick, Log, TEXT("Freeing Joyztick Resources"));
	}
	
}

