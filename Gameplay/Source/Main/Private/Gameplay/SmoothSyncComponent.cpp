#include "Gameplay/SmoothSyncComponent.h"

#include "FootPrintComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

USmoothSyncComponent::USmoothSyncComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USmoothSyncComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	root = GetOwner()->GetRootComponent();
	bServer = GetWorld()->IsServer();
	bServer ? ServerBeginPlay() : ClientBeginPlay();
}

void USmoothSyncComponent::ClientBeginPlay()
{
	lastReceivedPos = root->GetComponentLocation();
	lastReceivedRotation = root->GetComponentRotation();
	lastFramePos = root->GetComponentLocation();
	UActorComponent* com = GetOwner()->GetComponentByClass(UMeshComponent::StaticClass());
	if (com)
	{
		CheckRenderComponent = Cast<UMeshComponent>(com);
	}
}

void USmoothSyncComponent::ServerBeginPlay()
{
	ServerFps = GEngine->FixedFrameRate;
	UActorComponent* foot = GetOwner()->GetComponentByClass(UFootPrintComponent::StaticClass());
	if (foot)
	{
		FootPrintComponent = Cast<UFootPrintComponent>(foot);
		FootPrintComponent->AddFootPrint(root->GetComponentLocation(), FPlatformTime::Seconds(), 0);
	}
}


FVector USmoothSyncComponent::GetVelocity()
{
	return curVelocity;
}

void USmoothSyncComponent::ServerSetRotationFollowVelocity_Implementation(bool follow)
{
	bRotationFollowVelocity = follow;
}

void USmoothSyncComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnable) return;

	bServer ? ServerTick(DeltaTime) : ClientTick(DeltaTime);
}

void USmoothSyncComponent::ServerTick(float DeltaTime)
{
	SendPos();
	if (bSyncRotation)
	{
		SendRotation();
	}
	if (DrawServerPath)
	{
		FVector curPos = root->GetComponentLocation();
		FVector velocity = GetOwner()->GetVelocity();
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), curPos, curPos + velocity * 0.1, FLinearColor::Blue, 1.5);
	}
}

void USmoothSyncComponent::SendPos()
{
	FVector curPos = root->GetComponentLocation();
	if (!SameVector(lastSendPos, curPos, 0.1))
	{
		lastSendPos = curPos;
		if (FootPrintComponent)
		{
			FootPrintComponent->AddFootPrint(curPos, FPlatformTime::Seconds(), 0);
		}
		ServerSendPosToEveryone(curPos);
	}
}

void USmoothSyncComponent::SendRotation()
{
	FVector curRotation = root->GetComponentRotation().Euler();
	if (curRotation.X < 0)
	{
		curRotation.X += 360;
	}
	if (curRotation.Y < 0)
	{
		curRotation.Y += 360;
	}
	if (curRotation.Z < 0)
	{
		curRotation.Z += 360;
	}
	if (!SameVector(lastSendRotaion, curRotation, 0.1))
	{
		lastSendRotaion = curRotation;
		ServerSendRotationToEveryone(uint16(curRotation.X * 100), uint16(curRotation.Y * 100),
		                             uint16(curRotation.Z * 100));
	}
}


void USmoothSyncComponent::ClientTick(float DeltaTime)
{
	ClientLerpPos(DeltaTime);
	if (lerpRotatorSpeed > 0)
	{
		ClientLerpRotation(DeltaTime);
	}
	else
	{
		if (bRotationFollowVelocity)
		{
			RotationFollowVelocity(DeltaTime);
		}
	}
}

void USmoothSyncComponent::ClientLerpPos(float DeltaTime)
{
	const FVector curPos = root->GetComponentLocation();
	if (SameVector(curPos, lastReceivedPos, 0.1))
	{
		curVelocity = FVector::ZeroVector;
	}
	else
	{
		root->SetWorldLocation(FMath::VInterpConstantTo(root->GetComponentLocation(), lastReceivedPos, DeltaTime,
		                                                lerpPosSpeed), bSweep);
	}
	lastFramePos = curPos;
}

void USmoothSyncComponent::ClientLerpRotation(float DeltaTime)
{
	const FRotator curRotator = root->GetComponentRotation();
	if (SameVector(curRotator.Vector(), lastReceivedRotation.Vector(), 0.01))
	{
		lerpRotatorSpeed = 0;
	}
	else
	{
		if (bUseFixedRotationSpeed)
		{
			lerpRotatorSpeed = FixedRotationSpeed;
			lerpRotatorSpeed = UKismetMathLibrary::DegreesToRadians(lerpRotatorSpeed);
		}
		root->SetWorldRotation(FMath::QInterpConstantTo(curRotator.Quaternion(), lastReceivedRotation.Quaternion(),
		                                                DeltaTime, lerpRotatorSpeed));
	}
}

void USmoothSyncComponent::RotationFollowVelocity(float DeltaTime)
{
	const FRotator curRotator = root->GetComponentRotation();
	FRotator velocityRotator = curVelocity.Rotation();
	LimitRotation360(velocityRotator);

	if (bRotationFollowVelocityIgnoreRoll)
	{
		velocityRotator.Roll = 0;
	}
	if (bRotationFollowVelocityIgnorePitch)
	{
		velocityRotator.Pitch = 0;
	}
	if (bRotationFollowVelocityIgnoreYaw)
	{
		velocityRotator.Yaw = 0;
	}
	if (bFixSlope)
	{
		velocityRotator.Pitch = lastReceivedRotation.Pitch;
	}
	FQuat TargetQuat = curVelocity.SizeSquared() > 0.01
		                   ? velocityRotator.Quaternion()
		                   :lastReceivedRotation.Quaternion();
	root->SetWorldRotation(FMath::QInterpConstantTo(curRotator.Quaternion(), TargetQuat,
	                                                DeltaTime, UKismetMathLibrary::DegreesToRadians(FixedRotationSpeed))
		.Rotator());
	lastReceivedRotation = root->GetComponentRotation();
}


