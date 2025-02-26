#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetInstance(nullptr),
	HUDWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr)

{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//현재 PlayerController에 연결된 Local Player 객체를 가져옴
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		//Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// Subsystem을 통해 우리가 할당한 IMC를 활성화
				// 우선순위(Priorty)는 0이 가장 높은 순위
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

UUserWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMyPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock *ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"), MyGameInstance->TotalScore)
					));
				}
			}
		}
	}
}

void AMyPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
		if (MyGameState)
		{
			MyGameState->UpdateHUD();
		}
	}
}

void AMyPlayerController::StartGame()
{
	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MyGameInstance->CurrentLevelIndex = 0;
		MyGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void AMyPlayerController::ExitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		this,
		EQuitPreference::Quit,
		false
	);
}