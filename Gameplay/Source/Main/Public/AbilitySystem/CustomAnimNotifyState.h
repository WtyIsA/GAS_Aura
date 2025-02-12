
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CustomAnimNotifyState.generated.h"

UCLASS()
class MAIN_API UCustomAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)override;

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)override;

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)override;
	UFUNCTION()
	void ProcessEventByName(const FName& EventName);
	UPROPERTY()
	AActor* Owner;
	UPROPERTY()
	UAnimSequenceBase* CurAnimation;
};
