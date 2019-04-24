// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "GenericJoystickComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogJoyztick, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOYZTICK_API UGenericJoystickComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGenericJoystickComponent();
	~UGenericJoystickComponent();

	UFUNCTION(BlueprintPure)
		static bool IsInitialized();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic Joystick Config")
		bool bTriggerAxisMoveWhenAxisEqualsZero = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic Joystick Config")
		bool bJoyztickInputEnabled;

//Events
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoystickKeyChange, int, Button);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoystickMove, FVector2D, Axis);

	UPROPERTY(BlueprintAssignable)
		FJoystickKeyChange OnKeyPress;

	UPROPERTY(BlueprintAssignable)
		FJoystickKeyChange OnKeyDown;

	UPROPERTY(BlueprintAssignable)
		FJoystickKeyChange OnKeyRelease;

	UPROPERTY(BlueprintAssignable)
	FJoystickMove OnJoystickMove;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	void CreateWindowClass();
	void InitButtonsMap();
	static LRESULT CALLBACK WndProcGlobal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//Windows Callbacks & stuff
private:
	static bool InitInstance();
	static bool IsNotZero(FVector2D inVector);

	ATOM RegisterWindowClass();
	


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintPure)
		static FORCEINLINE  TArray<UGenericJoystickComponent*> GetRegisteredComponents() { return RegisteredComponents; };

	UFUNCTION(BlueprintPure)
		static void GetButtonsByMask(int Button, TMap<int,int> inHashTable, TArray<int>& ButtonsOut);

	UFUNCTION(BlueprintPure)
		static FVector2D NormalizeJoyInput(FVector2D Input);

	UFUNCTION(BlueprintCallable)
		void UnregisterAllComponents();


	UFUNCTION(BlueprintCallable, Category = "Joyzticks")
		void EnableJoyzInput();

	UFUNCTION(BlueprintCallable, Category = "Joyzticks")
		void DisableJoyzInput();


	UFUNCTION(BlueprintCallable, Category = "Joyzticks", meta=(WorldContext=Target))
		static void EnableJoyztickInput(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Joyzticks", meta = (WorldContext = Target))
		static void DisableJoyztickInput(AActor* Target);

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsJoyztickInputEnabled() const { return bJoyztickInputEnabled;  };

	static void SetJoyztickInputStatus(bool bEnabled, AActor* Owner);



	void CheckReleasedButtons();

protected:
	bool bStopThread;
	MSG msg;

protected:
	static HWND hWnd;
	static HINSTANCE hInstance;
	static TArray<UGenericJoystickComponent*> RegisteredComponents;
	static TMap<int, int> ButtonsMap;
	static TMap<int, int> ButtonsReleaseMap;
	static bool bCapturingJoystick;

	TArray<int> ButtonsPressedLastFrame;
	TArray<int> ButtonsPressedThisFrame;



private:
	//This is Triggered only when the first component is destroyed
	void FreeResources();

};
