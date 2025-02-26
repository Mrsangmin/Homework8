// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h" // 우리가 정의한 구조체
#include "SpawnVolume.generated.h"

class UBoxComponent;


UCLASS()
class HOMEWORK8_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	//스폰 영역을 담당할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	//스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem(); // 리턴 형식을 AActor* 로 변경

	//특정 아이템 클래스를 스폰하는 함수
	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	FVector GetRandomPointInVolume() const;
};	
