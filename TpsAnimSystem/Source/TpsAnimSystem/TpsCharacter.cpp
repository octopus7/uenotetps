#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"

ATpsCharacter::ATpsCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Components
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritRoll = false;
    CameraBoom->bInheritYaw = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    HpText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HpText"));
    HpText->SetupAttachment(RootComponent);
    HpText->SetHorizontalAlignment(EHTA_Center);
    HpText->SetVerticalAlignment(EVRTA_TextCenter);
    HpText->SetWorldSize(24.f);
    HpText->SetTextRenderColor(FColor::White);
    HpText->SetVisibility(true, true);

    // Character rotation and movement defaults (typical TPS)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = true; // Rotate toward movement input
        MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
        MoveComp->JumpZVelocity = 600.f;
        MoveComp->AirControl = 0.35f;
        MoveComp->MaxWalkSpeed = WalkSpeed;
        MoveComp->MinAnalogWalkSpeed = 20.f;
        MoveComp->BrakingDecelerationWalking = 2048.f;
        MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
    }
}

void ATpsCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitialSpawnTransform = GetActorTransform();

    // Clamp health to max on begin
    Health = FMath::Clamp(Health, 0.f, MaxHealth);

    // Place HP text above head
    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        const float Z = Capsule->GetScaledCapsuleHalfHeight() + HpTextOffsetZ;
        HpText->SetRelativeLocation(FVector(0.f, 0.f, Z));
    }
    UpdateHpText();

    // Ensure walk speed applied (in case defaults were changed in BP)
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }

    // Add Mapping Context if provided (assets set via Blueprint)
    if (DefaultMappingContext)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ULocalPlayer* LP = PC->GetLocalPlayer())
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
                {
                    Subsys->AddMappingContext(DefaultMappingContext, InputMappingPriority);
                }
            }
        }
    }
}

void ATpsCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Rotate HP text around Z to face camera
    if (HpText)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            const float YawFacing = FRotator::NormalizeAxis(PC->GetControlRotation().Yaw + 180.f);
            HpText->SetWorldRotation(FRotator(0.f, YawFacing, 0.f));
        }
    }

    // Track fall start height for fall-damage
    if (const UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        const bool bFalling = MoveComp->IsFalling();
        if (bFalling && !bTrackingFall)
        {
            bTrackingFall = true;
            FallStartZ = GetActorLocation().Z;
        }
        else if (!bFalling && bTrackingFall)
        {
            // Safety: in case Landed wasn't called (edge), finalize here using current Z
            const float FallHeight = FMath::Max(0.f, FallStartZ - GetActorLocation().Z);
            if (bEnableFallDamage && !bIsDead)
            {
                if (FallHeight > MinDamageHeight)
                {
                    const float Alpha = FMath::Clamp((FallHeight - MinDamageHeight) / FMath::Max(1.f, (MaxDamageHeight - MinDamageHeight)), 0.f, 1.f);
                    const float Damage = MaxFallDamage * Alpha;
                    if (Damage > 0.f)
                    {
                        ApplyDamage(Damage);
                    }
                }
            }
            bTrackingFall = false;
        }
    }

    if (bIsDead || !bEnableFallDeath)
    {
        return;
    }

    const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp && MoveComp->IsFalling())
    {
        // Count only when actually moving downward to avoid counting rising jump time.
        if (GetVelocity().Z <= 0.f)
        {
            CurrentFallTime += DeltaSeconds;
            if (CurrentFallTime >= FallDeathTimeThreshold)
            {
                HandleDeath();
            }
        }
    }
    else
    {
        CurrentFallTime = 0.f;
    }
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATpsCharacter::Input_Move);
        }

        if (LookAction)
        {
            EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATpsCharacter::Input_Look);
        }

        if (JumpAction)
        {
            EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ATpsCharacter::Input_JumpPressed);
            EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATpsCharacter::Input_JumpReleased);
            EIC->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ATpsCharacter::Input_JumpReleased);
        }

        if (SprintAction)
        {
            EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ATpsCharacter::Input_SprintPressed);
            EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATpsCharacter::Input_SprintReleased);
            EIC->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ATpsCharacter::Input_SprintReleased);
        }

        if (CrouchAction)
        {
            EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATpsCharacter::Input_CrouchToggle);
        }
    }
}

