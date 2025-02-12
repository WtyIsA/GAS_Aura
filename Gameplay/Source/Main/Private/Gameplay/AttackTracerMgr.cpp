// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTracerMgr.h"

#include "Animation/AnimInstance.h"
UAttackTracerMgr* UAttackTracerMgr::MgrHandle = nullptr;

UAttackTracerMgr* UAttackTracerMgr::Get()
{
	if (MgrHandle == nullptr)
	{
		MgrHandle = NewObject<UAttackTracerMgr>();
		MgrHandle->AddToRoot();
	}
	return MgrHandle;
}

void UAttackTracerMgr::AddBlockStuck(USkeletalMeshComponent* Mesh)
{
	if (Mesh)
	{
		InStuck.Add(Mesh);
	}
}

void UAttackTracerMgr::RemoveBlockStuck(USkeletalMeshComponent* Mesh)
{
	if (Mesh)
	{
		InStuck.Remove(Mesh);
	}
}

UAttackTracerInfo& UAttackTracerMgr::GetAttackTracerInfo(USkeletalMeshComponent* mesh)
{
	if (!AttackTracerInfoMap.Contains(mesh))
	{
		AttackTracerInfoMap.Add(mesh,NewObject<UAttackTracerInfo>());
	}
	return *AttackTracerInfoMap[mesh];
}

void UAttackTracerMgr::BeginStuck(float StuckTime, float StuckPlayRate, USkeletalMeshComponent* MeshComp,
                                  UAnimSequenceBase* Anim)
{
	if (InBounce.Contains(MeshComp))
	{
		return;
	}
	InStuck.Add(MeshComp);
	UAnimMontage* Montage = Cast<UAnimMontage>(Anim);
	MeshComp->GetAnimInstance()->Montage_SetPlayRate(Montage, StuckPlayRate);
	FTimerHandle Handle;
	MeshComp->GetOwner()->GetWorldTimerManager().SetTimer(Handle, [=]()
	{
		if (IsValid(Montage) &&IsValid(MeshComp))
		{
			MeshComp->GetAnimInstance()->Montage_SetPlayRate(Montage, 1);
		}
	}, StuckTime, false);
}

void UAttackTracerMgr::BeginBounce(float BounceTime, float BouncePlayRate,float BounceBlendOutTime, USkeletalMeshComponent* MeshComp,
                                   UAnimSequenceBase* Anim)
{
	if (InBounce.Contains(MeshComp))
	{
		return;
	}
	if (InStuck.Contains(MeshComp))
	{
		return;
	}
	InBounce.Add(MeshComp);
	UAnimMontage* Montage = Cast<UAnimMontage>(Anim);
	MeshComp->GetAnimInstance()->Montage_SetPlayRate(Montage, -BouncePlayRate);
	FTimerHandle Handle;
	MeshComp->GetOwner()->GetWorldTimerManager().SetTimer(Handle, [=]()
	{
		if (IsValid(Montage)&&IsValid(MeshComp))
		{
			MeshComp->GetAnimInstance()->Montage_Stop(BounceBlendOutTime, Montage);
			InBounce.Remove(MeshComp);
		}
	}, BounceTime, false);
}
