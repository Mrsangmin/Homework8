#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class HOMEWORK8_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AMineItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* ExplosionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDamage;
	bool bHasExploded;
	//지뢰 발동 여부
	FTimerHandle ExplosionTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;

	void Explode();
};
