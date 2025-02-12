#include "LuaManger.h"

#include "Engine/GameInstance.h"
#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor/UnrealEdEngine.h"
#else
#include "Engine/GameEngine.h"
#endif

#ifdef USER_DB_SQLITE
    #include "LuaDBMgr.h"
#endif

#include "SelfGameInstance.h"
#include "LuaMessageTableOptInC.h"
#include "Player/MyShootCharacter.h"
#include "Replication/SimpleReplicationDriver.h"
#if !UE_SERVER
#include "Sdk/SdkInterface.h"
#include "Internationalization/TextFormatter.h"
#include "Text/TextFormatArgumentModifier_Main.h"
#include "AbilitySystem/CustomGameplayCueManager.h"
#endif
#include <LuaTableToJson.h>

#include "UtilsProject.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "UnLua/Private/UnLuaPrivate.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ListView.h"
#include "Res/ResMgr.h"
#include "Input/ShortcutkeyMgr.h"
#include "Kismet/GameplayStatics.h"
#include "Level/LevelCallback.h"
#include "UI/Ctrls/ImageGlobal.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/InputSettings.h"
#include "Http/HttpDownloadMgr.h"
#include "Materials/MaterialInstance.h"
#include "Slate/Private/Widgets/Views/SListPanel.h"
#include "Engine/AssetManager.h"
#include "EmptyObject.h"
#include "IPlatformFilePak.h"
#include "UnLuaManager.h"
#include "Cfg/CfgManager.h"
#include "Components/WidgetComponent.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "GameFramework/GameUserSettings.h"
#include "Gameplay/FPSManager.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#if ENABLE_MEMORYTRACKER
#include "Memory/MemoryTracker.h"
#endif
#include "GameplayTagsManager.h"
#include "IpNetDriver.h"
#include "Sockets.h"
#include "AbilitySystem/Attributes/CombatAttributeSet.h"
#include "AbilitySystem/Calculations/CustomCalculation.h"
#include "Net/MessagePool.h"
#include "Sdk/AccessToken.h"
#include "Sdk/VoiceInterface.h"
#include "Engine/LevelActorContainer.h"
#include "UnLuaEx.h"
#include "LuaGcTickable.h"
#include "AbilitySystem/CustomAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Target/GameplayAbilityTargetData_Msg.h"
#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Components/TileView.h"
#include "Engine/LevelStreaming.h"
#include "Http/CHttpDownloadTask.h"
#include "Input/CustomNavigationConfig.h"
#include "Net/SessionMgr.h"
#if PLATFORM_LINUX
#include "Fork/FForkProcess.h"
#endif

#include "CommonInputSubsystem.h"
#include "EvolutionOutput.h"
#include "Framework/Application/SlateUser.h"
#include "Gameplay/AttackTracerMgr.h"
#include "UI/UserWidgetEx/SettingInputProcess.h"
#include "Widgets/SViewport.h"

UGameInstance* GetGameInstance_()
{
    UGameInstance* GameInstance = nullptr;
#if WITH_EDITOR
    UUnrealEdEngine* engine = Cast<UUnrealEdEngine>(GEngine);
    if (engine && engine->PlayWorld) GameInstance = engine->PlayWorld->GetGameInstance();
#else
    UGameEngine* engine = Cast<UGameEngine>(GEngine);
    if (engine) GameInstance = engine->GameInstance;
#endif
    return GameInstance;
}

ULuaManger* ULuaManger::handle = nullptr;
UGameInstance* ULuaManger::m_pGame = nullptr;
ULuaManger* ULuaManger::Get(UGameInstance* pGameIns)
{
    if(handle == nullptr)   
    {
        m_pGame = pGameIns;
        handle = NewObject<ULuaManger>(pGameIns);
        handle->AddToRoot();
    }
    return  handle;
}

void ULuaManger::PreInit()
{
    m_luaGcTick = nullptr;
    FLevelCallback::Get();
#if ENABLE_MEMORYTRACKER
    FMemoryTracker::Get()->Init();
#endif
    handle->_nLatentUuid = 1;      
    handle->m_lastTickTime = FPlatformTime::Seconds();  
    FCoreDelegates::OnBeginFrame.AddUObject(handle, &ULuaManger::Tick);
    
#ifdef USER_DB_SQLITE
    FLuaDBMgr::Get();
#endif
    if (handle->IsEmulator())
    {
        UE_LOG(GAME, Display, TEXT("[main]IsEmulator set streaming.poolsize %d"), __LINE__);
        UKismetSystemLibrary::ExecuteConsoleCommand(m_pGame, TEXT("r.streaming.poolsize 4000"));
        UKismetSystemLibrary::ExecuteConsoleCommand(m_pGame, TEXT("r.AllowOcclusionQueries 0"));
    }
        
    if(IsRunningDedicatedServer())
    {
        m_luaGcTick = new FLuaGcTickable();
        FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ULuaManger::OnPostLoadMapWithWorld);        
    }
    else
    {
#if !UE_SERVER
        FTextFormatter::Get().RegisterTextArgumentModifier(FTextFormatString::MakeReference(TEXT("fromconfig")),[](const FTextFormatString& InArgsString, const FTextFormatPatternDefinitionConstRef& InPatternDef) { return TextFormatArgumentModifier_FromConfig::Create(InArgsString); });

        FVoiceInterface::GetIns()->Init();
#if WITH_APPLICATION_CORE
        GLog->AddOutputDevice(FEvolutionOutputReportGs::GetAsFOutputDevice());
#else
       GWarn = FEvolutionOutputWarn::GetAsFeedbackContext();
       GError = FEvolutionOutputError::GetAsOutputDeviceError();
#endif
#endif
    }
}

void ULuaManger::PostInit()
{
    UnLua::ClassBindLuaFile(UCustomCalculation::StaticClass()->GetDefaultObject());
    UnLua::ClassBindLuaFile(UCustomGameplayCueManager::StaticClass()->GetDefaultObject());
    UnLua::BindPreSettingLuaFile();
#if !UE_SERVER
    if (!IsRunningDedicatedServer())
    {
        TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(0);
        if (User.IsValid())
        {
            User->SetUserNavigationConfig(MakeShared<FCustomNavigationConfig>());
        }
    }
#endif
}

void ULuaManger::OnPersistantLevelLoaded_Implementation()
{
}

void ULuaManger::SetTimer(float DeltaTime)
{
}

void ULuaManger::SetLuaSearchPathAndOcPath(const TArray<FString>& luaSearchPaths, const FString& ocPath)
{
    FCfgManager::Get()->SetOcPath(ocPath);
    UnLua::SetLuaSearchPaths(luaSearchPaths);
}

void ULuaManger::Start_Implementation()
{
}


void ULuaManger::Init_Implementation()
{
}

UFPSManager* ULuaManger::GetFPSManager()
{
    return UFPSManager::Get(m_pGame);
}

UAttackTracerMgr* ULuaManger::GetAttackTracerManager()
{
    return UAttackTracerMgr::Get();
}

void ULuaManger::SetEnableMotionData(bool enable)
{
    FPlatformApplicationMisc::EnableMotionData(enable);
}

FString ULuaManger::GetLuaSrcPath(FString& luaScriptName)const
{
    luaScriptName = GLuaSrcRelativePath;
    return GLuaSrcFullPath;
}

FString ULuaManger::GetPersistentDownloadDir()const
{
    return CHttpDownloadMgr::Get()->m_savePath;
}

FString ULuaManger::GetPakSavePath() const
{
    return FUtilsProject::GetPakSavePath();
}


UObject* ULuaManger::GetResMgr()
{
    return  UResMgr::Get();
}

void ULuaManger::Free()
{
    if(handle == nullptr)
        return;
    handle->UnInit();    
    handle->RemoveFromRoot();
    handle = nullptr;
    CMessagePool::Free();

#if ENABLE_MEMORYTRACKER
    FMemoryTracker::Free();
#endif
#if !UE_SERVER
    FVoiceInterface::GetIns()->UnInit();
#endif    
}

void ULuaManger::UnInit()
{
    Shutdown();
    if(m_pGame)
    {
        if(_tHandle.IsValid())
            m_pGame->GetTimerManager().ClearTimer(_tHandle);
    }
    for(auto item : m_rootObjs)
    {
        if(IsValid(item))
            item->RemoveFromRoot();
    }
    m_rootObjs.Empty();
    LuaFree();
    FLevelCallback::Free();
#ifdef USER_DB_SQLITE
    FLuaDBMgr::Free();
#endif

#if WITH_APPLICATION_CORE
    GLog->RemoveOutputDevice(FEvolutionOutputReportGs::GetAsFOutputDevice());
#else
    GError = FPlatformOutputDevices::GetError();
    GWarn = FPlatformOutputDevices::GetFeedbackContext();
#endif
    SetReportLogLevel(false,0);
    if(m_luaGcTick)
        delete m_luaGcTick;
    m_luaGcTick = nullptr;
}

void ULuaManger::Tick()
{
    double nowTick = FPlatformTime::Seconds();
    float ticktime = nowTick - m_lastTickTime;
    CHttpDownloadMgr::Get()->Tick(ticktime);
    TickInLua(ticktime);
#ifdef USER_DB_SQLITE
    FLuaDBMgr::Get()->Tick(ticktime);
#endif
    m_lastTickTime = nowTick;
}

void ULuaManger::TickInLua_Implementation(float DeltaSeconds)
{
}

void ULuaManger::Shutdown_Implementation()
{
}

void ULuaManger::EmptyOverrideMaterials(UMeshComponent* mesh)
{
    if(!IsValid(mesh))
        return ;
    mesh->EmptyOverrideMaterials();
}

FString ULuaManger::GetMaterialPath(UMaterialInstance* mi)
{
    if(!IsValid(mi))
        return FString();
    return mi->GetOutermost()->GetName();
}

void ULuaManger::MarkObjectToDestory(UObject* pObject)
{
    if(IsValid(pObject))
    {
        pObject->MarkPendingKill(); 
        if(const UUserWidget * pWidget = Cast<UUserWidget>(pObject))
        {
            if(!pWidget->WidgetTree)
                return;
            TArray<UWidget*> allChildren;
            pWidget->WidgetTree->GetAllWidgets(allChildren);
            for (auto item : allChildren)
            {
                MarkObjectToDestory(item);
            }
        }
        else
        {
            if(const UListView* pList = Cast<UListView>(pObject))
            {
                for (auto item : pList->GetListItems())
                {                    
                    UUserWidget* listEntry = pList->GetEntryWidgetFromItem(item);
                    if(listEntry)
                        MarkObjectToDestory(listEntry);
                }                
            }
        }           
    }
}

void ULuaManger::RemoveUObjectFromRoot(UObject* pObject)
{
    if(IsValid(pObject))
    {
        pObject->RemoveFromRoot();
        m_rootObjs.Remove(pObject);
    }
}

void ULuaManger::ToGc()
{
    GEngine->ForceGarbageCollection(true);
}

UGameUserSettings* ULuaManger::GetGameUserSettings()
{
    return  GEngine->GetGameUserSettings();
}

void ULuaManger::SetScalabilityLevel(uint8 Scalability)
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

    UserSettings->SetFoliageQuality(Scalability);

    UserSettings->SetViewDistanceQuality(Scalability);

    UserSettings->SetShadowQuality(Scalability);

    UserSettings->SetVisualEffectQuality(Scalability);

    UserSettings->SetPostProcessingQuality(Scalability);

    UserSettings->SetShadingQuality(Scalability);

    UserSettings->SetTextureQuality(Scalability);
    
    SetMSAA(Scalability);
    
    UserSettings->ApplyHardwareBenchmarkResults();
}

void ULuaManger::SetShadowQuality(uint8 quality)
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    UserSettings->SetShadowQuality(quality);
    UserSettings->ApplyHardwareBenchmarkResults();
}

void ULuaManger::SetMaterialQualityLevel(uint8 quality)
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    UserSettings->SetTextureQuality(quality);
    UserSettings->ApplyHardwareBenchmarkResults();
}

void ULuaManger::SetMSAA(uint8 quality)
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (quality > 2 && GetActiveDeviceProfileName().Contains("Adreno7xx"))
    {
        quality = 2;
    }
    UserSettings->SetAntiAliasingQuality(quality);
    UserSettings->ApplyHardwareBenchmarkResults();
}

uint32 ULuaManger::GetGBRam()
{
    return FGenericPlatformMemory::GetPhysicalGBRam();
}

uint64 ULuaManger::GetMRam()
{
   return FPlatformMemory::GetConstants().TotalPhysical / 1024 / 1024 ;
}

uint64 ULuaManger::GetAvailableMRam()
{
    return  FPlatformMemory::GetStats().AvailablePhysical / 1024 / 1024 ;
}

FString ULuaManger::GetPerformanceLevel()
{
    if (UDeviceProfileManager::Get().GetActiveProfile())
    {
        return  UDeviceProfileManager::Get().GetActiveProfile()->BaseProfileName;
    }
    return TEXT("");
}

FString ULuaManger::GetActiveDeviceProfileName()
{
    return UDeviceProfileManager::Get().GetActiveDeviceProfileName();
}

void ULuaManger::SetUROParam(USkeletalMeshComponent* mesh,TArray<float>& thresholds )
{
    if (mesh)
    {
        mesh->AnimUpdateRateParams->MaxEvalRateForInterpolation = 5;
        mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Reset();
        if (thresholds.Num()==0)
        {
            mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.01);
            mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.003);
            mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.0005);
            mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.0002);
        }
        else
        {
            for (auto Theshold : thresholds)
            {
                mesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(Theshold);
            }
        }
        mesh->AnimUpdateRateParams->BaseNonRenderedUpdateRate =15;
    }
}

void ULuaManger::IncreaseBodyInstanceDamping(USkeletalMeshComponent* TargetMesh, float MultiFactor, FName InRootBoneName)
{
    if (TargetMesh && TargetMesh->GetPhysicsAsset())
    {
        static auto Callback = [MultiFactor](FBodyInstance* Body)
        {
            if (Body->LinearDamping > 0.f)
            {
                Body->LinearDamping *= MultiFactor;
            }
            else
            {
                Body->LinearDamping = 0.1f;
            }
            
            if (Body->AngularDamping > 0.f)
            {
                Body->AngularDamping *= MultiFactor;
            }
            else
            {
                Body->AngularDamping = 0.1f;
            }
            
            Body->UpdateDampingProperties();
        };
        TargetMesh->ForEachBodyBelow(InRootBoneName, true, false, Callback);
    }
}

void ULuaManger::ResetBodyInstanceDamping(USkeletalMeshComponent* TargetMesh)
{
    if (TargetMesh)
    {
        if (UPhysicsAsset * PhysAsset = TargetMesh->GetPhysicsAsset())
        {
            for (int32 BodyIdx = 0; BodyIdx < TargetMesh->Bodies.Num(); ++BodyIdx)
            {
                if (FBodyInstance* BodyInst = TargetMesh->Bodies[BodyIdx])
                {
                    if(PhysAsset->SkeletalBodySetups.Num() > BodyIdx)
                    {
                        if (UBodySetup * PhysAssetBodySetup = PhysAsset->SkeletalBodySetups[BodyIdx])
                        {
                            BodyInst->LinearDamping = PhysAssetBodySetup->DefaultInstance.LinearDamping;
                            BodyInst->AngularDamping = PhysAssetBodySetup->DefaultInstance.AngularDamping;
                            BodyInst->UpdateDampingProperties();
                        }
                    }
                }
            }
        }
    }
}