void ATpsCharacter::Input_Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (!Controller)
    {
        return;
    }

    const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    if (Axis.Y != 0.f)
    {
        AddMovementInput(ForwardDir, Axis.Y);
    }
    if (Axis.X != 0.f)
    {
        AddMovementInput(RightDir, Axis.X);
    }
}

void ATpsCharacter::Input_Look(const FInputActionValue& Value)
{
    const FVector2D Look = Value.Get<FVector2D>();
    if (!Controller)
    {
        return;
    }

    AddControllerYawInput(Look.X);
    AddControllerPitchInput(Look.Y);
}

void ATpsCharacter::Input_JumpPressed()
{
    Jump();
}

void ATpsCharacter::Input_JumpReleased()
{
    StopJumping();
}

void ATpsCharacter::ApplySprint(bool bEnable)
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        bIsSprinting = bEnable;
        MoveComp->MaxWalkSpeed = bEnable ? SprintSpeed : WalkSpeed;
    }
}

void ATpsCharacter::Input_SprintPressed()
{
    if (bSprintToggle)
    {
        ApplySprint(!bIsSprinting);
    }
    else
    {
        ApplySprint(true);
    }
}

void ATpsCharacter::Input_SprintReleased()
{
    if (!bSprintToggle)
    {
        ApplySprint(false);
    }
}

void ATpsCharacter::Input_CrouchToggle()
{
    if (GetCharacterMovement()->IsCrouching())
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }
}

void ATpsCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (!bTrackingFall || bIsDead)
    {
        bTrackingFall = false;
        return;
    }

    const float LandZ = GetActorLocation().Z;
    const float FallHeight = FMath::Max(0.f, FallStartZ - LandZ);

    if (bEnableFallDamage)
    {
        if (FallHeight > MinDamageHeight)
        {
            const float Alpha = FMath::Clamp((FallHeight - MinDamageHeight) / FMath::Max(1.f, (MaxDamageHeight - MinDamageHeight)), 0.f, 1.f);
            const float Damage = MaxFallDamage * Alpha;
            if (Damage > 0.f)
            {
                ApplyDamage(Damage);
            }
        }
    }

    bTrackingFall = false;
}

void ATpsCharacter::HandleDeath()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
        MoveComp->SetMovementMode(EMovementMode::MOVE_None);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreLookInput(true);
        PC->SetIgnoreMoveInput(true);
        FInputModeUIOnly UIOnly;
        UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(UIOnly);
        PC->bShowMouseCursor = true;
    }

    ShowDeathUI();
    OnDied();
}

void ATpsCharacter::ShowDeathUI()
{
    if (DeathWidgetInstance || !DeathWidgetClass)
    {
        return;
    }
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DeathWidgetInstance = CreateWidget<UUserWidget>(PC, DeathWidgetClass);
        if (DeathWidgetInstance)
        {
            DeathWidgetInstance->AddToViewport(1000);
        }
    }
}

void ATpsCharacter::HideDeathUI()
{
    if (DeathWidgetInstance)
    {
        DeathWidgetInstance->RemoveFromParent();
        DeathWidgetInstance = nullptr;
    }
}

void ATpsCharacter::RequestRespawn()
{
    if (!bIsDead)
    {
        return;
    }

    HideDeathUI();

    SetActorTransform(InitialSpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
        MoveComp->StopMovementImmediately();
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }

    CurrentFallTime = 0.f;
    bIsDead = false;

    // Restore health
    const float OldHealth = Health;
    Health = MaxHealth;
    OnHealthChanged(Health, Health - OldHealth);
    UpdateHpText();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreLookInput(false);
        PC->SetIgnoreMoveInput(false);
        FInputModeGameOnly GameOnly;
        PC->SetInputMode(GameOnly);
        PC->bShowMouseCursor = false;
    }

    ApplySprint(false);
    OnRespawned();
}

void ATpsCharacter::ApplyDamage(float Amount)
{
    if (bIsDead || Amount <= 0.f)
    {
        return;
    }
    const float Prev = Health;
    Health = FMath::Clamp(Health - Amount, 0.f, MaxHealth);
    OnHealthChanged(Health, Health - Prev);
    UpdateHpText();
    if (Health <= 0.f)
    {
        HandleDeath();
    }
}

void ATpsCharacter::UpdateHpText()
{
    if (!HpText)
    {
        return;
    }
    const int32 Cur = FMath::RoundToInt(Health);
    const int32 Max = FMath::Max(1, FMath::RoundToInt(MaxHealth));
    HpText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Cur, Max)));
}
