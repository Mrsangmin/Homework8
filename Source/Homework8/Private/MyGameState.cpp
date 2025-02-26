#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "MyGameInstance.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AMyGameState::AMyGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	CurrentWaveIndex = 0;
	MaxWaves = 3;
	WaveDuration = 20.0f;
	CoinsToSpawnPerWave = {20, 30, 40};
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&AMyGameState::UpdateHUD,
		0.1f,
		true
	);
}

void AMyGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
		}
	}

	CurrentWaveIndex = 0;
	StartWave();
}

void AMyGameState::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d 시작!"), CurrentWaveIndex + 1);
	//코인 카운트 리셋
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	//지난 Wave에서 스폰된 아이템 정리
	for (AActor* Item : CurrentWaveItems)
	{
		if (Item && Item->IsValidLowLevelFast())
		{
			Item->Destroy();
		}
	}

	//코인 스폰 개수 설정(CoinsToSpawnPerWave의 배열)
	int32 CoinsToSpawn = 20; //기본 값
	if (CoinsToSpawnPerWave.IsValidIndex(CurrentWaveIndex))
	{
		CoinsToSpawn = CoinsToSpawnPerWave[CurrentWaveIndex];
	}

	//코인 스폰
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < CoinsToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
					CurrentWaveItems.Add(SpawnedActor);
				}
			}
		}
	}
	//타이머 시작
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&AMyGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);
	UpdateHUD();

}

void AMyGameState::EndWave()
{
	//남아있는 Wave 타이머 제거
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	//Wave 증가
	CurrentWaveIndex++;
	//다음 Wave가 없으면 레벨 종료
	if (CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		//다음 Wave 시작
		StartWave();
	}
}

//시간 만료 시 호출
void AMyGameState::OnWaveTimeUp()
{
	//Wave가 끝났다고 판단하고 EndWave로 이동
	EndWave();
}

void AMyGameState::OnCoinCollected()
{
	TObjectPtr<AActor> Temp = this;
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		//모든 코인을 다 주웠으면 Wave 종료
		EndWave();
	}
}

void AMyGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex++;
			AddScore(Score);
			MyGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

int32 AMyGameState::GetScore() const
{
	return Score;
}

void AMyGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			Score += Amount;
			MyGameInstance->AddToScore(Amount);
		}
	}
	UpdateHUD();
}

void AMyGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->SetPause(true);
			MyPlayerController->ShowMainMenu(true);
		}
	}
}

void AMyGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
						if (MyGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), MyGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d / %d"), CurrentWaveIndex + 1, MaxWaves)));
				}
			}
		}
	}
}