FString ULuaManger::GetMachineId()
{
    return FPlatformMisc::GetLoginId();
}

FString ULuaManger::GetOperatingSystemId()
{
    return FPlatformMisc::GetOperatingSystemId();
}

FString ULuaManger::GetMacAddressString()
{
    return FPlatformMisc::GetMacAddressString();
}

void ULuaManger::AddUObjectToRoot(UObject* pObject)
{
    if(IsValid(pObject))
    {
        pObject->AddToRoot();
        m_rootObjs.Add(pObject);
    }
}

void ULuaManger::AddWidgetToGameViewPort(UUserWidget* widget, int nOrder)
{
    if(!IsValid(widget))
        return;
    if ( m_pGame->GetGameViewportClient())
    {
        TSharedRef<SWidget> ViewportContent = widget->TakeWidget();
        m_pGame->GetGameViewportClient()->AddViewportWidgetContent(ViewportContent, nOrder);
    }
}

void ULuaManger::RemoveWidgetFromGameViewPort(UUserWidget* widget)
{
    if(!IsValid(widget))
        return;
    if (m_pGame->GetGameViewportClient())
    {
        TSharedRef<SWidget> ViewportContent = widget->TakeWidget();
        m_pGame->GetGameViewportClient()->RemoveViewportWidgetContent(ViewportContent);       
    }
}

void ULuaManger::SetInputMode(bool bUiOnly, bool bShowMouse, UWidget* widget, bool bLockMouse)
{
    APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
    if(p)
    {
        bool ShowMouseChange = p->ShouldShowMouseCursor() != bShowMouse;
        if (bUiOnly)
            UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(p, widget, EMouseLockMode::DoNotLock);
        else
            UWidgetBlueprintLibrary::SetInputMode_GameOnly(p);
        p->SetShowMouseCursor(bShowMouse);
        ULuaManger::m_pGame->GetGameViewportClient()->SetIgnoreInput(false);
        if (bShowMouse && ShowMouseChange)
        {
            FVector2D MousePos = FSlateApplication::Get().GetGameViewport()->GetCachedGeometry().GetLocalSize() / 2;
            MousePos = USlateBlueprintLibrary::LocalToAbsolute(FSlateApplication::Get().GetGameViewport()->GetCachedGeometry(), MousePos);
            FSlateApplication::Get().SetCursorPos(MousePos);
        }
    }
}

int32 ULuaManger::GetStringAsciiLen(FString str)
{
    int nLen = 0;
    for(int i = 0; i < str.Len(); i++)
    {
        nLen++;
        if(!isascii(str[i]))
            nLen++;
    }    
    return nLen;
}

FString ULuaManger::CheckStringAsciiLen(FString str, int nMaxLen)
{
    int nLen = 0;
    for(int i = 0; i < str.Len(); i++)
    {
        nLen++;
        if(!isascii(str[i]))
            nLen++;
        if(nLen > nMaxLen)
        {
            return str.Mid(0, i);
        }
    }    
    return str;
}

FVector2D ULuaManger::GetWidgetSize(UWidget* pWidget)
{
    if(!IsValid(pWidget) || !pWidget->GetCachedWidget().IsValid())
        return  FVector2D();
    const FGeometry& info = pWidget->GetCachedWidget()->GetCachedGeometry();
    return  info.Size;
}

FVector2D ULuaManger::GetWidgetAbsolutePostion(UWidget* pWidget)
{
    if(!IsValid(pWidget) || !pWidget->GetCachedWidget().IsValid())
        return  FVector2D();
    const FGeometry& info = pWidget->GetCachedWidget()->GetCachedGeometry();
    FVector2D pos = info.GetAbsolutePosition();
    UListView* pParentList = Cast<UListView>(UUserListEntryLibrary::GetOwningListView(pWidget));
    if (pParentList)
    {
        UTileView* pTileView = Cast<UTileView>(pParentList);
        if(pTileView)
            return pos;
        const FGeometry& listGeometry = pParentList->GetCachedWidget()->GetCachedGeometry();
        FVector2D listPos = listGeometry.GetAbsolutePosition();

        const float LayoutScaleMultiplier = listGeometry.GetAccumulatedLayoutTransform().GetScale();
        FFloatProperty* pProperty = Cast<FFloatProperty>(
            pParentList->GetClass()->FindPropertyByName(TEXT("EntrySpacing")));
        double fSpaceing = 0.0f;
        if (pProperty)
        {
            fSpaceing = pProperty->GetFloatingPointPropertyValue(pProperty->ContainerPtrToValuePtr<void>(pParentList));
        }
        bool bFirstInLine = pos.X - listPos.X < fSpaceing / 2 + 0.001f;
        if (bFirstInLine)
            return pos;

        if (pParentList->GetOrientation() == Orient_Horizontal)
            pos.X += fSpaceing * LayoutScaleMultiplier;
        else
        {
            pos.Y += fSpaceing * LayoutScaleMultiplier;
        }
    }
    
    return pos;
}

bool ULuaManger::IsPostionUnderWidget(UWidget* pWidget, FVector2D AbsolutePos)
{
    if(!IsValid(pWidget) || !pWidget->GetCachedWidget().IsValid())
        return false;   
    const FGeometry& info = pWidget->GetCachedWidget()->GetCachedGeometry();
    return  info.IsUnderLocation(AbsolutePos);
}

FVector2D ULuaManger::GetViewportSize()
{
    FVector2D size = m_pGame->GetWorld()->GetGameViewport()->Viewport->GetSizeXY();
    float Scale  = UWidgetLayoutLibrary::GetViewportScale(m_pGame->GetWorld());
    if(Scale != 0)
    {
        size /= Scale;
    }
    return  size;
}


UWidget* ULuaManger::GetTopWidget(UWidget* pCtrl)
{
    if(!IsValid(pCtrl))
        return nullptr;
    UWidget* pTopParent = pCtrl;
    UWidget* pTemp = Cast<UWidget>(pCtrl->GetOuter());
    UWidgetTree* pTrees  = Cast<UWidgetTree>(pCtrl->GetOuter());
    
    while (pTemp != nullptr || pTrees != nullptr)
    {
        if(pTemp != nullptr)
        {
            pTopParent = pTemp;
            pTrees  = Cast<UWidgetTree>(pTemp->GetOuter());
            pTemp = Cast<UWidget>(pTemp->GetOuter());
        }else
        {
            pTemp = Cast<UWidget>(pTrees->GetOuter());
            pTrees  = Cast<UWidgetTree>(pTrees->GetOuter());
        }
    }
    return pTopParent;
}

void ULuaManger::GetWidgetChildren(UWidget* Widget,  TArray<UWidget*>& allChildren)
{
    if(!IsValid(Widget))
        return;
    if ( INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(Widget) )
    {
        TArray<FName> SlotNames;
        NamedSlotHost->GetSlotNames(SlotNames);

        for ( FName SlotName : SlotNames )
        {
            if ( UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName) )
            {
                allChildren.Add(SlotContent);
                GetWidgetChildren(SlotContent, allChildren);
            }
        }
    }

    if ( UPanelWidget* PanelParent = Cast<UPanelWidget>(Widget) )
    {
        for ( int32 ChildIndex = 0; ChildIndex < PanelParent->GetChildrenCount(); ChildIndex++ )
        {
            if ( UWidget* ChildWidget = PanelParent->GetChildAt(ChildIndex) )
            {
                allChildren.Add(ChildWidget);
                GetWidgetChildren(ChildWidget, allChildren);
            }
        }
    }
}

void ULuaManger::GetAllUText(UUserWidget* pWidget, TArray<UWidget*>& allText)
{
    if(!IsValid(pWidget) || !pWidget->WidgetTree)
        return;
    TArray<UWidget*> allChildren;
    pWidget->WidgetTree->GetAllWidgets(allChildren);
    for(auto item : allChildren)
    {
        if(UTextBlock* pText = Cast<UTextBlock>(item))
        {
            allText.Add(item);
        }else if(URichTextBlock* pText2 = Cast<URichTextBlock>(item))
        {
            allText.Add(item);
        }
    }
}

void ULuaManger::GetWidgetTexts(UWidget* pWidget,  TArray<UWidget*>& allText)
{
    if (!IsValid(pWidget))
        return;
    TArray<UWidget*> allChildren;
    GetWidgetChildren(pWidget, allChildren);
    for (auto item : allChildren)
    {
        if (UTextBlock* pText = Cast<UTextBlock>(item))
        {
            allText.Add(item);
        }
        else if (URichTextBlock* pText2 = Cast<URichTextBlock>(item))
        {
            allText.Add(item);
        }
    }
}

void ULuaManger::GetAllImageGlobal(UUserWidget* pWidget, TArray<UWidget*>& allImage)
{
    if(!IsValid(pWidget))
        return;
    TArray<UWidget*> allChildren;
    pWidget->WidgetTree->GetAllWidgets(allChildren);
    for(auto item : allChildren)
    {
        if(UImageGlobal* pImage = Cast<UImageGlobal>(item))
        {
            allImage.Add(item);
        }
    }
}

bool ULuaManger::OnWigetRightMouseButtonDown_Implementation(UWidget* topwidget, UUserWidget* widget, FVector2D screenPos)
{
    return  false;
}

bool ULuaManger::OnWigetMouseButtonDown_Implementation(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos)
{
    return false;
}

bool ULuaManger::OnWigetMouseButtonUp_Implementation(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos)
{
    return false;
}

bool ULuaManger::OnWigetMouseButtonMove_Implementation(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos, FVector2D CursorDelta)
{
    return false;
}


bool ULuaManger::OnWigetMouseLeave_Implementation(UWidget* topwidget, UUserWidget* widget)
{
    return  false;
}

bool ULuaManger::CheckKeyDown(FString keyName)
{
    APlayerController * pControl = m_pGame->GetFirstLocalPlayerController(m_pGame->GetWorld());
    UInputComponent* input = UShortcutkeyMgr::Get()->GetInputComp();
    if(pControl)
    {
        bool bDown = pControl->IsInputKeyDown(FKey(*keyName));
        return bDown;
    }
    return  false;
}

FVector2D ULuaManger::GetWidgetRelativePos(FVector2D pos, UWidget* selfWidget, UWidget* parentWidget)
{
    if(!IsValid(selfWidget) || !IsValid(parentWidget) || !selfWidget->GetCachedWidget().IsValid() || !parentWidget->GetCachedWidget().IsValid())
        return  FVector2D::ZeroVector;
    const FGeometry& info = selfWidget->GetCachedWidget()->GetCachedGeometry();
    FVector2D absolute = info.LocalToAbsolute(pos);
    const FGeometry& infoParent =  parentWidget->GetCachedWidget()->GetCachedGeometry();
    absolute =  infoParent.AbsoluteToLocal(absolute);
    return  absolute;
}

void ULuaManger::SetListAlignment(UWidget* pWidget, int alignment)
{
    UListView* pList = Cast<UListView>(pWidget);
    if(IsValid(pList))
    {
        TSharedPtr<STableViewBase> sListView = StaticCastSharedPtr<STableViewBase>(pList->GetCachedWidget());       
        if(sListView.Get() && sListView->ItemsPanel)
        {
            if(alignment > (int)EListItemAlignment::Fill)
                alignment = (int)EListItemAlignment::Fill;
            if(alignment < 0)
                alignment = 0;
            sListView->ItemsPanel->ItemAlignment = alignment;
            sListView->RebuildList();
        }
    }
}

UListViewBase* ULuaManger::GetListViewFromListEntry(UUserWidget* listEntry)
{
    if(!IsValid(listEntry))
        return nullptr;
    TSharedPtr<const IObjectTableRow> SlateRow = IObjectTableRow::ObjectRowFromUserWidget(listEntry);
    if (SlateRow.IsValid())
    {
        return SlateRow->GetOwningListView();
    }
    return nullptr;
}

void ULuaManger::WriteIniFile(const FString& strSection, const FString& key, const FString& value)
{
    FString filePath = FUtilsProject::GetUserConfigPath();
    GConfig->SetString(*strSection, *key, *value, filePath);
    GConfig->Flush(false, *filePath);
}
FString ULuaManger::GetIniFile(const FString& strSection, const FString& key, const FString& Name)
{
    FString filePath;
    if(Name.Len() == 0)
        filePath = FUtilsProject::GetUserConfigPath();
    else
        filePath = GetIniFilePath(Name);
    FString str(TEXT(""));
    GConfig->GetString(*strSection, *key, str, filePath );
    return  str;
}

void ULuaManger::EmptySectionIniFile(const FString& strSection,  bool bMatchKey)
{
    FString filePath = FUtilsProject::GetUserConfigPath();
    if(bMatchKey)
        GConfig->EmptySectionsMatchingString(*strSection, filePath);
    else
        GConfig->EmptySection(*strSection, filePath);
    GConfig->Flush(false, *filePath);
}

void ULuaManger::RemoveKeyIniFile(const FString& strSection, const FString& key)
{
    FString filePath = FUtilsProject::GetUserConfigPath();
    GConfig->RemoveKey(*strSection,*key, filePath);
    GConfig->Flush(false, *filePath);
}


FString ULuaManger::GetFileContent(FString strFilePath, bool bProjectFile)
{
    if(bProjectFile)
        strFilePath = FPaths::ProjectContentDir() + strFilePath;
    FString strContent;
    FFileHelper::LoadFileToString(strContent, *strFilePath);
    return  strContent;
}

bool ULuaManger::SaveStringToFile(const FString& strFilePath, const FString& strContent)
{
    CreateDir(FPaths::GetPath(strFilePath));
    return FFileHelper::SaveStringToFile(strContent, *strFilePath);
}

void ULuaManger::CreateDir(FString strPath)
{
    if(!IFileManager::Get().DirectoryExists(*strPath))
    {
        IFileManager::Get().MakeDirectory(*strPath, true);
    }
}

void ULuaManger::DeleteDir(FString strPath)
{
    IFileManager::Get().DeleteDirectory(*strPath, true, true);
}

void ULuaManger::MountPaks()
{
    FUtilsProject::MountSavePaks();
}

bool ULuaManger::FileIsExist(FString filePath, bool bProjectRelativeFile)
{
    if(bProjectRelativeFile)
        filePath = FPaths::ProjectContentDir() + filePath;
    return  IFileManager::Get().FileExists(*filePath);   
}

void ULuaManger::GetFilesFromPath(const FString& filePath, const FString& FileExtension, TArray<FString>& files)
{
    IFileManager::Get().FindFiles(files, *filePath, *FileExtension);
}

bool ULuaManger::DeletePak(const FString& pakName)
{
    FString pakFullPath = FPaths::Combine(GetPakSavePath(), pakName);
    FPakPlatformFile* PakFileMgr=(FPakPlatformFile*)FPlatformFileManager::Get().GetPlatformFile(FPakPlatformFile::GetTypeName());
    if(PakFileMgr == NULL)
    {
        UE_LOG(GAME, Error, TEXT("PakFileMgr is null"));
        return false;
    }
    if(PakFileMgr->GetLowerLevel() == nullptr)
        PakFileMgr->SetLowerLevel(&FPlatformFileManager::Get().GetPlatformFile());

    PakFileMgr->Unmount(*pakFullPath);
  
    return IFileManager::Get().Delete(*pakFullPath);
}

