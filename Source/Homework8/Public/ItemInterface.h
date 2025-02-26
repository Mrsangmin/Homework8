// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HOMEWORK8_API IItemInterface
{
	GENERATED_BODY()

public:
	//지뢰, 힐링, 코인
	//힐링, 코인 = 즉시 발동 - 오버랩
	//지뢰 - 범위 내 오버랩 - 발동 5초뒤 폭발 - 오버랩 - 데미지

	//인터페이스면 거의 다 = 0을 붙여줘야함
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0; //= 0이 없으면 가상함수 형태라 오버라이드 할지 안할지를 마음대로 할 수 없음
	
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0;
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0; //타입형 알아낼 때 FName이 더 빠름

};
