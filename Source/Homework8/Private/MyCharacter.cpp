// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyGameState.h"
#include "EnhancedInputComponent.h"
//ī�޶�, �������� ���� ������ �ʿ��� ���� include
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//������ �� ����
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// ������ ���� ��Ʈ ������Ʈ��(CapsulComponent) ����
	SpringArmComp->SetupAttachment(RootComponent);
	//ĳ���Ϳ� ī�޶� ������ �Ÿ� ����
	SpringArmComp->TargetArmLength = 300.0f;
	//��Ʈ�ѷ� ȸ���� ���� ������ �ϵ� ȸ��
	SpringArmComp->bUsePawnControlRotation = true;

	//ī�޶� ������Ʈ ����
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//������ ���� ���� ��ġ�� ī�޶� ����
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	//ī�޶�� ������ ���� ȸ���� �����Ƿ� PawnControlRotation�� ����
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 250.0f;
	SprintSpeedMultiplier = 2.0f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	//�ʱ� ü�� ����
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}


// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Enhanced InputConponent �� ĳ����
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				//IA_Move �׼� Ű�� "Ű�� ������ �ִ� ����" Move() ȣ��
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				//IA_Jump �׼� Ű�� "Ű�� ������ �ִ� ����" StartJump() ȣ��
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::StartJump
				);
				//IA_Jump �׼� Ű���� "���� �� ����" StopJump() ȣ��
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AMyCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				//IA_Look �׼� ���콺�� "������ ��" Look() ȣ��
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				//IA_Sprint �׼� Ű�� "Ű�� ������ �ִ� ����" StartSprint() ȣ��
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::StartSprint
				);
				//IA_Sprint �׼� Ű���� "���� �� ����" StopSprint() ȣ��
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AMyCharacter::StopSprint
				);
			}
		}
	}
}
void AMyCharacter::Move(const FInputActionValue& value)
{
	//��Ʈ�ѷ��� �־�� ���� ��� ����
	if (!Controller) return;

	//Value�� Axis2D�� ������ IA_Move�� �Է°� (WASD)�� ��� ����
	// ��) (X=1, Y=0) �� ���� / (X=-1, Y=0) �� ���� / (X=0, Y=1) �� ������ / (X=0, Y=-1) �� ����
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		//ĳ���Ͱ� �ٶ󺸴� ����(����)���� X�� �̵�
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// ĳ������ ������ �������� Y�� �̵�
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMyCharacter::StartJump(const FInputActionValue& value)
{
	// Jump �Լ��� Character�� �⺻ ����
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AMyCharacter::StopJump(const FInputActionValue& value)
{
	// StopJumping �Լ��� Character�� �⺻ ����
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AMyCharacter::Look(const FInputActionValue& value)
{
	// ���콺�� X, Y �������� 2D ������ ������
	FVector2D LookInput = value.Get<FVector2D>();

	// X�� �¿� ȸ�� (Yaw), Y�� ���� ȸ�� (Pitch)
	// �¿� ȸ��
	AddControllerYawInput(LookInput.X);
	// ���� ȸ��
	AddControllerPitchInput(LookInput.Y);
}

void AMyCharacter::StartSprint(const FInputActionValue& value)
{
	// Shift Ű�� ���� ���� �� �Լ��� ȣ��ȴٰ� ����
	// ������Ʈ �ӵ��� ����
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AMyCharacter::StopSprint(const FInputActionValue& value)
{
	// Shift Ű�� �� ���� �� �Լ��� ȣ��
	// ���� �ӵ��� ����
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

float AMyCharacter::GetHealth() const
{
	return Health;
}

//ü�� ȸ�� �Լ�
void AMyCharacter::AddHealth(float Amount)
{
	//ü���� ȸ����Ŵ. �ִ� ü���� �ʰ����� �ʵ��� ����
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health increased to %f"), Health);
	UpdateOverheadHP();
}

//������ ó�� �Լ�
float AMyCharacter::TakeDamage(
	float DamageAmount, 
	struct FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser)
{
	//�⺻ ������ ó�� ���� ȣ�� (�ʼ� �ƴ�)
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//ü���� ��������ŭ ���ҽ�Ű�� 0 ���Ϸ� �������� �ʵ��� Clamp
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health decreased to %f"), Health);
	UpdateOverheadHP();

	//ü���� 0 ���ϰ� �Ǹ� ��� ó��
	if (Health <= 0.0f)
	{
		OnDeath();
	}

	//���� ����� �������� ��ȯ
	return ActualDamage;
}

//��� ó�� �Լ�
void AMyCharacter::OnDeath()
{
	UE_LOG(LogTemp, Error, TEXT("Character is Dead"));
	AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
	if (MyGameState)
	{
		MyGameState->OnGameOver();
	}
	//��� �� ����
}

void AMyCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}