int32 ULuaManger::UnMountSavePaks(bool bAutoDelete)
{
    return FUtilsProject::UnMountSavePaks(bAutoDelete);
}


void ULuaManger::AddShortcutKey(FString keyName, UObject* obj, bool bShift, bool bAlt, bool bCtrl, bool bCmd, int32 inputEvent)
{
    UShortcutkeyMgr::Get()->AddShortcutKey(keyName, bShift, bAlt, bCtrl, bCmd, obj, inputEvent);
}

void ULuaManger::RemoveShortcutKey(FString keyName, UObject* obj, bool bShift, bool bAlt, bool bCtrl, bool bCmd, int32 inputEvent)
{
    UShortcutkeyMgr::Get()->RemoveShortcutKey(keyName, bShift, bAlt, bCtrl,bCmd, obj, inputEvent);
}

void ULuaManger::RemoveObjectShortcutKeys(UObject* obj)
{
    UShortcutkeyMgr::Get()->RemoveObjectShortcutKeys(obj);
}

void ULuaManger::OnShortcutKey_Implementation(UObject* widget, const FString& keyName, bool bShift, bool bAlt,
    bool bCtrl, bool bCmd, int32 inputEvent)
{
}

UInputComponent* ULuaManger::GetInputComponent()
{
    return UShortcutkeyMgr::Get()->GetInputComp();
}

void ULuaManger::AddAxisKey(FString keyName, UObject* obj)
{
#if !UE_SERVER
    if(FSdkInterface::GetIns()->IsEmulator())
        return;
#endif

    UShortcutkeyMgr::Get()->AddAxisKey(keyName, obj);
}

void ULuaManger::RemoveAxisKey(FString keyName, UObject* obj)
{
    UShortcutkeyMgr::Get()->RemoveAxisKey(keyName, obj);
}

void ULuaManger::OnAxisKey_Implementation(UObject* widget, const FString& keyName, float axisvalue, FVector2D touch1, FVector2D touch2)
{
}

bool ULuaManger::ChangeActionKey(FString actionName, FString fromkeyName, FString tokeyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd)
{
    UInputSettings* InputSettings = (UInputSettings*)GetDefault<UInputSettings>();
    if(InputSettings)
    {
        const TArray <FInputActionKeyMapping>& keymaps = InputSettings->GetActionMappings();
        TArray <FInputActionKeyMapping>* p = (TArray <FInputActionKeyMapping>*)&keymaps;
        for(auto& item : *p)
        {
            if(item.ActionName == FName(*actionName) && item.Key.ToString() == fromkeyName)
            {
                item.Key = FKey(*tokeyName);
                item.bAlt = bAlt;
                item.bCmd = bCmd;
                item.bCtrl = bCtrl;
                item.bShift = bShift;
                return  true;
            }
        }
    }
    return  false;
}

bool ULuaManger::ChangeAxisKey(FString actionName, FString fromkeyName, FString tokeyName)
{
    UInputSettings* InputSettings = (UInputSettings*)GetDefault<UInputSettings>();
    if(InputSettings)
    {
        const TArray <FInputAxisKeyMapping>& keymaps = InputSettings->GetAxisMappings();
        TArray <FInputAxisKeyMapping>* p = (TArray <FInputAxisKeyMapping>*)& keymaps;
        for(auto& item : *p)
        {
            if(item.AxisName == FName(*actionName) && item.Key.ToString() == fromkeyName)
            {
                item.Key = FKey(*tokeyName);
                return true;
            }
        }
    }
    return false;
}

void ULuaManger::ForceRebuildKeymaps()
{
    UInputSettings* InputSettings = (UInputSettings*)GetDefault<UInputSettings>();
    if(InputSettings)
    {
        InputSettings->ForceRebuildKeymaps();
    }
}

bool ULuaManger::ChangeShortcutKey(FString fromKeyName, FString toKeyname, UObject* obj , bool bShift , bool bAlt ,bool bCtrl , bool bCmd , int32 inputEvent)
{
    return UShortcutkeyMgr::Get()->ChangeShortcutKey(fromKeyName, toKeyname, bShift, bAlt, bCtrl, bCmd, obj, inputEvent);
}

void ULuaManger::RegisterSettingInputProcess()
{
    if (!SettingInputProcessor)
    {
        SettingInputProcessor = MakeShared<FSettingInputProcess>();
    }

    FSlateApplication::Get().RegisterInputPreProcessor(SettingInputProcessor);
}

void ULuaManger::UnregisterSettingInputProcess()
{
    if (!SettingInputProcessor)
    {
        UE_LOG(GAME, Error, TEXT("Call ULuaManger::UnregisterSettingInputProcess but SettingInputProcessor is not initialized."))
    }
    else
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(SettingInputProcessor);
    }
}


void ULuaManger::OpenLevel(FString levelName)
{
    UGameplayStatics::OpenLevel(m_pGame->GetWorld(), *levelName);
}

void ULuaManger::LoadStreamLevel(FString levelName,bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad)
{
    FLatentActionInfo ActionInfo;
    ActionInfo.CallbackTarget = this;
    ActionInfo.UUID = ++_nLatentUuid;
    UGameplayStatics::LoadStreamLevel(m_pGame->GetWorld(), *levelName,bMakeVisibleAfterLoad, bShouldBlockOnLoad, ActionInfo);
}

void ULuaManger::UnLoadStreamLevel(FString levelName, bool bShouldBlockOnLoad)
{
    FName tempName(*levelName);
    FLatentActionInfo ActionInfo;
    ActionInfo.CallbackTarget = this;
    ActionInfo.UUID = ++_nLatentUuid;
    UGameplayStatics::UnloadStreamLevel(m_pGame->GetWorld(), tempName, ActionInfo, bShouldBlockOnLoad);
}

void ULuaManger::LoadWorldStreamLevel(UObject* worldObject, FString levelName,bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad)
{
    check(worldObject)
    FLatentActionInfo ActionInfo;
    ActionInfo.CallbackTarget = this;
    ActionInfo.UUID = ++_nLatentUuid;
    UGameplayStatics::LoadStreamLevel(worldObject, *levelName, bMakeVisibleAfterLoad, bShouldBlockOnLoad, ActionInfo);
    
    if (bShouldBlockOnLoad)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(worldObject, EGetWorldErrorMode::ReturnNull);
        if (!World)
        {
            UE_LOG(GAME, Error, TEXT("[LoadWorldStreamLevel]world not find when load level %s"), *levelName);
            return;
        }

        World->FlushLevelStreaming(EFlushLevelStreamingType::Full);
    }
}

void ULuaManger::UnLoadWorldStreamLevel(UObject* worldObject, FString levelName, bool bShouldBlockOnLoad)
{
    FLatentActionInfo ActionInfo;
    ActionInfo.CallbackTarget = this;
    ActionInfo.UUID = ++_nLatentUuid;
    UGameplayStatics::UnloadStreamLevel(worldObject, *levelName, ActionInfo, bShouldBlockOnLoad);
    
    if (bShouldBlockOnLoad)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(worldObject, EGetWorldErrorMode::ReturnNull);
        if (!World)
        {
            UE_LOG(GAME, Error, TEXT("[UnLoadWorldStreamLevel]world not find when load level %s"), *levelName);
            return;
        }

        World->FlushLevelStreaming(EFlushLevelStreamingType::Full);
    }
}

bool ULuaManger::LoadMap(FName WorldContextHandle, const FString& mapPath)
{
    FWorldContext* WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    if (!WorldContext)
    {
        UE_LOG(GAME, Error, TEXT("[LoadMap]Wrong WorldContextHandle:%s"), *WorldContextHandle.ToString());
        return false;
    }
    UGameplayStatics::FlushLevelStreaming(WorldContext->World());
    FString Error;
    if (!GEngine->LoadMap(*WorldContext, FURL(*mapPath), nullptr, Error))
    {
        UE_LOG(GAME, Error, TEXT("[LoadMap]Load %s error:%s"), *mapPath, *Error);
        return false;
    }

    return true;
}


void ULuaManger::OnLoadStreamLevel_Implementation(const FString& worldName, UWorld* pWorld)
{
}

void ULuaManger::OnUnLoadStreamLevel_Implementation(const FString& worldName, UWorld* pWorld)
{
}

void ULuaManger::OnLevelLoaded_Implementation(UWorld* pWorld, const FString& levelName)
{
}

void ULuaManger::OnDsNetWorkFailure_Implementation(UWorld* World, const FString& worldName,UNetDriver* NetDriver, int32 nailureType, const FString& ErrorString)
{
}

void ULuaManger::OnLevelUnloadComplete_Implementation(UWorld* World, const FString& worldName, bool bSessionEnded,
    bool bCleanupResources)
{
}

FString ULuaManger::GetIniFilePath(const FString& Name)
{
    if (Name == TEXT("Game"))
    {
        return GGameIni;
    }
    else if (Name == TEXT("Input"))
    {
        return GInputIni;
    }
    else if (Name == TEXT("GameUserSettings"))
    {
        return GGameUserSettingsIni;
    }
    else if (Name == TEXT("Scalability"))
    {
        return GScalabilityIni;
    }
    else if (Name == TEXT("Hardware"))
    {
        return GHardwareIni;
    }
    return GEngineIni;
}

void ULuaManger::DragWidget_Implementation(UObject* pDragObj,int32 nDragType,int32 nDropType, UWidget* pDrop, UUserWidget* pTopParent)
{
}

void ULuaManger::OnBeginDragWidget_Implementation(UObject* pDragObj, int32 nDragType, UUserWidget* pTopParent, UDragDropWidget* _CurDragVisualWidget)
{
}

void ULuaManger::OnCancelDragWidget_Implementation(UObject* pDragObj, int32 nDragType, UUserWidget* pTopParent)
{
}

void ULuaManger::OnCoverWidgetChange_Implementation(UUserWidget* pTopParent, UUserWidget* coverPanel, UDragDropWidget* coverObj,UDragDropWidget* dragObj,int32 nDragType,int32 nDropType)
{
}

AActor* ULuaManger::GetCauserFromGESpec(const FGameplayEffectSpec& Spec)
{
    return Spec.GetContext().GetEffectCauser();
}

const FVector ULuaManger::GetHitPosFromGESpec(const FGameplayEffectSpec& Spec)
{
    if (Spec.GetContext().GetHitResult())
    {
        return Spec.GetContext().GetHitResult()->ImpactPoint;
    }
    return FVector::ZeroVector;
}

const float ULuaManger::GetModAttributeMagnitudeFromSpec(const FGameplayEffectSpec& Spec, FGameplayAttribute Attribute)
{
    if (const FGameplayEffectModifiedAttribute* ModAttribute = Spec.GetModifiedAttribute(Attribute))
    {
        return ModAttribute->TotalMagnitude;
    }

    return 0.f;
}

const bool ULuaManger::IsAttributeChangeFromSpec(const FGameplayEffectSpec& Spec, FName AttributeName)
{
    FGameplayAttribute TargetAttribute = UCustomAttributeSet::GetPropertyByName(UCombatAttributeSet::StaticClass(),AttributeName);
    for (auto It : Spec.ModifiedAttributes)
    {
        if (TargetAttribute == It.Attribute)
        {
            return true;
        }
    }
    return false;
}

const bool ULuaManger::IsGESpecContainsTag(const FGameplayEffectSpec& Spec, const FName& InName)
{
    FGameplayTagContainer Container;
    Spec.GetAllAssetTags(Container);
    return Container.HasTag(UGameplayTagsManager::Get().RequestGameplayTag(InName));
}

FGameplayTag ULuaManger::GetGameplayTagByString(const FName& InName)
{
    return FGameplayTag::RequestGameplayTag(InName);
}

float ULuaManger::GetActiveGETimeRemaining(const FActiveGameplayEffect& ActiveGameplayEffect)
{
    return ActiveGameplayEffect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
}

FActiveGameplayEffectHandle ULuaManger::GetActiveGEHandle(const FActiveGameplayEffect& ActiveGameplayEffect)
{
    return ActiveGameplayEffect.Handle;
}

bool ULuaManger::GESpecGrantedHasTag(const FGameplayEffectSpec& GESpec, const FName& TagName)
{
    FGameplayTagContainer GrantedTags;
    GESpec.GetAllGrantedTags(GrantedTags);
    return GrantedTags.HasTag(FGameplayTag::RequestGameplayTag(TagName));
}

UObject* ULuaManger::GetCDO(const UClass* InClass)
{
    if (InClass)
        return InClass->GetDefaultObject();
    return nullptr;
}

float ULuaManger::GetSetByCaller(const FGameplayEffectSpec& Spec, FName& InName, float InDefaultValue)
{
    return Spec.GetSetByCallerMagnitude(InName,false,InDefaultValue);
}

float ULuaManger::GetTagSetByCaller(const FGameplayEffectSpec& Spec, FGameplayTag& InTag, float InDefaultValue)
{
    return Spec.GetSetByCallerMagnitude(InTag,false,InDefaultValue);
}

void ULuaManger::RefreshBindCommonUIKeys(TMap<FName,FCommonUIRedirectKeyData>& RedirectMap)
{
    FUIActionBinding::RefreshCustomRedirectKeys(RedirectMap);
    UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetFirstLocalPlayer());
    if (CommonInputSubsystem)
    {
        CommonInputSubsystem->OnInputMethodChangedNative.Broadcast(CommonInputSubsystem->GetCurrentInputType());
    }
}

const float ULuaManger::GetImpactForceFromASC(UAbilitySystemComponent* ASC)
{
    if (const UCombatAttributeSet* AttributeSet = ASC->GetSet<UCombatAttributeSet>())
    {
        return AttributeSet->GetImpactForce();
    }
    return 0.f;
}

FString ULuaManger::EncodeUrl(FString url)
{
    url = FGenericPlatformHttp::UrlEncode(url);
    return url;
}


void ULuaManger::StartDownload(FString url, FString savepath, int32 nRetryTimes, int64 nTotalSize, int32 nThreadCount, FString strMd5,const FOnHttpDownloadComplete& callback)
{
    if(savepath.Len() < 2)
        CHttpDownloadMgr::Get()->Download(url, nRetryTimes, strMd5, callback);
    else
        CHttpDownloadMgr::Get()->Download(url, savepath, nTotalSize, nThreadCount, nRetryTimes, strMd5, callback);
}

void ULuaManger::StartDownloadRange(FString url, FString savepath,TArray<int32> nRanges, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete& callback)
{    
    TArray<int64> nLargeRanges;
    for(auto temp:nRanges)
    {
        nLargeRanges.Add(temp);
    }
    CHttpDownloadMgr::Get()->Download(url, savepath, nLargeRanges, nRetryTimes, strMd5, callback);
}

void ULuaManger::StartDownloadLargeFileRange(FString url, FString savepath,TArray<FString> strRanges, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete& callback)
{
    TArray<int64> nRanges;
    for(auto& str:strRanges)
    {
        nRanges.Add(FCString::Atoi64(*str));
    }
    CHttpDownloadMgr::Get()->Download(url, savepath, nRanges, nRetryTimes, strMd5, callback);
}

void ULuaManger::StopDownload(FString url)
{
    CHttpDownloadMgr::Get()->Stop(url);
}

