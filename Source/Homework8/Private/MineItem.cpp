
#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GamePlayStatics.h" //UGameplayStatics 사용
#include "Particles/ParticleSystemComponent.h"


AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;

	ItemType = "Mine";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	Super::ActivateItem(Activator);

	//5초 후 폭발 실행
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay);
	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			//데미지를 발생시켜 Actor->TakeDamage()가 실행되도록 함
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}

	//지뢰 제거
	DestroyItem();

	if (Particle)
	{
		TWeakObjectPtr<UParticleSystemComponent> SmartParticle = Particle;
		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[SmartParticle]()
			{
				if (SmartParticle.IsValid())
				{
					SmartParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}