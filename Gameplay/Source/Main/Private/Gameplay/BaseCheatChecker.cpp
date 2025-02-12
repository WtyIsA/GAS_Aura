#include "BaseCheatChecker.h"

#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Gameplay/SyncMoveComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UBaseCheatChecker::UBaseCheatChecker()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseCheatChecker::BeginPlay()
{
	Super::BeginPlay();
	PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	IgnoreActors.Add(GetOwner());
}

void UBaseCheatChecker::SelfPullBacked()
{
}


void UBaseCheatChecker::SetCurState(EStateType state)
{
	lastState = curState;
	curState = state;
}

bool UBaseCheatChecker::IsTargetPosFly(FVector targetPos,EStateType state)
{
	
	if (state == EStateType::InAir)
	{
		return false;
	}
	bool hasGround =false;
	FVector groundPos = GetGroundPos(targetPos,hasGround);
	if (hasGround&&(targetPos.Z - groundPos.Z) < 200)
	{
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TargetPosFly,CurState:%d,  targetPos:  %s  groundPos:%s "), state,*targetPos.ToString(), *groundPos.ToString())
	return true;
}

bool UBaseCheatChecker::IsAirPosOk(FVector targetPos, double timestamp,EStateType state)
{
	if (state == EStateType::InAir)
	{
		double flyTime = timestamp - enterAirTimeStamp;
		
		FVector pos =GetFlyPos(enterAirPos,enterAirVelocity,flyTime);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), targetPos, FColor::Blue,
		              false, 1, 0, 1);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), pos, FColor::Green,
		              false, 1, 0, 1);
		float zOffset = targetPos.Z - pos.Z;
		//UE_LOG(LogTemp,Warning,TEXT(" ZOffset:%f, server z:%f, target z:%f"),zOffset,pos.Z,targetPos.Z)
		if (zOffset > 100)
		{
			UE_LOG(LogTemp,Warning,TEXT("Air Pos Error ZOffset:%f, server z:%f, target z:%f"),zOffset,pos.Z,targetPos.Z)
			return false;
		}
	}
	return true;
}

bool UBaseCheatChecker::IsCollisionRadiusChange()
{
	return false;
}

bool UBaseCheatChecker::IsClimbPosOk(FVector targetPos, EStateType state)
{
	return true;
}

bool UBaseCheatChecker::IsLadderPosOk(FVector targetPos, EStateType state)
{
	return true;
}

bool UBaseCheatChecker::IsCurvePosOk(FVector targetPos, EStateType state, double timestamp)
{
	return true;
}

FVector UBaseCheatChecker::GetGroundPos(FVector pos,bool& hasGround)
{
	FHitResult OutHit;
	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(this, pos + FVector::UpVector * 10,
	                                                       pos + FVector::DownVector * 10000,
	                                                       PrimitiveComponent->Bounds.BoxExtent * 0.5 * 0.7,
	                                                       PrimitiveComponent->GetComponentRotation(),
	                                                       UEngineTypes::ConvertToTraceType(ECC_Visibility),
	                                                       false,
	                                                       IgnoreActors
	                                                       , EDrawDebugTrace::None, OutHit
	                                                       , true
	                                                       , FLinearColor::White, FLinearColor::Red
	                                                       , 5.f);

	if (bHit)
	{
		hasGround =true;
		return OutHit.ImpactPoint;
	}
	hasGround =false;
	return PrimitiveComponent->GetComponentLocation();
}

FVector UBaseCheatChecker::GetFlyPos(const FVector& StartPos, const FVector& Speed, const double FlyTime) const
{
	FVector EndPos;
	EndPos.X = StartPos.X + Speed.X * FlyTime;
	EndPos.Y = StartPos.Y + Speed.Y * FlyTime;
	EndPos.Z = StartPos.Z + Speed.Z * FlyTime + 0.5f *-980 * GravityScale * FlyTime * FlyTime;
	return EndPos;
}

void UBaseCheatChecker::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UBaseCheatChecker::PassSwimStateCheck(EStateType state)
{
	if (!bCheckSwim)
	{
		return true;
	}
	if (!SyncMoveComponent)
	{
		return false;
	}
	FHitResult OutHit;
	FVector objPos = SyncMoveComponent->GetRootPos();
	const bool bHit = UKismetSystemLibrary::LineTraceSingleByProfile(this,objPos + FVector::UpVector * 4000 ,objPos+ FVector::DownVector*250,
															"WaterTrace",
															false, IgnoreActors
															, EDrawDebugTrace::None, OutHit
															, true);
	if (!bHit)
	{
		if (state==EStateType::Swim)
		{
			UE_LOG(LogTemp,Warning,TEXT("No Water But state==EStateType::Swim  CurPos:"),*objPos.ToString())
			return false;
		}
		return true;
	}
	else
	{
		if (state==EStateType::Swim&&(objPos.Z-OutHit.ImpactPoint.Z) < 120)
		{
			return true;
		}
		if ( state != EStateType::Swim&&(objPos.Z-OutHit.ImpactPoint.Z) > -EnterSwimDepth)
		{
			return true;
		}
	}
	UE_LOG(LogTemp,Warning,TEXT(" Not Pos Swim Check DIS: %f "),objPos.Z-OutHit.ImpactPoint.Z);
	return false;
}

void UBaseCheatChecker::SetMaxSpeed(float speed)
{
	MaxSpeed = speed * 1.1;
}

void UBaseCheatChecker::SetXMoveCurve(UCurveFloat* curve)
{
	xMoveCurve = curve;
}

void UBaseCheatChecker::SetYMoveCurve(UCurveFloat* curve)
{
	yMoveCurve = curve;
}

void UBaseCheatChecker::SetZMoveCurve(UCurveFloat* curve)
{
	zMoveCurve = curve;
}