void ULuaManger::PauseDownload(const FString& url)
{
    CHttpDownloadMgr::Get()->Pause(url);
}

void ULuaManger::ContinueDownload(const FString& url)
{
    CHttpDownloadMgr::Get()->Continue(url);
}

bool ULuaManger::HttpIsDownloading(const FString& url)
{
    return CHttpDownloadMgr::Get()->GetTask(url) != nullptr;
}

void ULuaManger::OnDownloadComplete_Implementation(const FString& strUel, bool bSuccess, const FString&contentOrsavePath, bool bSaveFile, int errorCode)
{
}

void ULuaManger::HttpDataWriteToDisk()
{
    CHttpDownloadMgr::Get()->DataWriteToDisk();
}

int64 ULuaManger::GetHttpDownloadedLen(FString& downloadInfo)
{
    return  CHttpDownloadMgr::Get()->GetHadDownloaded(downloadInfo);
}

int64 ULuaManger::GetTaskDownloadedSize(const FString& strUrl)
{
    CHttpDownloadTask*pTask = CHttpDownloadMgr::Get()->GetTask(strUrl);
    if(!pTask )
        return 0;
    FString info;
    return pTask->GetHadDownloaded(info);
}


void ULuaManger::ReDownloadErrorTask()
{
    CHttpDownloadMgr::Get()->ReDownloadErrorTask();
}

void  ULuaManger::OpenUrl(FString strUrl)
{
    FPlatformProcess::LaunchURL(*strUrl, nullptr, nullptr);
}
int64 ULuaManger::GetTick()
{
    int64 nTick = FDateTime::Now().GetTicks() / ETimespan::TicksPerMillisecond;
    return nTick;
}

int32 ULuaManger::GetLuaMem()
{
    lua_State *L = UnLua::GetState();
    int32 MemCount = lua_gc(L, LUA_GCCOUNTB, 0) + (lua_gc(L, LUA_GCCOUNT, 0) * 1024);
    return MemCount;
}

void ULuaManger::Copy(FString& str)
{
    FPlatformApplicationMisc::ClipboardCopy(*str);
}

bool ULuaManger::ChangePrecomputedVisibilityHandler(UObject* WorldContextObject, const FString& InLongPackageName)
{
    check(WorldContextObject);
    UWorld* World = WorldContextObject->GetWorld();
    check(World);
    return World->ChangePrecomputedVisibilityHandler(InLongPackageName);
}

const ULevelStreaming* ULuaManger::GetLevelByPackageName(UObject* WorldContextObject, const FString& InLongPackageName)
{
    check(WorldContextObject);
    UWorld* World = WorldContextObject->GetWorld();
    check(World);
    return World->GetLevelByPackageName(InLongPackageName);
}

bool ULuaManger::AllSubLevelIsLoaded()
{
    UWorldComposition* pComp = GWorld->WorldComposition;
    if(pComp == nullptr)
        return false;
    int32 NumPlayers = GEngine->GetNumGamePlayers(GWorld);
    if (NumPlayers == 0)
    {
        return false;
    }
    bool bAddNewLoctations = false;
    bool bFirstGet = _LargeWorldLocations.Num() == 0;

    for (int32 PlayerIndex = 0; PlayerIndex < NumPlayers; ++PlayerIndex)
    {
        ULocalPlayer* Player = GEngine->GetGamePlayer(GWorld, PlayerIndex);
        if (Player && Player->PlayerController)
        {
            FVector ViewLocation;
            FRotator ViewRotation;
            Player->PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
            bool bExist = false;
            for (auto& itemLocation : _LargeWorldLocations)
            {
                if (itemLocation.X == ViewLocation.X && itemLocation.Y == ViewLocation.Y)
                {
                    bExist = true;
                    break;
                }
            }
            if (!bExist)
            {
                UE_LOG(GAME, Log, TEXT("OnLoadStreamLevel reget to show list, this pos %s"),
                       *ViewLocation.ToString())
            }
            bAddNewLoctations = bAddNewLoctations || !bExist;
        }
    }

    if (bAddNewLoctations || bFirstGet)
    {
        _LargeWorldLocations.Empty();
        _LargeDistanceVisibleLevels.Empty();
        for (int32 PlayerIndex = 0; PlayerIndex < NumPlayers; ++PlayerIndex)
        {
            ULocalPlayer* Player = GEngine->GetGamePlayer(GWorld, PlayerIndex);
            if (Player && Player->PlayerController)
            {
                FVector ViewLocation;
                FRotator ViewRotation;
                Player->PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
                _LargeWorldLocations.Add(ViewLocation);
            }
        }
        TArray<FDistanceVisibleLevel> DistanceHiddenLevels;
        pComp->GetDistanceVisibleLevels(_LargeWorldLocations.GetData(), _LargeWorldLocations.Num(), _LargeDistanceVisibleLevels, DistanceHiddenLevels);
    }
    else
    {
    }
    for(auto& item : _LargeDistanceVisibleLevels)
    {
        if(item.StreamingLevel == nullptr || item.StreamingLevel->GetLoadedLevel() == nullptr ||
            !item.StreamingLevel->GetLoadedLevel()->bIsVisible)
            return false;
    }
    _LargeWorldLocations.Empty();
    _LargeDistanceVisibleLevels.Empty();
    return  true;
}

bool ULuaManger::IsWorldReady(const FName& WorldContextHandle)
{
    FWorldContext* WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    if (!WorldContext)
    {
        UE_LOG(GAME, Error, TEXT("[IsWorldReady]wrong WorldContextHandle %s"), *WorldContextHandle.ToString())
        return false;
    }

    if (!WorldContext->TravelURL.IsEmpty() || WorldContext->PendingNetGame)
        return false;
   
    UWorld* World = WorldContext->World();
    if (!World)
        return false;

    if (m_pGame->GetLocalPlayers().Num() == 0)
        return false;

    ULocalPlayer* LocalPlayer = m_pGame->GetLocalPlayers()[0];
    if (!LocalPlayer)
        return false;

    if (!LocalPlayer->PlayerController ||!LocalPlayer->PlayerController->GetPawn())
        return false;

    UWorldComposition* WorldComposition = World->WorldComposition;
    if (WorldComposition)
    {
        double OldTilesStreamingTimeThreshold = WorldComposition->TilesStreamingTimeThreshold;
        WorldComposition->TilesStreamingTimeThreshold = 0.0;
        WorldComposition->UpdateStreamingState();
        WorldComposition->TilesStreamingTimeThreshold = OldTilesStreamingTimeThreshold;
    }

    if (World->HasStreamingLevelsToConsider())
        return false;

    TArray<ULevelStreaming*> LevelStreamings = World->GetStreamingLevels();
    for (ULevelStreaming* LevelStreaming : LevelStreamings)
    {
        if (LevelStreaming->GetCurrentState() != ULevelStreaming::ECurrentState::LoadedNotVisible && LevelStreaming->
            GetCurrentState() != ULevelStreaming::ECurrentState::LoadedVisible)
        {
            return false;
        }
    }
    return CheckLevelStreamingDistanceVisible();
}

bool ULuaManger::CheckLevelStreamingDistanceVisible()
{
    return GetWorld()->CheckLevelStreamingDistanceVisible();
}

bool ULuaManger::IsServerWorldReady(UWorld* World)
{
    if (!World)
        return false;

    UWorldComposition* WorldComposition = World->WorldComposition;
    if (WorldComposition)
    {
        WorldComposition->UpdateStreamingState();
    }

    if (World->HasStreamingLevelsToConsider())
        return false;

    if(IsRunningDedicatedServer())
    {
        TArray<ULevelStreaming*> LevelStreamings = World->GetStreamingLevels();
        for(ULevelStreaming* LevelStreaming : LevelStreamings)
        {
            if(LevelStreaming->GetCurrentState() != ULevelStreaming::ECurrentState::LoadedVisible)
            {
                return false;
            }
        }       
    }   

    return true;
}

void ULuaManger::AddActorToLevelCluster(AActor* Actor)
{
    if (Actor == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULuaManger::AddActorToLevelCluster Actor was null"));
        return;
    }
    ULevel* Level = Actor->GetLevel();
    if (Level == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULuaManger::AddActorToLevelCluster Actor Level was null"));
        return;
    }
    if (Level->bActorClusterCreated && Actor->CanBeInCluster())
    {
        Level->ActorCluster->CreateCluster();
        Actor->AddToCluster(Level->ActorCluster);
        Level->ActorsForGC.RemoveSwap(Actor);
    }
}

void ULuaManger::AddObjectToActorCluster(UObject* Object, AActor* Actor)
{
    if (Object == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULuaManger::AddObjectToActorCluster Object was null"));
        return;
    }
    if (Actor == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ULuaManger::AddObjectToActorCluster Actor was null"));
        return;
    }
    Object->AddToCluster(Actor);
}

void ULuaManger::AddToAlwaysRelevant(AActor* Actor, APlayerController* PlayerController)
{
    if(IsValid(Actor) && IsValid(PlayerController) && IsRunningDedicatedServer())
    {
        if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
        {
            if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
            {
                SimpleReplicationDriver->AddToAlwaysRelevant(Actor,PlayerController);
            }        
        }
    }
}

void ULuaManger::RemoveFromAlwaysRelevant(AActor* Actor, APlayerController* PlayerController)
{
    if(IsValid(Actor) && IsValid(PlayerController) && IsRunningDedicatedServer())
    {
        if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
        {
            if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
            {
                SimpleReplicationDriver->RemoveFromAlwaysRelevant(Actor,PlayerController);
            }        
        }
    }
}

UBTNode* ULuaManger::GetActiveNodeFromBehaviorTree(UBehaviorTreeComponent* pTree)
{
    if(IsValid(pTree))
    {
        return (UBTNode*)pTree->GetActiveNode();
    }
    return nullptr;
}

int32 ULuaManger::GetActiveInstanceIdFromBehaviorTree(UBehaviorTreeComponent* pTree)
{
    if(IsValid(pTree))
        return pTree->GetActiveInstanceIdx();
    return 0;
}

bool ULuaManger::SdkIsInit()
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->SdkIsInit();
#endif
    return true;
}

void ULuaManger::RestartApp()
{
#if !UE_SERVER
    FPlatformMisc::RestartApplication();
#endif
}

void ULuaManger::GetPlatformInfo()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->GetPlatformInfo();
#endif
}

void ULuaManger::SetPlatformInfo_Implementation(int nPlatformId, const FString& ProjectId, const FString& channelId, const FString& deviceId)
{
    
}


void ULuaManger::ShowLoginPanel()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->ShowSdkLoginPanel();    
#endif
}

void ULuaManger::OpenUserCenter()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->OpenUserCenter();
#endif
}

bool ULuaManger::CanAutoLogin()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->CanAutoLogin();
#endif
    return false;
}

void ULuaManger::AutoLogin()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->AutoLogin();
#endif
}

void ULuaManger::Login(int nLoginType)
{
#if !UE_SERVER
    if(nLoginType == 1)
        FSdkInterface::GetIns()->GuestLogin();
    else
        FSdkInterface::GetIns()->LoginNoUi(nLoginType);

#endif
}

void ULuaManger::AccountBind(int nLoginType, const FSdkLoginDelegate& OnBindAccoutBack)
{    
#if !UE_SERVER
    OnAccountBindCallBack = std::move(OnBindAccoutBack); 
    FSdkInterface::GetIns()->AccountBind(nLoginType);
#endif
}

void ULuaManger::AccountUnBind(int nLoginType, const FSdkLoginDelegate& OnUnBindAccoutBack)
{    
#if !UE_SERVER
    OnAccountUnBindCallBack = std::move(OnUnBindAccoutBack);
    FSdkInterface::GetIns()->AccountUnBind(nLoginType);
#endif
}

void ULuaManger::AccountForceRebend(int nAccountType, int force, const FSdkLoginDelegate& OnBindAccoutBack)
{    
#if !UE_SERVER
    OnAccountBindCallBack = std::move(OnBindAccoutBack); 
    FSdkInterface::GetIns()->AccountForceRebend(nAccountType,force);
#endif
}

FString ULuaManger::GetLoginUserInfo()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->GetLatestUserInfo();
#endif
    return FString();
}

void ULuaManger::LogOut()
{    
#if !UE_SERVER
#if PLATFORM_WINDOWS
    OnLogOutCallBack.ExecuteIfBound(true,TEXT(""),TEXT(""),TEXT(""));
#else
    FSdkInterface::GetIns()->Logout();
#endif
#endif
}
void ULuaManger::SwitchLogin(uint64 userId,const FSdkLoginDelegate& OnSwitchAccoutBack)
{
#if !UE_SERVER
    OnSwitchAccountCallBack = std::move(OnSwitchAccoutBack);
    FSdkInterface::GetIns()->SwitchAccount(userId);
#endif
}

void ULuaManger::GetHistoryAccounts(const FSdkAccountsDelegate& OnHistoryAccout)
{
#if !UE_SERVER
    OnHistoryAccountsCallBack = std::move(OnHistoryAccout);
    FSdkInterface::GetIns()->GetHistoryAccounts();
#endif
}

void ULuaManger::DeleteHistoryAccount(uint64 userId, const FSdkBoolLongDelegate& OnDeleteResult)
{
#if !UE_SERVER
    OnDelateHistoryAccountCallBack = std::move(OnDeleteResult);
    FSdkInterface::GetIns()->DeleteHistoryAccount(userId);
#endif
}

bool ULuaManger::IsEmulator()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->IsEmulator();
#endif
    return false;
}

bool ULuaManger::IsSandbox()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->IsSandbox();
#endif
    return false;
}

void ULuaManger::SetDebug(bool bEnable)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->SetDebug(bEnable);
#endif
    
}

void ULuaManger::SetSandbox(bool bEnable)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->SetSandboxEnable(bEnable);
#endif
}


double ULuaManger::GetBatteryLevel()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->GetBatteryLevel();
#endif
    return 0.0f;
}

bool ULuaManger::MobileIsCharging()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->MobileIsCharging();
#endif
    return false;
}

bool ULuaManger::CheckProtocolVersion()
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->CheckProtocolVersion();
#endif
    return false;
}

void ULuaManger::SetUserHasAgreedProtocolsUpdate()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->SetUserHasAgreedProtocolsUpdate();
#endif
}


void ULuaManger::GetUserAgreementUrlAndPrivacyPolicy(FString& strUserAgreementUrl,FString& strPrivacyPolicy)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->GetUserAgreementUrlAndPrivacyPolicy(strUserAgreementUrl, strPrivacyPolicy);
#endif
}

void ULuaManger::Report(const FString& eventName, const FString& key, const FString& value, const FString& extraData)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->Report(eventName, key,value, extraData);
#endif
}

void ULuaManger::UploadLog(const FString& filePath, uint64 roleId, const FString& roleName, const FSdkDelegate& updateCallback)
{
#if !UE_SERVER
    OnUpdateFileCallBack = std::move(updateCallback);
    FSdkInterface::GetIns()->UploadFile(filePath, roleId, roleName);
#endif
}

void  ULuaManger::JudgeAgeGate(const FSdkAgeDelegate& ageCallback)
{
#if !UE_SERVER
    OnAgeGateCallback = std::move(ageCallback);
    FSdkInterface::GetIns()->JudgeAgeGate();
#endif
}

