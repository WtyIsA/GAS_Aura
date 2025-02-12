// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/ActorRandom.h"
#if WITH_EDITOR
#include "Editor/UnrealEdEngine.h"
#endif

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#if WITH_EDITOR
#include "Misc/FileHelper.h"
#endif

// Sets default values
AActorRandom::AActorRandom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
#if WITH_EDITORONLY_DATA
	SpriteScale = 0.f;
#endif
}

void AActorRandom::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	CheckIds();
#endif
}

FString AActorRandom::GetModuleName_Implementation() const
{
	return FString("Share.ActorRandom");
}

// Called when the game starts or when spawned
void AActorRandom::BeginPlay()
{
	Super::BeginPlay();
	Random();
}

#if WITH_EDITOR
void AActorRandom::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UE_LOG(LogTemp,Log,TEXT("name : %s, type : %u"),*PropertyChangedEvent.Property->GetName(),PropertyChangedEvent.ChangeType)
	if (PropertyChangedEvent.Property->GetName().Equals("RandomActors"))
	{
		switch (PropertyChangedEvent.ChangeType)
		{
		case EPropertyChangeType::Redirected:
		case EPropertyChangeType::ArrayAdd:
		case EPropertyChangeType::ArrayClear:
		case EPropertyChangeType::ArrayRemove:
		case EPropertyChangeType::ValueSet:
			Random();
			break;
		default:
			break;
		}
	}
}

void AActorRandom::PostActorCreated()
{
	Super::PostActorCreated();
	Random();
}
#endif

void AActorRandom::DestroySelf()
{
	Destroy(true);
}

void AActorRandom::DestroySelfAndAllChildren()
{
	DestroyChild(this);
}

void AActorRandom::NotifyDestroyed()
{
	Destroy(true);
	if (RandomActors.Num() > 0)
	{
		for (auto element : RandomActors)
		{
			DestroyChild(element);
		}
	}

	if (PerpetualActors.Num() > 0)
	{
		for (auto element : PerpetualActors)
		{
			DestroyChild(element);
		}
	}
}

void AActorRandom::DestroyChild(AActor* Child)
{
	if (Child)
	{
		if (AActorRandom* Item = Cast<AActorRandom>(Child))
		{
			Item->NotifyDestroyed();
		}
		else
		{
			Child->Destroy(true);
		}
	}
}

#if WITH_EDITOR
void AActorRandom::NotifyHiddenEdChange(bool bCurHidden)
{
	SetIsTemporarilyHiddenInEditor(bCurHidden);
	if (RandomActors.Num() > 0)
	{
		for (auto element : RandomActors)
		{
			EdHideChildChange(element,bCurHidden);
		}
	}

	if (PerpetualActors.Num() > 0)
	{
		for (auto element : PerpetualActors)
		{
			EdHideChildChange(element,bCurHidden);
		}
	}
}

