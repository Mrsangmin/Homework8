#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

UCLASS()
class HOMEWORK8_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<int32> CoinsToSpawnPerWave;


	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	//전역 점수를 저장하는 변수 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	//현재 점수를 읽는 함수
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	//점수를 추가해주는 함수
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	void OnGameOver();
	void OnCoinCollected();

	void StartWave();
	void EndWave();
	void OnWaveTimeUp();

	void StartLevel();
	void EndLevel();
	void UpdateHUD();

private:
	UPROPERTY()
	TArray<AActor*> CurrentWaveItems;
};