void ULuaManger::ModifyAgeGateStatus(bool bEnable)
{    
#if !UE_SERVER
    FSdkInterface::GetIns()->ModifyAgeGateStatus(bEnable);
#endif
}

bool ULuaManger::QueryAgeGateStatus()
{    
#if !UE_SERVER
    return  FSdkInterface::GetIns()->QueryAgeGateStatus();
#endif
    return true;
}

FString ULuaManger::GetSdkOpenId()
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->GetSdkOpenId();
#endif
    return FString();
}

void ULuaManger::AddLocalNotification(const FString& title,const FString& content, uint64 nDelaySeconds)
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->AddLocalNotification(title, content, nDelaySeconds);
#endif
}

int32 ULuaManger::GetCurrentNetState()
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->GetCurrentNetState();
#endif
    return 1;
}

void ULuaManger::VerifyActivationCode(const FString& strCode)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->VerifyActivationCode(strCode);
#endif
}


void ULuaManger::OpenUrlBySdk(const FString& title, const FString& url, const FString& extraData)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->OpenUrl(title, url, extraData);
#endif
}

void ULuaManger::RealNameVerify(const FString& url, bool bMustSuccess, const FSdkLoginDelegate& realNameCallback)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->RealNameVerify(url, bMustSuccess);
#endif
}

void ULuaManger::GetAntiAddictionState()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->GetAntiAddictionState();
#endif
}

void ULuaManger::CDKeyExchange(const FString& cdkey, const FString& roleid, const FString& serverid, const FSdkBoolIntStringDelegate& cdkeyCallback)
{
#if !UE_SERVER
    OnCdKeyExchangeCallback = std::move(cdkeyCallback);
    FSdkInterface::GetIns()->CDKeyExchange(cdkey, roleid, serverid);
#endif
}

void ULuaManger::NetDiagnosis(const  FString& target, int timeout, const FSdkBoolIntStringDelegate& callback)
{
#if !UE_SERVER
    OnNetDiagnosisCallback = std::move(callback);
    FSdkInterface::GetIns()->NetDiagnosis(target, timeout);
#endif
}

void ULuaManger::FetchNotice(int nScene, const FString& savePath, const FString& strLanguage, const FString& otherParams, const FSdkFetchNoticeDelegate& callback)
{
#if !UE_SERVER
    OnFetchNoticeCallback = std::move(callback);
    FSdkInterface::GetIns()->FetchNotice(nScene, savePath, strLanguage, otherParams);
#endif 
}

FString ULuaManger::GetRtcToken(const FString& roomId, const FString& userId, const FString& appId, const FString& appkey, int32 nExpireTime)
{
    if(nExpireTime <= 0)
        nExpireTime = 7200;
    rtc::tools::AccessToken token(TCHAR_TO_UTF8(*appId), TCHAR_TO_UTF8(*appkey), TCHAR_TO_UTF8(*roomId),TCHAR_TO_UTF8(*userId));
    int utcExpireTime = FDateTime::UtcNow().ToUnixTimestamp() + nExpireTime;
    token.ExpireTime(utcExpireTime);
    token.AddPrivilege(rtc::tools::AccessToken::Privileges::PrivSubscribeStream, utcExpireTime);
    token.AddPrivilege(rtc::tools::AccessToken::Privileges::PrivPublishStream, utcExpireTime);
    FString strToken(UTF8_TO_TCHAR(token.Serialize().c_str()));
    return strToken;
}


void ULuaManger::JoinRoom(const FString& roomId, const FString& userId, const FString& token, int roomType, int getspeakerInterval, bool enableRangeAudio, const FOnVoiceJoinRoomResult& callback)
{
#if !UE_SERVER
    if(roomId.Len() == 0 || userId.Len() == 0 || token.Len() == 0)
        return;
    if(OnJoinRomResult.IsBound())
        UnLua::DelegateUnBind(&OnJoinRomResult);
    OnJoinRomResult = std::move(callback);
    FVoiceInterface::GetIns()->JoinRoom(roomId, userId, token, roomType, getspeakerInterval, enableRangeAudio);
#endif 
}

void ULuaManger::LeaveRoom(const FOnVoiceLeaveRoom& callback)
{
#if !UE_SERVER
    if(OnLeaveRoom.IsBound())
        UnLua::DelegateUnBind(&OnLeaveRoom);
    OnLeaveRoom = std::move(callback);
    FVoiceInterface::GetIns()->LeaveRoom();
#endif
}

void ULuaManger::UpdateToken(const FString& token)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->UpdateToken(token);
#endif
}

void ULuaManger::SetWorldRoomReceivelRange(int disMin, int disMax)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->SetWorldRoomReceivelRange(disMin,disMax);
#endif
}

void ULuaManger::UpdatePostion(double posX, double posY, double posZ)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->UpdatePostion((int)posX,(int)posY,(int)posZ);
#endif
}

void ULuaManger::EnableMicrophone(bool bEnable)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->EnableMicrophone(bEnable);
#endif
}

void ULuaManger::EnableAudioReceive(const FString& userId, bool bEnable)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->EnableAudioReceive(userId, bEnable);
#endif
}

void ULuaManger::EnableSpeakerphone(bool bEnable)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->EnableSpeakerphone(bEnable);
#endif
}

void ULuaManger::SetGetSpeakersCallback(const FOnVoiceSpeakerList& callback)
{
#if !UE_SERVER
    if(OnSpeakerList.IsBound())
        UnLua::DelegateUnBind(&OnSpeakerList);
    OnSpeakerList = std::move(callback);
#endif
}

void ULuaManger::SetSpeakerVolumeChangedCallback(const FOnSpeakerVolumeChanged& callback)
{
#if !UE_SERVER
    if(OnSpeakerVolumeCallback.IsBound())
        UnLua::DelegateUnBind(&OnSpeakerVolumeCallback);
    OnSpeakerVolumeCallback = std::move(callback);
#endif
}


void ULuaManger::SetRecordingVolume(int volume)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->SetRecordingVolume(volume);
#endif
}

void ULuaManger::SetPlaybackVolume(int volume,const FString& userId)
{
#if !UE_SERVER
    FVoiceInterface::GetIns()->SetPlaybackVolume(volume, userId);
#endif
}

void ULuaManger::SetUserInfoForSecureService(const FString& roleId, const FString& serverId)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->SetUserInfoForSecureService(roleId, serverId);
#endif
}

void ULuaManger::AddLevelNameToGPM(const FString& strLevelName)
{
    FLevelCallback::Get()->AddLevelNameToGPM(strLevelName);
}

bool ULuaManger::IsPermissionGranted(const FString& strPermission)
{
#if !UE_SERVER
       return   FSdkInterface::GetIns()->IsPermissionGranted(strPermission);
#else
    return true;
#endif
    
}

void ULuaManger::OpenAppSetting()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->OpenAppSetting();
#endif
}

bool ULuaManger::RequestPermission(const FString& strPermission, bool bFirstRequest)
{
#if !UE_SERVER
    return FSdkInterface::GetIns()->RequestPermission(strPermission, bFirstRequest);
#endif
    return true;
}


bool ULuaManger::IsNotificationsAllowed(const FString& strChannelName)
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->IsNotificationsAllowed(strChannelName);
#else
    return true;
#endif
}

void ULuaManger::GetCountryCode(const FSdkBoolIntThreeStringsDelegate& callback)
{
#if !UE_SERVER
    if(OnGetCountryCodeCallback.IsBound())
        UnLua::DelegateUnBind(&OnGetCountryCodeCallback);
    OnGetCountryCodeCallback = std::move(callback);
    FSdkInterface::GetIns()->GetCountryCode();
#endif
    
}

void ULuaManger::ChangeLanguage(const FString& langType)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->ChangeLanguage(langType);
#endif    
}

void ULuaManger::ShowRatingUi()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->ShowRatingUi();
#endif  
}

void ULuaManger::ShowRatingCustomUi()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->ShowRatingCustomUi();
#endif  
}

void ULuaManger::DismissRatingCustomUi()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->DismissRatingCustomUi();
#endif  
}

void ULuaManger::OnRatingPraiseBtnClick()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->OnRatingPraiseBtnClick();
#endif  
}


void ULuaManger::OnRatingFeedbackBtnClick()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->OnRatingFeedbackBtnClick();
#endif  
}

bool ULuaManger::IsBsdkPay()
{
#if !UE_SERVER
   return  FSdkInterface::GetIns()->IsBsdkPay();
#endif
    return true;
}

void ULuaManger::Pay(const FString& productId, int goodType, int amount, const FString& currency, const FString& roleId, const FString& roleName, int roleLevel, int roleVipLevel, const FString& serverId, const FString& extInfo, const FSdkBoolIntThreeStringsDelegate& OnPay)
{
#if !UE_SERVER
    if(OnPayCallback.IsBound())
        UnLua::DelegateUnBind(&OnPayCallback);
    OnPayCallback = std::move(OnPay);
    FSdkInterface::GetIns()->Pay(productId, goodType, amount, currency,roleId, currency, roleLevel, roleVipLevel, serverId, extInfo);
#endif 
}

void ULuaManger::QueryProducts(const FString& productId, const FSdkBoolIntTwoStringsDelegate& OnQueryProductBack)
{
#if !UE_SERVER
    if(OnQueryProductInfoCallback.IsBound())
        UnLua::DelegateUnBind(&OnQueryProductInfoCallback);
    OnQueryProductInfoCallback = std::move(OnQueryProductBack);
    FSdkInterface::GetIns()->QueryProducts(productId);
#endif   
}

void ULuaManger::QueryGoods(const FString& roleId, const FString& serverId, bool bCurrency, const FSdkBoolIntStringDelegate& OnQueryGoodsBack)
{
#if !UE_SERVER
    if(OnQueryGoodsCallback.IsBound())
        UnLua::DelegateUnBind(&OnQueryGoodsCallback);
    OnQueryGoodsCallback = std::move(OnQueryGoodsBack);
    FSdkInterface::GetIns()->QueryGoods(roleId, serverId, bCurrency);
#endif 
}

void ULuaManger::SetMinorLimit(const FString& country, const FString& content, const FSdkBoolIntStringDelegate& OnSetMinorLimitBack)
{
#if !UE_SERVER
    if(OnSetMinorLimitCallback.IsBound())
        UnLua::DelegateUnBind(&OnSetMinorLimitCallback);
    OnSetMinorLimitCallback = std::move(OnSetMinorLimitBack);
    FSdkInterface::GetIns()->SetMinorLimit(country, content);
#endif  
}

void ULuaManger::GetMinorLimit(const FString& country, const FSdkBoolIntStringDelegate& OnGetMinorLimitBack)
{
#if !UE_SERVER
    if(OnGetMinorLimitCallback.IsBound())
        UnLua::DelegateUnBind(&OnGetMinorLimitCallback);
    OnGetMinorLimitCallback = std::move(OnGetMinorLimitBack);
    FSdkInterface::GetIns()->GetMinorLimit(country);
#endif
}

void ULuaManger::HasOpenFriendChainPermission(int32 nPlatformId, const FSdkBoolLongDelegate& OnCallBack)
{
#if !UE_SERVER
    if(OnGetFriendChainPermission.IsBound())
        UnLua::DelegateUnBind(&OnGetFriendChainPermission);
    OnGetFriendChainPermission = std::move(OnCallBack);
    FSdkInterface::GetIns()->HasOpenFriendChainPermission(nPlatformId);
#endif
    
}

void ULuaManger::GetFriendChainInfo(int32 nPlatformId, const FSdkBoolTwoIntStringDelegate& OnCallBack)
{
#if !UE_SERVER
    if(OnGetFriendChainInfo.IsBound())
        UnLua::DelegateUnBind(&OnGetFriendChainInfo);
    OnGetFriendChainInfo = std::move(OnCallBack);
    FSdkInterface::GetIns()->GetFriendChainInfo(nPlatformId);
#endif  
}

bool ULuaManger::UnZip(const FString& filePath, const FString& destDir, bool bSoZip, bool bDeleteFile)
{
#if !UE_SERVER
    bool bRe = FSdkInterface::GetIns()->UnZip(filePath, destDir, bSoZip);
    if(bRe && bDeleteFile)
        IFileManager::Get().Delete(*filePath);
    return bRe;
#endif
    if(bDeleteFile)
        IFileManager::Get().Delete(*filePath);
    return true;
}

FString ULuaManger::GetChannelOp()
{
#if !UE_SERVER
    return  FSdkInterface::GetIns()->GetChannelOp();
#endif
    return "";
}

void ULuaManger::ChannelGeneralAction(int32 code)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->ChannelGeneralAction(code);
#endif
}

void ULuaManger::NetMpaStart(const FString& startLog)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->NetMpaStart(startLog);
#endif
}

bool ULuaManger::NetMpaAddAddress(const FString& host, int port,const FString& contextId, bool bChangeHost,FString& finalUrl, const FSdkDelegate& callback)
{
#if !UE_SERVER
    if(host.IsEmpty())
        return false;
    if(OnNetMpaAddAddressCallback.IsBound())
        UnLua::DelegateUnBind(&OnNetMpaAddAddressCallback);
    OnNetMpaAddAddressCallback = std::move(callback);
    bool isIp = true;
    for(int i = 0 ; i < host.Len(); i++)
    {
        if(host[i] != TEXT('.') && !FChar::IsDigit(host[i]))
        {
            isIp = false;
            break;
        }
    }
    TArray<FString> accAddress;
    if(isIp)
    {
        finalUrl = host;
        accAddress.Add(FString::Printf(TEXT("%s:%d"),*host, port));
        return FSdkInterface::GetIns()->NetMpaAddAddress(accAddress,contextId );
    }
    FString strUrl = host;
    if(bChangeHost)
    {
        int nPos = 0;
        if(!host.FindChar(TEXT('.'), nPos))
            return false;
        strUrl.InsertAt(nPos,TEXT("-gna"));
    }
    finalUrl = strUrl;
    ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    auto AsyncResolverHandler = [strUrl,port,contextId](FAddressInfoResult Results)
    {
        AsyncTask(ENamedThreads::GameThread, [strUrl,port,contextId,Results]()
        {
            if (Results.ReturnCode != SE_NO_ERROR || Results.Results.Num() == 0)
            {
                UE_LOG(GAME, Error, TEXT("NetMpaAddAddress socket,getaddresinfo,error,%s"), *strUrl);
                Get()->OnNetMpaAddAddressCallback.ExecuteIfBound(false,TEXT("getaddresinfo error"));
                return;
            }        
            TArray<FString> accAdress;
            for (auto& item : Results.Results)
            {
                accAdress.Add(FString::Printf(TEXT("%s:%d"), *item.Address->ToString(false), port));
            }
            if(!FSdkInterface::GetIns()->NetMpaAddAddress(accAdress,contextId))
                Get()->OnNetMpaAddAddressCallback.ExecuteIfBound(false,TEXT("java NetMpaAddAddress return false"));
                
        });
    };
       
    SocketsSystem->GetAddressInfoAsync(AsyncResolverHandler, *strUrl, *FString::Printf(TEXT("%d"), port),
        EAddressInfoFlags::AllResultsWithMapping | EAddressInfoFlags::OnlyUsableAddresses, NAME_None, ESocketType::SOCKTYPE_Datagram);
    return true;
#endif
    return false;
}

void ULuaManger::NetMpaStop(const FString& stopLog)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->NetMpaStop(stopLog);
#endif
}

