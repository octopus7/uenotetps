#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

ATpsCharacter::ATpsCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

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