void AActorRandom::CheckIds()
{
#if WITH_EDITOR
	if (ConfigPath.IsEmpty())
		return;
	
	FString CfgString;
	if (!FFileHelper::LoadFileToString(CfgString, *ConfigPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load file %s."), *ConfigPath);
		return;
	}
	
	CfgString.TrimEndInline();
	TArray<FString> LinesToProcess;
	CfgString.ParseIntoArray(LinesToProcess, TEXT("\r\n"), false);
	uint32 NumFileLines = LinesToProcess.Num();

	TArray<int> CfgIds;
	for (uint32 LineIdx = 1; LineIdx < NumFileLines; ++LineIdx)
	{
		FString CurLine = LinesToProcess[LineIdx];
		TArray<FString> ValuesToProcess;
		CurLine.ParseIntoArray(ValuesToProcess, TEXT("\t"), false);

		if (ValuesToProcess.Num() > 0)
		{
			CfgIds.Add(FCString::Atoi(*ValuesToProcess[0]));
			for (int ValueIdx = 1; ValueIdx < ValuesToProcess.Num(); ValueIdx++)
			{
				if (ValueIdx % 2 > 0 && ValuesToProcess[ValueIdx].IsNumeric())
				{
					CfgIds.Add(FCString::Atoi(*ValuesToProcess[ValueIdx]));
				}
			}
		}
	}

	OnCheckIds(CfgIds);

#endif
}

void AActorRandom::OnCheckIds(TArray<int> CfgIds)
{
	if (!CfgIds.Contains(CfgId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find cfg with id : %d, actor name is : %s"), CfgId, *UKismetSystemLibrary::GetDisplayName(this))
	}

	for(int i = 0; i < RandomActors.Num(); i++)
	{
		if (AActorRandom* AR = Cast<AActorRandom>(RandomActors[i]))
		{
			AR->OnCheckIds(CfgIds);
		}
		else
		{
			int ChildCfgId = GetCfgIdFromActor(RandomActors[i]);
			if (!CfgIds.Contains(ChildCfgId))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find cfg with id : %d, actor name is : %s"), ChildCfgId, *UKismetSystemLibrary::GetDisplayName(RandomActors[i]))
			}
		}
	}
}

int AActorRandom::GetCfgIdFromActor(AActor* InActor)
{
#if WITH_EDITOR
	if (FProperty* Prop = InActor->GetClass()->FindPropertyByName("CfgId"))
	{
		return *Prop->ContainerPtrToValuePtr<int>(InActor);
	}
#endif
	return 0;
}

void AActorRandom::EdHideChildChange(AActor* Child, bool bCurHidden)
{
	if (Child)
	{
		if (AActorRandom* Item = Cast<AActorRandom>(Child))
		{
			Item->NotifyHiddenEdChange(bCurHidden);
		}
		else
		{
			Child->SetIsTemporarilyHiddenInEditor(bCurHidden);
		}
	}
}
#endif

void AActorRandom::Random(bool bFromClick)
{
#if UE_SERVER || UE_EDITOR
	if (RandomActors.Num() > 0)
	{
		int TotalProbability = 0;
		TArray<TPair<AActor*,int>> ActorToProbability;
		for (auto element : RandomActors)
		{
			if (element)
			{
				const int CurProperty = GetActorRandomProperty(element);
				if (CurProperty > 0)
				{
					TotalProbability += CurProperty;
					ActorToProbability.Add(TPair<AActor*,int>(element,TotalProbability));
				}
			}
		}

		int Result = UKismetMathLibrary::RandomIntegerInRange(1,TotalProbability);
#if WITH_EDITOR
		if (GIsEditor)
		{
			FString ElementPercentage = "";
			for(int i = 0; i < ActorToProbability.Num(); i++)
			{
				TPair<AActor*,int> curPair = ActorToProbability[i];
				if (!IsInRange(Result,curPair))
				{
					if (curPair.Key)
					{
						if (IsStartPIE())
						{
							DestroyChild(curPair.Key);
						}
						else
						{
							EdHideChildChange(curPair.Key, true);
						}
					}
				}
				else
				{
					if (!IsStartPIE() && curPair.Key)
					{
						EdHideChildChange(curPair.Key, false);
						if (AActorRandom* Child = Cast<AActorRandom>(curPair.Key))
						{
							Child->Random(bFromClick);
						}
					}
				}
			}

			if (!IsStartPIE())
			{
				for(int i = 0; i < ActorToProbability.Num(); i++)
				{
					const int CurProperty = GetActorRandomProperty(ActorToProbability[i].Key);
					ElementPercentage += UKismetSystemLibrary::GetDisplayName(ActorToProbability[i].Key)
					+ "\tprobability from : " + FString::Printf(TEXT("%04d"),ActorToProbability[i].Value - CurProperty)
					+ " (not contain) to : " + FString::Printf(TEXT("%04d"),ActorToProbability[i].Value)
					+ " (contains) probability range length : " + FString::FromInt(CurProperty) + ".\n";
				}
				ElementPercentage += " \n";
				UE_LOG(LogTemp,Log,TEXT("Object : %s, RandomResult : %d.\n%s"),
					*UKismetSystemLibrary::GetDisplayName(this),
					Result,
					*ElementPercentage)
				
				GEditor->RedrawAllViewports();
			}
		}
		else
#endif//#if WITH_EDITOR
		{
			for(int i = 0; i < ActorToProbability.Num(); i++)
			{
				TPair<AActor*,int> curPair = ActorToProbability[i];
				if (!IsInRange(Result,curPair))
				{
					if (curPair.Key)
					{
						DestroyChild(curPair.Key);
					}
				}
			}
		}
	}

#if WITH_EDITOR
	if (bFromClick)
	{
		if (PerpetualActors.Num() > 0)
		{
			for (auto element : PerpetualActors)
			{
				if (AActorRandom* Child = Cast<AActorRandom>(element))
				{
					Child->Random(true);
				}
			}
		}
	}
#endif//#if WITH_EDITOR

#if !WITH_EDITOR
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AActorRandom::DestroySelf);
	TimerManager.SetTimerForNextTick(Delegate);
#endif//#if !WITH_EDITOR

#else//#if UE_SERVER || UE_Editor
	if (!GEngine->IsEditor())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &AActorRandom::DestroySelfAndAllChildren);
		TimerManager.SetTimerForNextTick(Delegate);
	}
#endif//#if UE_SERVER#else
}

bool AActorRandom::IsInRange(const int& InResult, const TPair<AActor*,int>& InPair)
{
	const int CurProperty = GetActorRandomProperty(InPair.Key);
	return InResult <= InPair.Value && InResult > InPair.Value - CurProperty;
}

#if WITH_EDITOR
bool AActorRandom::IsStartPIE() const
{
	return GEditor->GetPlayInEditorSessionInfo().IsSet() && GEditor->GetPlayInEditorSessionInfo()->PlayRequestStartTime > 0;
}
#endif