void ULuaManger::AddUploadLogfileFullPaths(const TArray<FString>& files)
{
#if !UE_SERVER
    TArray<FString>toLogFiles(files);
    FString logPath = FString::Printf(TEXT("%s%s.log"), *FPaths::ProjectLogDir(), FApp::GetProjectName());
    FString logFileName = FPaths::GetCleanFilename(logPath);
    IFileManager& FileManager = IFileManager::Get();    

    for(auto& item:toLogFiles)
    {
        if(item.EndsWith(logFileName))
        {
            FDateTime OriginalTime = FileManager.GetTimeStamp(*logPath);
            FString BackupFilename = FString::Printf(TEXT("%s%s%s%s.log"), *FPaths::ProjectLogDir(), FApp::GetProjectName(),BACKUP_LOG_FILENAME_POSTFIX,*OriginalTime.ToString());
            toLogFiles.Add(BackupFilename);
            break;
        }
    }
		
    FSdkInterface::GetIns()->AddUploadLogfileFullPaths(toLogFiles);
#endif
}

void ULuaManger::AddUploadLogfileDirPath(const FString& strDir)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->AddUploadLogfileDirPath(strDir);
#endif
}

void ULuaManger::GetSdkFriends()
{
#if !UE_SERVER
    FSdkInterface::GetIns()->GetFriends();
#endif
}

void ULuaManger::InviteFriend(const FString& friendID,const FString& InviteInfo)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->InviteFriend(friendID, InviteInfo);
#endif
}

void ULuaManger::SendMsgToFriend(const FString& friendID, const FString& msg)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->SendMsgToFriend(friendID, msg);
#endif
}



void ULuaManger::UploadException(const FString&  name, const FString&  reason, const FString&  stackTrace)
{
#if !UE_SERVER
    FSdkInterface::GetIns()->UploadException(name, reason, stackTrace);
#endif
}

void ULuaManger::Log(int nLevel, FString& strModuleName, FString& strLogContent) const
{
    switch (nLevel)
    {
    case ELogVerbosity::Fatal:
        UE_LOG(GAME, Fatal, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    case ELogVerbosity::Error:
        UE_LOG(GAME, Error, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    case ELogVerbosity::Warning:
        UE_LOG(GAME, Warning, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    case ELogVerbosity::Display:
        UE_LOG(GAME, Display, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;    
    case ELogVerbosity::Log:
        UE_LOG(GAME, Log, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    case ELogVerbosity::Verbose:
        UE_LOG(GAME, Verbose, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    case ELogVerbosity::VeryVerbose:
        UE_LOG(GAME, VeryVerbose, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    default:
        UE_LOG(GAME, Display, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
        break;
    }
}

ULocalPlayer* ULuaManger::GetFirstLocalPlayer()
{
    return GEngine->GetFirstGamePlayer(GetWorld());
}

void ULuaManger::ExecuteCommand(FString Command) const
{
    GEngine->Exec(GetWorld(),*Command);
}

DEFINE_LOG_CATEGORY(GAME)

class USelfGameInstanceEx : public USelfGameInstance
{
public:
	void SetWorldContext(FWorldContext* InWorldContext) { WorldContext = InWorldContext; }
};

class USelfViewportClientEx : public UGameViewportClient
{
public:

	void ResetWorldContext(struct FWorldContext& WorldContext)
	{
		WorldContext.AddRef(static_cast<UWorld*&>(World));
	}
};


UGameInstance* ULuaManger::GetGameInstance()
{
	return m_pGame;
}

UObject* ULuaManger::LoadRes(const FString& ResPath)
{
    UObject* pObj = StaticLoadObject(UObject::StaticClass(),nullptr, *ResPath);
    if(pObj)
        UnLua::AddObjectReferencer(pObj);
    return pObj;
}

void ULuaManger::LoadResAsync(const FString& ResPath, const FResLoadDelegate& Callback, int32 Priority, bool bCallbackAutoRelease)
{
	UAssetManager::GetStreamableManager().RequestAsyncLoad(ResPath, [ResPath, Callback,bCallbackAutoRelease]()
	{
		UObject *ResObject = StaticFindObject(UObject::StaticClass(), nullptr, *ResPath);
		if (ResObject == nullptr)
		{
			UE_LOG(GAME, Warning, TEXT("[LoadResAsync]load res %s failed"), *ResPath);
        }
	    UnLua::AddObjectReferencer(ResObject);
        Callback.ExecuteIfBound(ResPath, ResObject);
	    if(bCallbackAutoRelease)
	        UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
	}, Priority);
}

void ULuaManger::LoadMultiResAsync(TArray<FString>& ResPathList, const FMutliResLoadDelegate& Callback,
    int32 Priority, bool bCallbackAutoRelease)
{
    if (ResPathList.Num() > 0)
    {
        TArray<FSoftObjectPath> SoftPathList;
        for (FSoftObjectPath path : ResPathList)
        {
            SoftPathList.Add(path);
        }
        
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(SoftPathList, FStreamableDelegate::CreateLambda(
     [ResPathList, SoftPathList, Callback,bCallbackAutoRelease]()
         {
             TArray<UObject*> loadedObjects;
             for (FSoftObjectPath path : SoftPathList)
             {
                 loadedObjects.Add(path.ResolveObject());
                 UnLua::AddObjectReferencer(path.ResolveObject());
             }
                
             Callback.Execute(ResPathList, loadedObjects);
            if(bCallbackAutoRelease)
                UnLua::ReleaseLuaCallback(Callback.GetUObject(),Callback.GetFunctionName().ToString());
         }), Priority);
    }
    else
    {
        UE_LOG(GAME, Warning, TEXT("[LoadMultiResAsync]ResPathList is empty"));
        Callback.Execute(ResPathList, TArray<UObject*>());
        if(bCallbackAutoRelease)
            UnLua::ReleaseLuaCallback(Callback.GetUObject(),Callback.GetFunctionName().ToString());
    }
}

UObject* ULuaManger::CreateObject(const UClass* Class)
{
	check(m_pGame);
	
	if (!Class)
	{
		UE_LOG(GAME, Error, TEXT("[CreateObject]Class is null."));
		return nullptr;
	}

	return NewObject<UObject>(m_pGame, Class);
}

UUserWidget* ULuaManger::CreateUserWidget(TSubclassOf<UUserWidget> WidgetType)
{
	check(m_pGame);
	return UWidgetBlueprintLibrary::Create(m_pGame, WidgetType, UGameplayStatics::GetPlayerController(m_pGame,0));
}

UUserWidget* ULuaManger::CreateUserWidgetFromPath(const FString& ResPath, bool bAsync,const FResLoadDelegate& Callback, int32 Priority, bool bCallbackAutoRelease)
{
    if(ResPath.Len() == 0)
    {
        UE_LOG(GAME, Warning, TEXT("[CreateUserWidgetFromPath]ResPathList is empty"));
        if(bAsync)
        {
            Callback.ExecuteIfBound(ResPath, nullptr);
            if(bCallbackAutoRelease)
                UnLua::ReleaseLuaCallback(Callback.GetUObject(),Callback.GetFunctionName().ToString());
        }
        return nullptr;
    }
    if(!bAsync)
    {
        UClass* pResClass = Cast<UClass>(StaticLoadObject(UObject::StaticClass(),nullptr, *ResPath));
        if(!pResClass)
            return nullptr;
        return UWidgetBlueprintLibrary::Create(m_pGame, pResClass, nullptr);
    }
    UAssetManager::GetStreamableManager().RequestAsyncLoad(ResPath, [ResPath, Callback,bCallbackAutoRelease]()
    {
        UClass* pResClass = Cast<UClass>(StaticFindObject(UObject::StaticClass(), nullptr, *ResPath));
        if (!pResClass)
        {
            UE_LOG(GAME, Warning, TEXT("[LoadResAsync]load res %s failed"), *ResPath);            
        }
        UUserWidget* WidgetObj = UWidgetBlueprintLibrary::Create(m_pGame, pResClass, nullptr);
        Callback.ExecuteIfBound(ResPath, WidgetObj);
        if(bCallbackAutoRelease)
            UnLua::ReleaseLuaCallback(Callback.GetUObject(),Callback.GetFunctionName().ToString());
    }, Priority);
    return nullptr;
}

FName ULuaManger::LoadWorld(const FString& LevelPath)
{
	check(m_pGame);
	FWorldContext* CurrentWorldContext = m_pGame->GetWorldContext();
	check(CurrentWorldContext);
	
	UWorld* CurrentWorld = CurrentWorldContext->World();
	check(CurrentWorld);
    
    UWorld::WorldTypePreLoadMap.FindOrAdd(FName(*LevelPath)) = EWorldType::GamePreview;
    
    UPackage *WorldPackage = LoadPackage(nullptr, *LevelPath, ELoadFlags::LOAD_None);
    if (!WorldPackage)
    {
        UE_LOG(GAME, Error, TEXT("[LoadWorld]load level package %s failed."), *LevelPath);
        return NAME_None;
    }
	
	UWorld* NewWorld = UWorld::FindWorldInPackage(WorldPackage);
    if (!NewWorld)
    {
        UE_LOG(GAME, Error, TEXT("[LoadWorld]load world in package %s failed."), *LevelPath);
        return NAME_None;
    }
	
#if WITH_EDITOR
    if(GIsEditor)
    {
        FString PIEWorldPackageName = UWorld::ConvertToPIEPackageName(LevelPath, CurrentWorldContext->PIEInstance);
        UPackage* PlayWorldPackage = CreatePackage(*PIEWorldPackageName);

        // WorldType is setted to GamePreview and not to Game to avoid that the
        // function UEditorEngine::VerifyLoadMapWorldCleanup() blame us for a false memory leak
        UWorld::WorldTypePreLoadMap.FindOrAdd(FName(*PIEWorldPackageName)) = EWorldType::GamePreview;

        PlayWorldPackage->SetPackageFlags(PKG_PlayInEditor);
        PlayWorldPackage->PIEInstanceID = CurrentWorldContext->PIEInstance;
        PlayWorldPackage->FileName = WorldPackage->FileName;
        PlayWorldPackage->MarkAsFullyLoaded();

        GPlayInEditorID = PlayWorldPackage->PIEInstanceID;

        NewWorld = CastChecked<UWorld>(StaticDuplicateObject(
            NewWorld,				// Source root
            PlayWorldPackage,		// Destination root
            NewWorld->GetFName(),	// Name for new object
            RF_AllFlags,			// FlagMask
            NULL,					// DestClass
            EDuplicateMode::PIE
        ));

        if (PlayWorldPackage->PIEInstanceID != -1)
        {
            // Store prefix we used to rename this world and streaming levels package names
            NewWorld->StreamingLevelsPrefix = UWorld::BuildPIEPackagePrefix(PlayWorldPackage->PIEInstanceID);
        }

        GPlayInEditorID = -1;
    }
#endif //WITH_EDITOR
    

    NewWorld->AddToRoot();
    NewWorld->ClearFlags(RF_Standalone);

    FAudioDeviceManager* AudioDeviceManager = GEngine->GetAudioDeviceManager();
    check(AudioDeviceManager);
    FAudioDeviceHandle AudioDeviceHandle = CurrentWorld->GetAudioDevice();
    NewWorld->AudioDeviceHandle = AudioDeviceHandle;
    AudioDeviceManager->RegisterWorld(NewWorld, AudioDeviceHandle.GetDeviceID());

	const EWorldType::Type WorldType = CurrentWorldContext->WorldType;
	NewWorld->WorldType = WorldType;
	NewWorld->SetGameInstance(m_pGame);
    NewWorld->FeatureLevel = CurrentWorld->FeatureLevel;
    NewWorld->InitWorld();
	NewWorld->SetShouldTick(true);
    NewWorld->bAllowAudioPlayback = false;

	FWorldContext& NewWorldContext = GEngine->CreateNewWorldContext(WorldType);
    NewWorldContext.AudioDeviceID = AudioDeviceHandle.GetDeviceID();
	NewWorldContext.SetCurrentWorld(NewWorld);
	NewWorldContext.OwningGameInstance = m_pGame;
	NewWorldContext.GameViewport = CurrentWorldContext->GameViewport;
    NewWorldContext.PIEInstance = CurrentWorldContext->PIEInstance;
    NewWorldContext.PIEPrefix = CurrentWorldContext->PIEPrefix;
    NewWorldContext.PIEWorldFeatureLevel = CurrentWorldContext->PIEWorldFeatureLevel;

#if WITH_EDITOR
    if (GEditor)
    {
        auto SlatePlayInEditorInfo = GEditor->SlatePlayInEditorMap.Find(CurrentWorldContext->ContextHandle);
        if (SlatePlayInEditorInfo)
        {
            GEditor->SlatePlayInEditorMap.Add(NewWorldContext.ContextHandle, *SlatePlayInEditorInfo);
        }
    }
#endif // WITH_EDITOR
	
	SetActiveWorldContext(&NewWorldContext);
	
	FURL URL;
    NewWorld->SetGameMode(URL);
	NewWorld->InitializeActorsForPlay(URL);

    FString OutError;
    for (auto LocalPlayer : m_pGame->GetLocalPlayers())
    {
        // SpawnPlayActor overwrite the LocalPlayer PC but we don't want it!
        // backup the current player controller
        auto OldPC = LocalPlayer->PlayerController;

        LocalPlayer->SpawnPlayActor(URL.ToString(), OutError, NewWorld);

        auto NewPC = LocalPlayer->PlayerController;
        NewWorld->RemoveNetworkActor(NewPC);

        // restore the player controller
        LocalPlayer->PlayerController = OldPC;
    }
    
	NewWorld->BeginPlay();

    for (auto Level : NewWorld->GetLevels())
    {
        for (auto Actor : Level->Actors)
        {
            NewWorld->RemoveNetworkActor(Actor);
        }
    }
	
	SetActiveWorldContext(CurrentWorldContext);

#if WITH_EDITOR
    NewWorldContext.GameViewport = nullptr;
    NewWorldContext.RunAsDedicated = true;
#endif // WITH_EDITOR
    
	return NewWorldContext.ContextHandle;
}

FName ULuaManger::SwitchWorld(FName ToWorldContextHandle)
{
	check(m_pGame);

	FWorldContext* ToWorldContext = GEngine->GetWorldContextFromHandle(ToWorldContextHandle);
	if (!ToWorldContext)
	{
		UE_LOG(GAME, Error, TEXT("[SwitchWorld]find worldcontext %s failed."), *ToWorldContextHandle.ToString());
		return NAME_None;
	}

	UWorld *ToWorld = ToWorldContext->World();
	if (!ToWorld)
	{
		UE_LOG(GAME, Error, TEXT("[SwitchWorld]world is null:%s"), *ToWorldContextHandle.ToString());
		return NAME_None;
	}
	FWorldContext* CurrentWorldContext = m_pGame->GetWorldContext();
	check(CurrentWorldContext);
	if (CurrentWorldContext == ToWorldContext)
		return ToWorldContextHandle;
	
	UWorld* CurrentWorld = CurrentWorldContext->World();
	check(CurrentWorld);
	if (ToWorld == CurrentWorld)
		return ToWorldContextHandle;
	
	for (auto LocalPlayer : m_pGame->GetLocalPlayers())
	{
	    if (LocalPlayer->PlayerController)
	    {
	        LocalPlayer->PlayerController->PlayerInput->FlushPressedKeys();
	    }
	}

    ToWorld->SetShouldTick(true);

#if WITH_EDITOR
	ToWorldContext->GameViewport = CurrentWorldContext->GameViewport;
	ToWorldContext->RunAsDedicated = false;
	CurrentWorldContext->GameViewport = nullptr;
	CurrentWorldContext->RunAsDedicated = true;
#endif // WITH_EDITOR

	SetActiveWorldContext(ToWorldContext);

    for (auto LocalPlayer : m_pGame->GetLocalPlayers())
    {
        bool bLocalPlayerMoved(false);

        for (FConstPlayerControllerIterator Iterator = ToWorld->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            APlayerController* PlayerController = Iterator->Get();
            if (PlayerController && PlayerController->GetLocalPlayer() == LocalPlayer)
            {
                LocalPlayer->PlayerController = PlayerController;
                bLocalPlayerMoved = true;
                break;
            }
        }

        if(!bLocalPlayerMoved)
        {
            UE_LOG(GAME, Warning, TEXT("Unable to find the player controller for the local player with id=%d"), LocalPlayer->GetControllerId());
        }
    }

    FAudioDeviceManager* AudioDeviceManager = GEngine->GetAudioDeviceManager();
    check(AudioDeviceManager);

    AudioDeviceManager->SetActiveDevice(ToWorld->GetAudioDevice().GetDeviceID());
	
	return CurrentWorldContext->ContextHandle;
}

void ULuaManger::SetWorldTick(UWorld* world, bool bEnable)
{
    check(m_pGame);

    if (!world)
    {
        UE_LOG(GAME, Error, TEXT("[SetWorldTick]world is null"));
        return;
    }
    world->SetShouldTick(bEnable);
}

UWorld* ULuaManger::GetWorldByWorldContextHandle(FName WorldContextHandle)
{
    FWorldContext* WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    return WorldContext ? WorldContext->World() : nullptr;
}

FName ULuaManger::GetCurrentWorldContextHandle()
{
    check(m_pGame);
    return m_pGame->GetWorldContext()->ContextHandle;
}

UObject* ULuaManger::NewEmptyUObject(UObject* Outer)
{
    UObject* pObj = NewObject<UEmptyObject>();
    UnLua::AddObjectReferencer(pObj);
    return pObj;
}

UUserWidget* ULuaManger::GetListViewEntryOfItem(UListView* ListView, UObject* Item)
{
    if (ListView != nullptr && Item != nullptr)
    {
        return ListView->GetEntryWidgetFromItem(Item);
    }

    if (IsValid(ListView))
    {
        UE_LOG(GAME, Warning, TEXT("ULuaManger::GetListViewEntryOfItem wrong arg, ListView name : %s, Item is nil : %s"), *ListView->GetName(),Item?TEXT("False"):TEXT("True"))
    }
    else
    {
        UE_LOG(GAME, Warning, TEXT("ULuaManger::GetListViewEntryOfItem wrong arg, ListView is nil, Item is nil : %s"), Item?TEXT("False"):TEXT("True"))
    }
    return nullptr;
}

UObject* ULuaManger::GetListViewItemOfEntry(const UListView* ListView, const UUserWidget* entry)
{
    if (ListView != nullptr && entry != nullptr)
    {
        auto object = ListView->ItemFromEntryWidget(*entry);
        if (object)
            return *object;
    }

    UE_LOG(GAME, Warning, TEXT("ULuaManger::GetListViewItemOfEntry wrong arg"))
    return nullptr;
}

void ULuaManger::SetTexture2DPixel(UTexture2D* texture2D, int row, int column, uint8 value)
{
    if (texture2D == nullptr)
    {
        UE_LOG(GAME, Warning, TEXT("ULuaManger::SetTexture2DPixel Texture Is Null"))
        return;
    }
    const FPixelFormatInfo PF = GPixelFormats[texture2D->GetPixelFormat()];
    const int32 SrcBpp = PF.BlockSizeX;
    int32 Width = texture2D->GetSizeX();
    int32 Height = texture2D->GetSizeY();
    FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(FMath::Clamp(row, 0, Width), FMath::Clamp(column, 0, Height)
                                                                      , 0
                                                                      , 0
                                                                      , 1,1);
    uint8* data = new uint8(value);
    
    TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc =
    [](uint8* SrcData, const FUpdateTextureRegion2D* Regions)
    {
        delete SrcData;
        delete Regions;
    };
    texture2D->UpdateTextureRegions(0,1,Region,Width*SrcBpp,SrcBpp,data,DataCleanupFunc);
}

void ULuaManger::RefreshMeshPose(USkeletalMeshComponent* Mesh)
{
    if (Mesh&&Mesh->AnimScriptInstance)
    {
        Mesh->AnimScriptInstance->UpdateAnimation(0.f,true);
        Mesh->RefreshBoneTransforms();
    }
}

void ULuaManger::SetActiveWorldContext(FWorldContext* InWorldContext)
{
	check(InWorldContext)
	check(m_pGame)

	Cast<USelfGameInstanceEx, UGameInstance>(m_pGame)->SetWorldContext(InWorldContext);
	if (UGameViewportClient* ThisGameViewport = m_pGame->GetGameViewportClient())
	{
		Cast<USelfViewportClientEx, UGameViewportClient>(ThisGameViewport)->ResetWorldContext(*InWorldContext);
	}
}

void ULuaManger::OnActorRelevantChange(AActor* Actor, bool IsRelevant)
{
    for(TMap<UClass*,FOnActorRelevantChange>::TIterator Iterator = Class2RelevantChangeCallBack.CreateIterator();Iterator;++Iterator)
    {
        UClass* Class = Iterator.Key();
        if(Actor->IsA(Class))
        {
            FOnActorRelevantChange OnActorRelevantChange = Iterator.Value();
            OnActorRelevantChange.ExecuteIfBound(Actor,IsRelevant);
            break;
        }
    }
}

void ULuaManger::CreateProc(FString URL, FString Params,bool bLaunchDetached, bool bLaunchHidden, bool bLaunchReallyHidden)
{
    FPlatformProcess::CreateProc(*URL, *Params, bLaunchDetached,bLaunchHidden,bLaunchReallyHidden,nullptr, 0, nullptr, nullptr);
}

void ULuaManger::OnPostLoadMapWithWorld(UWorld* World)
{
    if(IsRunningDedicatedServer())
    {
        if(UReplicationDriver* ReplicationDriver = World->NetDriver->GetReplicationDriver())
        {
            if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
            {
                SimpleReplicationDriver->OnRelevantChange.AddUObject(this,&ULuaManger::OnActorRelevantChange);
                for(TMap<UClass*,FOnActorRelevantChange>::TIterator Iterator = Class2RelevantChangeCallBack.CreateIterator();Iterator;++Iterator)
                {
                    NeedNotifyClasses.Add(Iterator.Key());
                }
            }        
        }
    }
}

bool ULuaManger::ExistPendingGame(const FName& WorldContextHandle)
{
    FWorldContext* WorldContext = GEngine->GetWorldContextFromHandle(WorldContextHandle);
    if (!WorldContext)
    {
        UE_LOG(GAME, Error, TEXT("[ExistPendingGame]wrong WorldContextHandle %s"), *WorldContextHandle.ToString())
        return false;
    }
    return WorldContext->PendingNetGame!=nullptr;
}

// const USkySubsystem* ULuaManger::GetSkySystem() const
// {
//     if(IsValid(GetWorld()))
//         return GetWorld()->GetSubsystem<USkySubsystem>();
//     else
//         return nullptr;
// }

void ULuaManger::DumpReplication()
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->DumpInfo();
        }        
    }
}

void ULuaManger::AddToReplicationNormalActors(AActor* Actor)
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->AddToNormalActors(Actor);
        }        
    }
}

void ULuaManger::RemoveReplicationFromNormalActors(AActor* Actor)
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->RemoveFromNormalActors(Actor);
        }        
    }
}