void USmoothSyncComponent::ServerSendPosToEveryone_Implementation(FVector pos, bool bLerp)
{
	if (bServer)
	{
		return;
	}
	if (root == nullptr)
	{
		return;
	}
	lastReceivedPos = pos;
	bool bRendered = CheckRenderComponent && CheckRenderComponent->WasRecentlyRendered();
	if (bStickGround && bRendered)
	{
		FHitResult OutHit;
		const ECollisionChannel CollisionChannel = root->GetCollisionObjectType();
		const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ServerSendPosToEveryoneId), false, GetOwner());
		GetWorld()->LineTraceSingleByChannel(OutHit, pos, pos + FVector::DownVector * 200, CollisionChannel,
		                                     QueryParams);
		//DrawDebugLine(GetWorld(),pos,pos+FVector::DownVector*200,FColor::Orange,false,10);
		if (OutHit.bBlockingHit && OutHit.Component->GetCollisionObjectType() == ECC_WorldStatic)
		{
			lastReceivedPos.Z = OutHit.ImpactPoint.Z + Height * GetOwner()->GetActorScale().Z;
		}
		else
		{
			lastReceivedRotation.Pitch = 0;
		}
		if (bFixSlope)
		{
			float pitch;
			float roll;
			UKismetMathLibrary::GetSlopeDegreeAngles(root->GetRightVector(), OutHit.Normal, FVector::UpVector, pitch,
			                                         roll);
			if (UKismetMathLibrary::Abs(pitch) > 30)
			{
				pitch = pitch > 0 ? 30 : -30;
			}
			lastReceivedRotation.Pitch = pitch;
		}
	}

	bReachRecivedPos = false;
	CalculateLerpVelocity();
	if (bExtrapolation)
	{
		lastReceivedPos += curVelocity * (ExtrapolationTime + serverTickTime);
	}
	if (!bRendered || !bLerp)
	{
		if (bCanSetPosImmediately)
		{
			root->SetWorldLocation(lastReceivedPos);
		}
	}
}

void USmoothSyncComponent::CalculateLerpVelocity()
{
	if (root)
	{
		const FVector Delta = lastReceivedPos - root->GetComponentLocation();
		curVelocity = Delta / (serverTickTime + AllowNetDelay);
		lerpPosSpeed = Delta.Size() / (serverTickTime + AllowNetDelay);
	}
}

void USmoothSyncComponent::LimitRotation360(FRotator& rotator)
{
	if (rotator.Roll < 0)
	{
		rotator.Roll += 360;
	}
	if (rotator.Yaw < 0)
	{
		rotator.Yaw += 360;
	}
	if (rotator.Pitch < 0)
	{
		rotator.Pitch += 360;
	}
}


void USmoothSyncComponent::ServerSendRotationToEveryone_Implementation(uint16 roll, uint16 pitch, uint16 yaw)
{
	if (bServer)
	{
		return;
	}
	if (root == nullptr)
	{
		return;
	}
	FRotator r;
	r.Roll = roll / 100;
	r.Pitch = pitch / 100;
	r.Yaw = yaw / 100;
	if(bFixSlope)
	{
		r.Pitch = lastReceivedRotation.Pitch;
	}
	lastReceivedRotation = r;
	if (root)
	{
		const FQuat curRotator = root->GetComponentTransform().GetRotation();
		lerpRotatorSpeed = curRotator.AngularDistance(lastReceivedRotation.Quaternion()) / (RotationLerpTime * 1.2f);
	}
	if (CheckRenderComponent && !CheckRenderComponent->WasRecentlyRendered())
	{
		root->SetWorldRotation(lastReceivedRotation);
	}
}


void USmoothSyncComponent::ServerSendResetPosToEveryone_Implementation(FVector pos, bool bLerp)
{
	if (root)
	{
		root->SetWorldLocation(pos);
		if (!bServer)
		{
			ServerSendPosToEveryone_Implementation(pos, bLerp);
		}
	}
}


void USmoothSyncComponent::SetEnable(bool enable)
{
	bEnable = enable;
	if (!bEnable)
	{
		lastReceivedPos = root->GetComponentLocation();
		lastReceivedRotation = root->GetComponentRotation();
	}
}

void USmoothSyncComponent::SetLastReceivedPos(FVector pos)
{
	lastReceivedPos = pos;
}

bool USmoothSyncComponent::SameVector(FVector one, FVector two, float threshold)
{
	if (FMath::Abs(one.X - two.X) > threshold) return false;
	if (FMath::Abs(one.Y - two.Y) > threshold) return false;
	if (FMath::Abs(one.Z - two.Z) > threshold) return false;
	return true;
}


void USmoothSyncComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USmoothSyncComponent, ServerFps, COND_InitialOnly);
	DOREPLIFETIME(USmoothSyncComponent, bRotationFollowVelocity);
}

void USmoothSyncComponent::OnRep_ServerFps()
{
	serverTickTime = 1 / ServerFps;
}
