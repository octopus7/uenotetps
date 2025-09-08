#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;

#include "TpsCharacter.generated.h"

/**
 * Third-person character with standard movement and camera.
 * - Uses Enhanced Input (assets assigned via Blueprint; no asset creation in code)
 * - Provides Move, Look, Jump, Sprint (hold/toggle), and Crouch toggle
 */
UCLASS(Blueprintable, BlueprintType)
class TPSANIMSYSTEM_API ATpsCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATpsCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void Landed(const FHitResult& Hit) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    /* Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    /* Enhanced Input: Assets assigned in Blueprint (do not create in code) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction = nullptr;     // Axis2D

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction = nullptr;     // Axis2D

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions", meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction = nullptr;     // Digital (bool)

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions", meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction = nullptr;   // Digital (bool)

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions", meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction = nullptr;   // Digital (bool)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
    int32 InputMappingPriority = 0;

    /* Movement tuning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", Units = "cm/s"))
    float WalkSpeed = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", Units = "cm/s"))
    float SprintSpeed = 650.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
    bool bSprintToggle = false; // true = toggle; false = hold

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
    bool bIsSprinting = false;

    /* Fall death */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death|Fall", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", UIMin = "0.1", Units = "s"))
    float FallDeathTimeThreshold = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death|Fall", meta = (AllowPrivateAccess = "true"))
    bool bEnableFallDeath = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death|State", meta = (AllowPrivateAccess = "true"))
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death|State", meta = (AllowPrivateAccess = "true", Units = "s"))
    float CurrentFallTime = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death|Respawn", meta = (AllowPrivateAccess = "true"))
    FTransform InitialSpawnTransform;

    /* Death UI (assigned in BP) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> DeathWidgetClass;

    UPROPERTY(Transient)
    UUserWidget* DeathWidgetInstance = nullptr;

    /* Fall damage + Health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", UIMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true"))
    bool bEnableFallDamage = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", Units = "cm"))
    float MinDamageHeight = 600.0f; // below this height: no damage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true", ClampMin = "1.0", UIMin = "1.0", Units = "cm"))
    float MaxDamageHeight = 2000.0f; // at/above: max damage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
    float MaxFallDamage = 100.0f; // maps to Health scale by default

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true", Units = "cm"))
    float FallStartZ = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage|Fall", meta = (AllowPrivateAccess = "true"))
    bool bTrackingFall = false;

private:
    /* Input handlers */
    void Input_Move(const FInputActionValue& Value);
    void Input_Look(const FInputActionValue& Value);
    void Input_JumpPressed();
    void Input_JumpReleased();
    void Input_SprintPressed();
    void Input_SprintReleased();
    void Input_CrouchToggle();

    void ApplySprint(bool bEnable);

    /* Death/Respawn helpers */
    void HandleDeath();
    void ShowDeathUI();
    void HideDeathUI();

public:
    UFUNCTION(BlueprintCallable, Category = "Death")
    void RequestRespawn();

    UFUNCTION(BlueprintImplementableEvent, Category = "Death")
    void OnDied();

    UFUNCTION(BlueprintImplementableEvent, Category = "Death")
    void OnRespawned();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthNormalized() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

    UFUNCTION(BlueprintImplementableEvent, Category = "Health")
    void OnHealthChanged(float NewHealth, float Delta);
};