bool ULuaManger::ServerTravel(const FString& InURL, bool bAbsolute, bool bShouldSkipGameNotify)
{
    return GetWorld()->ServerTravel(InURL,bAbsolute,bShouldSkipGameNotify);
}

void ULuaManger::OnWindowActivatedEvent()
{
    if (OnWindowActiveEvent.IsBound())
    {
        OnWindowActiveEvent.Broadcast();
    }
}

void ULuaManger::OnWindowDeactivatedEvent()
{
    if (OnWindowDeactiveEvent.IsBound())
    {
        OnWindowDeactiveEvent.Broadcast();
    }
}

void ULuaManger::BindWindowEvent()
{
    if(m_pGame && m_pGame->GetGameViewportClient() && m_pGame->GetGameViewportClient()->GetWindow())
    {
        m_pGame->GetGameViewportClient()->GetWindow()->GetOnWindowActivatedEvent().AddUObject(this,&ULuaManger::OnWindowActivatedEvent);
    }

    if(m_pGame && m_pGame->GetGameViewportClient() && m_pGame->GetGameViewportClient()->GetWindow())
    {
        m_pGame->GetGameViewportClient()->GetWindow()->GetOnWindowDeactivatedEvent().AddUObject(this,&ULuaManger::OnWindowDeactivatedEvent);
    }
}

void ULuaManger::AddRelevantChangeCallback(UClass* Class, FOnActorRelevantChange Callback)
{
    Class2RelevantChangeCallBack.Emplace(Class,Callback);
}

void ULuaManger::SearchPlayersAt(FVector Location, float Distance, TArray<AActor*>& Actors) const
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->SearchPlayersAt(Location,Distance,Actors);
        }        
    }
}

void ULuaManger::SearchActorsAt(FVector Location, float Distance, TArray<AActor*>& Actors, UClass* Class) const
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->SearchActorsAt(Location,Distance,Actors,Class);
        }        
    }
}

void ULuaManger::SearchTinderActorsAt(FVector Location, float Distance, TArray<AActor*>& Actors) const
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->SearchTinderActorsAt(Location,Distance,Actors);
        }        
    }
}

bool ULuaManger::FindChineseCharactor(const FString& InStr)
{
    FString content;
    //U+4E00..U+9FA5
    for (int k = 1; k <InStr.Len(); k++)
    {
        int c = InStr[k];
        if (c >= 19968 && c <= 40869)
            return true;
    }
    
    return false;
}

void ULuaManger::NotifyReplicateActorFail(APlayerController* PlayerController,AActor* Actor) const
{
    if(OnReplicateActorFail.IsBound())
    {
        OnReplicateActorFail.Broadcast(PlayerController,Actor);
    }
}

TArray<int> ULuaManger::MapSignAlphaTest(UImage* InMapImage, UTexture2D* AdvancedHitTexture, uint8 TestAlpha, TMap<int,FVector2D> InPosMap)
{
#if !UE_SERVER
    TArray<int> AlphaTestResult;
    if (!IsValid(AdvancedHitTexture))
    {
        return AlphaTestResult;
    }

#if WITH_EDITOR
    TArray64<uint8>PicData;
    AdvancedHitTexture->Source.GetMipData(PicData, 0);
    FColor* ImageData = (FColor*)PicData.GetData();
    if (PicData.Num() == 0)
    {
        auto bd = AdvancedHitTexture->GetPlatformMips()[0].BulkData;
        ImageData = static_cast<FColor*>(bd.Lock(LOCK_READ_ONLY));
        bd.Unlock();
    }
#else //WITH_EDITOR
    auto bd = AdvancedHitTexture->GetPlatformMips()[0].BulkData;
    FColor* ImageData = (FColor*)(bd.Lock(LOCK_READ_ONLY));
    bd.Unlock();
#endif //WITH_EDITOR

    const int ImageWidth = AdvancedHitTexture->GetSizeX();
    const int ImageHeight = AdvancedHitTexture->GetSizeY();
    
    for (auto OnePos : InPosMap)
    {
        FVector2D LocalPosition = InMapImage->GetCachedGeometry().AbsoluteToLocal(OnePos.Value);
        LocalPosition.X = floor(LocalPosition.X);
        LocalPosition.Y = floor(LocalPosition.Y);
        LocalPosition /= InMapImage->GetCachedGeometry().GetLocalSize();

        if (LocalPosition.X >= 1.f || LocalPosition.Y >= 1.f || LocalPosition.X < 0.f || LocalPosition.Y < 0.f)
            return AlphaTestResult;

        LocalPosition.X *= ImageWidth;
        LocalPosition.Y *= ImageHeight;
        int BufferPosition = (static_cast<int>(LocalPosition.Y) * ImageWidth) + static_cast<int>(LocalPosition.X);

        if (ImageData && ImageData[BufferPosition].A > TestAlpha) 
        {
            AlphaTestResult.Emplace(OnePos.Key);
        }
    }

    return AlphaTestResult;
#else //!UE_SERVER
    return TArray<int>();
#endif //!UE_SERVER
}

FText ULuaManger::GetStringWithTranslation(FString keyString)
{
    if (ULuaManger::Get()->bNeedTranslation)
    {
        if (ULuaManger::Get()->FindChineseCharactor(keyString))
        {
            return FText::FromString(keyString);
        }
        
        FText TranslationText = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*keyString, TEXT("MultiLanguageTranslation"), *keyString);
        if (UKismetInternationalizationLibrary::GetCurrentLanguage() != "zh" && ULuaManger::Get()->FindChineseCharactor(TranslationText.ToString()))
        {
            const FText ErrorText = FText::FromString("Error");
            return ErrorText;
        }
        FString textStr = TranslationText.ToString();
        if (textStr.StartsWith("cfg.",ESearchCase::CaseSensitive) && textStr.Equals(keyString))
        {
            return FText();
        }
        return TranslationText;
    }

    return FText::FromString(keyString);
}

void ULuaManger::SetTextBlock(UObject* TextObject,FString str)
{
    if (!TextObject)
    {
        UE_LOG(GAME, Error,TEXT("[ULuaManger::SetTextBlock]\tNo TextObject."))
        return;
    }
    UFunction* fun = TextObject->FindFunction("SetText");
    if (!fun)
    {
        UE_LOG(GAME, Error,TEXT("[SetTextBlock]\tNot find SetText function."))
        return;
    }
    FText param = GetStringWithTranslation(str);
    TextObject->ProcessEvent(fun,&param);
}

void ULuaManger::AddTinderActor(AActor* Actor)
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->AddTinderActor(Actor);
        }        
    }
}

void ULuaManger::RemoveTinderActor(AActor* Actor)
{
    if(UReplicationDriver* ReplicationDriver = GetWorld()->NetDriver->GetReplicationDriver())
    {
        if(USimpleReplicationDriver* SimpleReplicationDriver = Cast<USimpleReplicationDriver>(ReplicationDriver))
        {
            SimpleReplicationDriver->RemoveTinderActor(Actor);
        }        
    }
}

double ULuaManger::GetCppTime()
{
    return FPlatformTime::Seconds();
}

FVector ULuaManger::GetFlyLocation(const FVector StartPos,const FVector Speed,const float FlyTime,const float FallTime,
    const float Gravity)
{
    FVector EndPos;
    EndPos.X = StartPos.X + Speed.X * FlyTime;
    EndPos.Y = StartPos.Y + Speed.Y * FlyTime;
    if (FlyTime <= FallTime)
    {
        EndPos.Z = StartPos.Z + Speed.Z * FlyTime;
    } else
    {
        EndPos.Z = StartPos.Z + Speed.Z * FlyTime + 0.5f * Gravity * (FlyTime - FallTime) * (FlyTime - FallTime);
    }
    return EndPos;
}

