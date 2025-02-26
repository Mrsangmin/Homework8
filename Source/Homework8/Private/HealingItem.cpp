// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "MyCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if(AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			//캐릭터의 체력 회복
			PlayerCharacter->AddHealth(HealAmount);
		}

		DestroyItem();
	}
}