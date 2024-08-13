// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


AAuraCharacter::AAuraCharacter()
{
	//角色将向加速方向旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//旋转速率
	GetCharacterMovement()->RotationRate - FRotator(0.f, 400.f, 0.f);
	//移动限制在平面
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
}