uint32 ULuaManger::GetObjectUniqueID(UObject* InActor)
{
    if (InActor && IsValid(InActor))
    {
        return InActor->GetUniqueID();
    }

    return 0;
}

bool ULuaManger::DirectoryExists(FString dirPath)
{
    return  IFileManager::Get().DirectoryExists(*dirPath);
}

FString ULuaManger::ProjectContentDir()
{
    return FPaths::ProjectContentDir();
}

void ULuaManger::WaitAndFork()
{
#if PLATFORM_LINUX
    FForkProcess::ForkIfRequested(GetWorld());
#endif  
}

bool ULuaManger::IsForkedChildProcess()
{
    return FForkProcessHelper::IsForkedChildProcess();
}

FString ULuaManger::GetCommandLine()
{
    return FString(FCommandLine::Get());
}

void ULuaManger::SetCommandLine(FString Command)
{
    FCommandLine::Set(*Command);
}

FString ULuaManger::GetMapName()
{
    return GetWorld()->GetMapName();
}

uint32 ULuaManger::GetProcessId()
{
    return FPlatformProcess::GetCurrentProcessId();
}

bool ULuaManger::IsLinuxPlatform()
{
#if PLATFORM_LINUX
    return true;
#endif
    return false;
}

uint16 ULuaManger::GetForkedChildProcessIndex()
{
    return FForkProcessHelper::GetForkedChildProcessIndex();
}

uint32 ULuaManger::GetListenPort()
{
    if(UNetDriver* NetDriver = GetWorld()->GetNetDriver())
    {
        if(UIpNetDriver* IpNetDriver = Cast<UIpNetDriver>(NetDriver))
        {
            if(FSocket* Socket = IpNetDriver->GetSocket())
            {
                return Socket->GetPortNo();
            }
        }
    }
    return 0;
}

void ULuaManger::StartListen(int Port)
{
    FURL DefaultURL;
    DefaultURL.Port = Port;
    GetWorld()->Listen(DefaultURL);
}

bool ULuaManger::IsRegisteredRelevantActor(AActor* Actor)
{
    if(Actor)
    {
        for(UClass* Class : NeedNotifyClasses)
        {
            if(Actor->IsA(Class))
            {
                return true;
            }
        }
    }
    return false;
}

void ULuaManger::LuaReportLogToGameServer_Implementation(const FString& V)
{
    
}

void ULuaManger::SetReportLogLevel(bool bReport, uint8 eLevel)
{
#if WITH_APPLICATION_CORE
    FEvolutionOutputReportGs::Get()->bLogToGameServer = bReport;
    FEvolutionOutputReportGs::Get()->eLogLevel = (ELogVerbosity::Type)eLevel;
#else
    FEvolutionOutputWarn::Get()->bLogToGameServer = bReport;
    FEvolutionOutputWarn::Get()->eLogLevel = (ELogVerbosity::Type)eLevel;
    FEvolutionOutputError::Get()->bLogToGameServer = bReport;
    FEvolutionOutputError::Get()->eLogLevel = (ELogVerbosity::Type)eLevel;
#endif
}

void ULuaManger::ForceStopBehaviorTree(UBehaviorTreeComponent* BehaviorTreeComponent)
{
    if(BehaviorTreeComponent)
    {
        BehaviorTreeComponent->StopTree(EBTStopMode::Forced);
    }
}

void ULuaManger::OpenDisregardForGC()
{
    GUObjectArray.OpenDisregardForGC();
}

void ULuaManger::CloseDisregardForGC()
{
    GUObjectArray.CloseDisregardForGC();
}

FGameplayAbilityTargetDataHandle ULuaManger::MakeAbilityTargetDataFromMsg(
    int32 MsgId, void* Msg, USessionMgr* SesionMgr, AActor* Actor)
{
    TArray<uint8> MsgData;
    if (SesionMgr)
    {
        SesionMgr->Marshal(MsgId, (Table*)Msg, MsgData);
    }
    FGameplayAbilityTargetData_Msg* TargetData = new FGameplayAbilityTargetData_Msg(MsgData);
    if (Actor)
    {
        TargetData->AddActor(Actor);
    }
    return FGameplayAbilityTargetDataHandle(TargetData);
}

Table* ULuaManger::ExtractMsgFromAbilityTargetData(FGameplayAbilityTargetDataHandle Handle, USessionMgr* SesionMgr,
                                                   int32 Index)
{
    const FGameplayAbilityTargetData_Msg* TargetData = static_cast<FGameplayAbilityTargetData_Msg*>(Handle.Get(Index));
    if (TargetData && SesionMgr)
    {
        return SesionMgr->UnMarshal(0, TargetData->Data);
    }
    return nullptr;
}

FHitResult ULuaManger::ExtractMsgHitResultFromAbilityTargetData(FGameplayAbilityTargetDataHandle Handle)
{
    const FGameplayAbilityTargetData_Msg* TargetData = static_cast<FGameplayAbilityTargetData_Msg*>(Handle.Get(0));
    if (TargetData)
    {
       return  *TargetData->GetHitResult();
    }
    return FHitResult();
}

Table* ULuaManger::EffectContextGetTargetDataToMsg(FGameplayEffectContextHandle EffectContext, USessionMgr* SesionMgr)
{
    FGameplayAbilityTargetDataHandle Handle = UCustomAbilitySystemBlueprintLibrary::EffectContextGetTargetData(
        EffectContext);
    return ExtractMsgFromAbilityTargetData(Handle, SesionMgr);
}

void ULuaManger::AddHitResultToTargetDataMsg(FGameplayAbilityTargetDataHandle Handle, FHitResult HitResult,FGameplayEffectContextHandle Context)
{
    FGameplayAbilityTargetData_Msg* TargetData = static_cast<FGameplayAbilityTargetData_Msg*>(Handle.Get(0));
    if (TargetData)
    {
         TargetData->AddHitResult(HitResult);
        if (Context.IsValid())
        {
            TargetData->AddTargetDataToContext(Context,false);
        }
    }
}

void ULuaManger::CloseNetConnection(UNetConnection* Connection)
{
    if(Connection)
    {
        Connection->Close();
    }
}

void ULuaManger::CopyStringToClipboard(FString StringToCopy)
{
    FPlatformApplicationMisc::ClipboardCopy(*StringToCopy);
}

void ULuaManger::ShowMouseCursor(bool bShow)
{
    FSlateApplication::Get().GetPlatformApplication()->Cursor->Show(bShow);
}

static int32 TextFormat(lua_State* L)
{
    int nParamNum = lua_gettop(L);
    if (nParamNum < 2)
    {
        FString logString = FString::Printf(TEXT("Param num is less than 2,num : %d"),nParamNum);
        UE_LOG(GAME, Error,TEXT("[TextFormat]\t%s"),*logString)
        return 0;
    }
    
    FText fmt = ULuaManger::GetStringWithTranslation(UTF8_TO_TCHAR(lua_tostring(L,1)));
    FFormatOrderedArguments args;
    for(int i = 2;i <= nParamNum;i++)
    {
        int luaType = lua_type(L,i);
        if(luaType == LUA_TSTRING)
        {
            args.Emplace(FFormatArgumentValue(ULuaManger::GetStringWithTranslation(UTF8_TO_TCHAR(lua_tostring(L,i)))));
        }
        else if (luaType == LUA_TNUMBER)
        {
            args.Emplace(FFormatArgumentValue(lua_tonumber(L,i)));
        }
        else if(luaType == LUA_TBOOLEAN)
        {
            args.Emplace(FFormatArgumentValue(lua_toboolean(L,i)));
        }
    }

    lua_pushfstring(L,TCHAR_TO_UTF8(*FText::Format(fmt,args).ToString()));
    return 1;
}

static int32 SetTextBlockFormat(lua_State* L)
{
    int nParamNum = lua_gettop(L);
    if (nParamNum < 3)
    {
        FString logString = FString::Printf(TEXT("Param num is less than 3,num : %d"),nParamNum);
        UE_LOG(GAME, Error,TEXT("[SetTextBlockFormat]\t%s"),*logString)
        return 0;
    }
    if (lua_type(L,1) != LUA_TUSERDATA)
    {
        UE_LOG(GAME, Error,TEXT("[SetTextBlockFormat]\tFirst param is not userdata."))
        return 0;
    }
    
    FText fmt = ULuaManger::GetStringWithTranslation(UTF8_TO_TCHAR(lua_tostring(L,2)));
    FFormatOrderedArguments args;
    for(int i = 3;i <= nParamNum;i++)
    {
        int luaType = lua_type(L,i);
        if(luaType == LUA_TSTRING)
        {
            args.Emplace(FFormatArgumentValue(ULuaManger::GetStringWithTranslation(UTF8_TO_TCHAR(lua_tostring(L,i)))));
        }
        else if (luaType == LUA_TNUMBER)
        {
            args.Emplace(FFormatArgumentValue(lua_tonumber(L,i)));
        }
        else if(luaType == LUA_TBOOLEAN)
        {
            args.Emplace(FFormatArgumentValue(lua_toboolean(L,i)));
        }
    }
    
    UObject* TextObject = UnLua::GetUObject(L,1);
    UFunction* fun = TextObject->GetClass()->FindFunctionByName("SetText");
    if (!fun)
    {
        UE_LOG(GAME,Error,TEXT("[SetTextBlockFormat]\tNotFindText."))
        return 0;
    }
    FText funArg = FText::Format(fmt,args);
    TextObject->ProcessEvent(fun,&funArg);
    
    return 0;
}

static int32 ULuaManger_UnPack(lua_State *L)
{
    if(lua_type(L,1) != LUA_TTABLE)
    {
        return 0;
    }
    Table* pTable = (Table*)lua_topointer(L,1);
    if(pTable->alimit == 0)
        return 0;
    unsigned int nSize = 0;
    for(unsigned int i = pTable->alimit ; i > 0; i--)
    {
        if(!ttisnil(pTable->array + i - 1))
        {
            nSize = i;
            break;
        }
    }
    for(unsigned int i = 0; i < nSize; i++)
    {
        CLuaMessageTableOptInC::PushValue(pTable->array + i, L);
    }
    return  nSize;
}

int32 ULuaManger_Report(lua_State *L)
{
#if UE_SERVER
    return 0;
#endif
    int nParamNum = lua_gettop(L);
    if (nParamNum < 2)
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_Report Param num is less than 2,num : %d"),nParamNum);
        return 1;
    }
    if (lua_type(L,1) != LUA_TSTRING)
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_Report First param is not string."))
        return 2;
    }
    FString strEvent(UTF8_TO_TCHAR(lua_tostring(L,1)));
    if(lua_type(L,2) == LUA_TSTRING)
    {
        FString strConent(UTF8_TO_TCHAR(lua_tostring(L,2)));
        FSdkInterface::GetIns()->Report(strEvent, strConent);
    }
    else if(lua_type(L,2) == LUA_TTABLE)
    {
        FString strJson;
        if(!FLuaTableToJson::ConvertLuaTableToJson((Table*)lua_topointer(L,2), strJson))
        {
            CLuaMessageTableOptInC::OnlyPrintLuaTrackback("", L);
            return 0;
        }
        FSdkInterface::GetIns()->Report(strEvent, strJson);
    }
    return 0;
}

int ULuaManger_ReleaseDelegate(lua_State *L)
{
    if(lua_gettop(L) < 2 )
    {
        UE_LOG(GAME, Error,TEXT("ReleaseDelegate  param 2 type error."))
        return 0;
    }
    UObject *Object = UnLua::GetUObject(L, 1);
    if (!Object)
    {
        UNLUA_LOGERROR(L, GAME, Error, TEXT("%s: Invalid object!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TFUNCTION)
    {
        UNLUA_LOGERROR(L, GAME, Error, TEXT("%s: Invalid function!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }
    auto LuaFunction = lua_topointer(L, 2);
    UnLua::ReleaseLuaCallback(Object, LuaFunction);
    return  0;
}

#include "JsonToLuaTable.h"
int ULuaManger_JsonToLuaTable(lua_State *L)
{
    if(lua_gettop(L) < 1)
    {
        UE_LOG(GAME, Error,TEXT("JsonToLuaTable no param."));
        return 0;
    }
    if(lua_type(L,1) != LUA_TSTRING)
    {
        UE_LOG(GAME, Error,TEXT("JsonToLuaTable  param not string."));
        return 0;
    }
    //UE_LOG(LogTemp,Display,TEXT("before ConvertJsonToLuaTable %d,%d, %p"), L->top - L->stack, L->ci->top - L->top,L->stack);
	
    if(FJsonToLuaTable::ConvertJsonToLuaTable(lua_tostring(L,1), L))
    {        
        return  1;
    }
    return 0;
}

int ULuaManger_LuaTableToJson(lua_State *L)
{
    if(lua_gettop(L) < 1)
    {
        UE_LOG(GAME, Error,TEXT("LuaTableToJson no param."));
        return 0;
    }
    if(lua_type(L,1) != LUA_TTABLE)
    {
        UE_LOG(GAME, Error,TEXT("LuaTableToJson  param not table."));
        return 0;
    }
    std::string strJson;
    if(!FLuaTableToJson::ConvertLuaTableToJson((Table*)lua_topointer(L,1), strJson))
    {
        CLuaMessageTableOptInC::OnlyPrintLuaTrackback("", L);
        return 0;
    }
    lua_pushstring(L,strJson.c_str());
    return 1;
} 

#include "PrintMemory.inl"
static const luaL_Reg ULuaMangerLib[] =
{
    {"UnPack", ULuaManger_UnPack},
    {"SetTextBlockFormat", SetTextBlockFormat},
    {"ReportTable", ULuaManger_Report},
    {"PrintLuaMem", ULuaManger_LuaGcMem},
    {"PrintLiveObjects", ULuaManger_PrintLiveObjects},
    {"CheckLuaNeedFullGc", ULuaManger_LuaFullGc},
    {"PrintObjectByClassName",ULuaManger_PrintObjectByClassName},
#if ENABLE_MEMORYTRACKER
    {"SetMemoryTracker",ULuaManger_SetMemoryTracker},
    {"AddMemoryTrackerTag",ULuaManger_AddMemoryTrackerTag},
    {"RemoveMemoryTrackerTag",ULuaManger_RemoveMemoryTrackerTag},
    {"SaveMemoryTracker",ULuaManger_SaveMemoryTracker},
    {"ClearMemoryTracker",ULuaManger_ClearMemoryTracker},
#endif    
    {"ReleaseDelegate",ULuaManger_ReleaseDelegate },
    {"JsonToLuaTable",ULuaManger_JsonToLuaTable},
    {"LuaTableToJson",ULuaManger_LuaTableToJson},
    {"TextFormat",TextFormat},
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(ULuaManger)
	ADD_LIB(ULuaMangerLib)
    ADD_FUNCTION(MakeAbilityTargetDataFromMsg)
    ADD_FUNCTION(ExtractMsgFromAbilityTargetData)
    ADD_FUNCTION(EffectContextGetTargetDataToMsg)
    ADD_FUNCTION(ExtractMsgHitResultFromAbilityTargetData)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(ULuaManger)
