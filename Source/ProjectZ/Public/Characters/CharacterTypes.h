#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped					UMETA(DisplayName = "Unequipped"),
	ECS_OneHandedWeapon				UMETA(DisplayName = "One-Handed"),
	ECS_TwoHandedWeapon				UMETA(DisplayName = "Two-Handed"),
	ECS_GunWeapon					UMETA(DisplayName = "Gun")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unocuupied					UMETA(DisplayName = "Unocuupied"),
	EAS_HitReaction					UMETA(DisplayName = "HitReaction"),
	EAS_Attacking					UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon				UMETA(DisplayName = "EquippingWeapon"),
	EAS_Dodge						UMETA(DisplayName = "Dodge"),
	EAS_Sprint						UMETA(DisplayName = "Sprint"),
	EAS_Dead						UMETA(DisplayName = "Dead"),
	EAS_UI							UMETA(DisplayName = "UI")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death						UMETA(DisplayName = "Death"),
	EDP_MAX							UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState						UMETA(DisplayName = "NoState"),

	EES_Patrolling					UMETA(DisplayName = "Patrolling"),
	EES_Dead						UMETA(DisplayName = "Dead"),
	EES_Chasing						UMETA(DisplayName = "Chasing"),
	EES_Attacking					UMETA(DisplayName = "Attacking"),
	EES_Engaged						UMETA(DisplayName = "Engaged")
};

UENUM(BlueprintType)
enum class EEnemyGrade : uint8
{
	EEG_Normal						UMETA(DisplayName = "Normal"),
	EEG_Boss						UMETA(DisplayName = "Boss")


};

namespace WalkSpeed
{
	const float Walk = 450.f;
	const float Run = 720.f;
};

namespace TimerRate
{
	const float AutomaticTrailRate = 0.05f;
	const float SprintRate = 0.025f;
};