#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped					UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOnHandedWeapon		UMETA(DisplayName = "Equipped On-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon		UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unocuupied					UMETA(DisplayName = "Unocuupied"),
	EAS_Attacking					UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon				UMETA(DisplayName = "EquippingWeapon")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive						UMETA(DisplayName = "Alive"),
	EDP_Death						UMETA(DisplayName = "Death")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling					UMETA(DisplayName = "Patrolling"),
	EES_Chasing						UMETA(DisplayName = "Chasing"),
	EES_Attacking					UMETA(DisplayName = "Attacking")
};