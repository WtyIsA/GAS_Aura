---@class Utils @辅助类
---@field PlayerBPName string
---@field RiderBPName string
---@field AIBaseName string;
---@field StrongHoldName string;
---@field LogLevel number;
---@field FZeroRotator FRotator
---@field HitResult FHitResult
Utils = {}
local CPP = Global.g_luaMgr

Utils.FVectorUp = UE4.FVector(0, 0, 1);
Utils.FZeroRotator = UE4.FRotator(0, 0, 0);
Utils.FVectorOne = UE4.FVector(1, 1, 1);
Utils.FVectorZero = UE4.FVector(0, 0, 0);

Utils.HitResult = UE4.FHitResult();
Utils.LogLevel = 0;
Utils.RandomNum = 0;
Utils.RedPointCfgId =2500;
Utils.Scope4CfgId =2501;
Utils.Scope8CfgId =2502;

local TimeZoneOff = 0
local DayOfSeconds = 1 * 24 * 60 * 60
local DayOfMillSeconds = 1 * 24 * 60 * 60 * 1000
local WeekOfSeconds = 7 * 24 * 60 * 60
local GameDayChangeSeconds = 5 * 60 * 60
local HourOfMillSeconds = 60 * 60
local IncrementNum = 0;
local SyncBsTime = 0
local BsStartTime = 0
local CppStartTime = 0
local LuaStartTime = 0

local mt = {__mode="kv"}
---@private
Utils.ObjContextTable = {}
setmetatable(Utils.ObjContextTable,mt)

function Utils.Init()
    TimeZoneOff = math.ceil(Utils.GetTimeZone())
    Utils.ZoneDiff = TimeZoneOff // 3600
    Utils.InitShareMgr()
    LuaStartTime = Utils.GetTime()
    CppStartTime = Global.g_luaMgr:GetCppTime()
    if Global.bServer or BattleUtils.Test then
        Utils.SetBPName();
        Utils.InitAllGE()
    else
        CPP.OnWebViewUrlClosed:Bind(CPP, Utils.OnWebViewUrlClosed);
        CPP.OnWebViewOpenUrlError:Bind(CPP, Utils.OnWebViewOpenUrlError);
    end
end

function Utils.SetBPName()
    Utils.RandomNum = math.random(100, 100000)
    Utils.PlayerBPName = UE4.UClass.Load("/Game/Character/Blueprints/BP_Character.BP_Character_C")
    Utils.AIBaseName = UE4.UClass.Load("/Game/AI/AI_Base.AI_Base_C")
    Utils.DoorBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_DoorBase.BP_DoorBase_C")
    Utils.TorchBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_TorchBase.BP_TorchBase_C")
    Utils.GroundItemBPName = UE4.UClass.Load("/Game/Blueprints/GroundItem/BP_GroundItem.BP_GroundItem_C")
    Utils.TreasureBoxBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_TreasureBase.BP_TreasureBase_C")
    Utils.AIBaseName = UE4.UClass.Load("/Game/AI/AI_Base.AI_Base_C")
    Utils.WarpGateBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_WarpGateBase.BP_WarpGateBase_C")
    Utils.AltarBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_altarBase.BP_altarBase_C")
    Utils.DoorOrganBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_OrganBase.BP_OrganBase_C")
    Utils.OliveTrapBPName = UE4.UClass.Load("/Game/Blueprints/Placement/BP_OliveTrap.BP_OliveTrap_C")
    Utils.ReviveAltarBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_ReviveAltarBase.BP_ReviveAltarBase_C")
    Utils.OreBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_OreBase.BP_OreBase_C")
    Utils.TreasurePileBPName = UE4.UClass.Load("/Game/Environmental/Blueprint/Interactive/BP_TreasurepileBase.BP_TreasurepileBase_C")

    --Utils.RiderBPName = UE4.UClass.Load("/Game/HorseAnimsetPro/BluePrints/BP_Rider.BP_Rider_C")
    --Utils.AIControllerName = UE4.UClass.Load("/Game/AI/AIController_Base.AIController_Base_C")
    --Utils.BuildBPName = UE4.UClass.Load("/Game/BuildingSystem/Blueprints/BuildingObjects/Base/BP_EBS_BuildingObject.BP_EBS_BuildingObject_C")
    --Utils.TrashName = UE4.UClass.Load("/Game/Blueprints/Trash/BP_Trash.BP_Trash_C")
    --Utils.GroundItemName = UE4.UClass.Load("/Game/Blueprints/GroundItem/BP_GroundItem.BP_GroundItem_C")
    --Utils.ChestBaseName = UE4.UClass.Load("/Game/Blueprints/HourChest/BP_ChestBase.BP_ChestBase_C");
    --Utils.ClueBaseName = UE4.UClass.Load("/Game/Blueprints/HourChest/BP_ClueBase.BP_ClueBase_C")
    --Utils.SwitchBaseName = UE4.UClass.Load("/Game/Blueprints/HourChest/BP_SwitchBase.BP_SwitchBase_C")
    --Utils.ItemBoxName = UE4.UClass.Load("/Game/Blueprints/GroundItem/BP_ItemBox.BP_ItemBox_C")
    --Utils.BuildingDoor = UE4.UClass.Load("/Game/Blueprints/SystemBuildingPart/BuildingDoor.BuildingDoor_C");
    --Utils.PlantName = UE4.UClass.Load("/Game/Blueprints/Plant/BP_Plant.BP_Plant_C")
    --Utils.BaseGunPartName = UE4.UClass.Load("/Game/GunSystem/Blueprints/Part/BP_BaseGunPart.BP_BaseGunPart_C")
    --Utils.BaseScopeName = UE4.UClass.Load("/Game/GunSystem/Blueprints/Part/BP_BaseScope.BP_BaseScope_C")
    --Utils.BasePropName = UE4.UClass.Load("/Game/GunSystem/Blueprints/Part/BP_Prop.BP_Prop_C")
    --Utils.CardDoorName = UE4.UClass.Load("/Game/Blueprints/SystemBuildingPart/CardDoor.CardDoor_C")
    --Utils.VehicleBaseName = UE4.UClass.Load("/Game/Vehicle/BP_VehicleParent.BP_VehicleParent")
end

function Utils.InitShareMgr()
    require("GameState")
    require("Event.EventUtils")
    require("Building.BuildUtils")
    if BattleUtils == nil then
        require("BattleUtils")
        local battleUtilsClass = UE4.UClass.Load("/Game/Characters/Common/Blueprints/BP_BattleUtils");
        BattleUtils = NewObject(battleUtilsClass, Global.g_gameInstance, nil, "BattleUtils")
        Global.g_luaMgr:AddObjToRoot(BattleUtils);
        BattleUtils.Init();
    else
        BattleUtils.Init();
    end
    
end

--委托类型封装
function CppCallback(selfObject, func)
    return { Global.g_luaMgr, function(returnObject, ...)
        func(selfObject, ...)
    end }
end

---@param WorldContext UObject
---@param startPos FVector
---@param endPos FVector
---@param ignoreActor TArray;
function Utils.CanSee(worldContext, startPos, endPos, ignoreActor)
    local hitInfo = UE4.FHitResult();
    UE4.UKismetSystemLibrary.LineTraceSingle(worldContext, startPos, endPos, UE4.ETraceTypeQuery.Bullet, false, ignoreActor, UE4.EDrawDebugTrace.None, hitInfo, true, nil, UE4.FLinearColor(255, 1, 1, 1), 1);
    if hitInfo.bBlockingHit then
        return false,hitInfo;
    end
    return true,hitInfo;
end

---@param OriginPos FVector
---@param FrontDir FVector
---@param BoxExtent FVector
---@param OutActors TArray<AActor> @[out]
---@param ActorsToIgnore TArray<AActor>
---@param ObjectTypes TArray<number @byte>
---@param ActorClassFilter TSubclassOf<UObject>
---@return boolean
function Utils.GetActorsInFrontRange(OriginPos, FrontDir, BoxExtent, OutActors, ActorsToIgnore, ObjectTypes, ActorClassFilter)
    FrontDir = UE4.UKismetMathLibrary.Normal(FrontDir, 0.01);
    local extentPos = UE4.FVector(BoxExtent.X, BoxExtent.Y, 0) * FrontDir;
    OriginPos = OriginPos + extentPos;
    if not UE4.UKismetSystemLibrary.BoxOverlapActors(Global.g_gameInstance:GetWorld(), OriginPos, BoxExtent, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors) then
        return false;
    end
    return true;
end

function Utils.GetIncrementNum()
    IncrementNum = IncrementNum + 1;
    return IncrementNum;
end

---@param owner AActor
---@param bpPath string
---@param classPath string
---@return AActor
function Utils.SpawnActorByPath(owner, bpPath, classPath)
    local World = Global.g_gameInstance:GetWorld();
    local bp = UE4.UClass.Load(bpPath);
    local pos = UE4.FTransform();
    if owner then
        pos = owner:GetTransform()
    end
    local actor = World:SpawnActor(bp, pos, UE4.ESpawnActorCollisionHandlingMethod.AlwaysSpawn, owner, owner, classPath);
    return actor;
end

---@private
---@return number @时差
function Utils.GetTimeZone()
    local now = Utils.GetTime()
    return math.ceil(os.difftime(now, os.time(os.date("!*t", now))))
end

---@return number @时间戳秒
function Utils.GetTime()
    if Global.bServer == false and HallMgr then
        return HallMgr:GetTime()
    end
    return math.ceil(os.time())
end

---@return number @时间戳秒f
function Utils.GetIntTime()
    return math.ceil(os.time())
end

---@return number@ 虚幻时间戳秒
function Utils.GetPlatformTime()
    return Global.g_luaMgr:GetCppTime()
end

---@return number @时间戳毫秒
function Utils.GetTimeInMill()
    return math.ceil((LuaStartTime + Global.g_luaMgr:GetCppTime() - CppStartTime) * 1000)
end

---@return number @带时差的时间戳
function Utils.GetTimeWithTimeZone()
    return math.ceil(Utils.GetTime() + TimeZoneOff)
end

function Utils.GetTimeZoneOff()
    return TimeZoneOff
end

function Utils.GetTimeOfDay()
    return Utils.GetTimeWithTimeZone() % Utils.GetDayOfSeconds()
end

---@return number @一天的秒数
function Utils.GetDayOfSeconds()
    return DayOfSeconds
end

---@return number @一天的毫秒数
function Utils.GetDayOfMillSeconds()
    return DayOfMillSeconds
end

---@return number @一天的剩余秒数
function Utils.GetRestOfDaySeconds()
    local time = Utils.GetTimeWithTimeZone()
    time = time % DayOfSeconds
    return DayOfSeconds - time
end

---@return number @一天的剩余毫秒数
function Utils.GetRestOfDayMillSeconds()
    return Utils.GetRestOfDaySeconds() * 1000
end

---@param fullString string @需要分割的字符串
---@param separator string @分隔符
---@param plain boolean [opt] 是否关闭模式匹配，如果 separator是 "."，需要设置为true
---@return ArrayList
function Utils.Split(fullString, separator, plain)
    local findStartIndex = 1
    local splitArray = ArrayList:new()
    local sepLen = string.len(separator);

    while true do
        local findLastIndex = string.find(fullString, separator, findStartIndex, plain)
        if not findLastIndex then
            splitArray:add(string.sub(fullString, findStartIndex, string.len(fullString)))
            break
        end
        splitArray:add(string.sub(fullString, findStartIndex, findLastIndex - 1))
        findStartIndex = findLastIndex + sepLen;
    end
    return splitArray
end

---@param fullString string @需要分割的字符串
---@param separator string @分隔符
function Utils.SplitAndGetLast(fullString, separator)
    local index = string.find(fullString, separator)
    if index == nil then
        return fullString
    end
    return string.sub(fullString, -string.find(string.reverse(fullString), separator) + 1, -1)
end

---@param num number @需要分割的数字
function Utils.SplitNumbers(num)
    local arr = ArrayList:new()
    if num == 0 then
        arr:add(0)
        return arr
    end
    while (num > 0) do
        arr:add(num % 10)
        num = math.floor(num / 10)
    end
    return arr
end

---@param str string
function Utils.Trim(str)
    return string.match(str, "^[%s]*(.-)[%s]*$")
end

function Utils.CheckChinese(s)
    --U+4E00..U+9FA5
    for k = 1, #s do
        local c = string.byte(s,k)
        if not c then break end
        if c>=228 and c<=233 then
            local c1 = string.byte(s,k+1)
            local c2 = string.byte(s,k+2)
            if c1 and c2 then
                local a1,a2,a3,a4 = 128,191,128,191
                if c == 228 then a1 = 184
                elseif c == 233 then a2,a4 = 190,165
                end
                if c1>=a1 and c1<=a2 and c2>=a3 and c2<=a4 then
                    return true
                end
            end
        end
    end
    return false
end

---@param txtCtrl UTextBlock
---@param str string
function Utils.SetTextBlockText(txtCtrl, str)
    if txtCtrl:IsA(UE4.UEditableText) or txtCtrl:IsA(UE4.UEditableTextBox) then
        txtCtrl:SetText(str)
        return;
    end    
    if txtCtrl then
        CPP:SetTextBlock(txtCtrl, str)
    else
        Utils.LogError("Utils.SetTextBlockText", "Text is nil\t" .. debug.traceback())
    end
end

---@param originStr string
local function ReplaceSWithIndex(originStr)
    local formatIndex = 0
    local startIndex, endIndex = 0, 0
    while string.find(originStr, "%%s") or string.find(originStr, "%.", 0, true) or string.find(originStr, "%%%%") do
        startIndex, endIndex = string.find(originStr, "%", startIndex + 1, true);
        if startIndex == nil or -1 == startIndex or startIndex >= string.len(originStr) then
            break
        end

        local afterFindChar = string.sub(originStr, startIndex + 1, startIndex + 1)
        if afterFindChar == 's' or afterFindChar == 'd' then
            originStr = string.sub(originStr, 1, startIndex - 1) .. "{" .. formatIndex .. "}" .. string.sub(originStr, startIndex + 2, string.len(originStr))
        elseif afterFindChar == '.' then
            afterFindChar = string.sub(originStr, startIndex + 3, startIndex + 3)
            if afterFindChar == 'f' then
                originStr = string.sub(originStr, 1, startIndex - 1) .. "{" .. formatIndex .. "}|digit(" .. string.sub(originStr, startIndex + 2, startIndex + 2) .. ")" .. string.sub(originStr, startIndex + 4, string.len(originStr))
            end
        elseif afterFindChar == "%" then
            originStr = string.sub(originStr, 1, startIndex - 1) .. string.sub(originStr, startIndex + 1, string.len(originStr))
            formatIndex = formatIndex - 1
        end

        formatIndex = formatIndex + 1;

        if formatIndex > 100 then
            Utils.LogWarning("Utils", string.format("insertIndex : %d, str : %s", formatIndex, originStr), 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)
            break
        end
    end

    return originStr
end

---@param txtCtrl UTextBlock
---@param fmt string
function Utils.SetTextBlockTextFormat(txtCtrl, fmt, param1, ...)
    if not txtCtrl then
        Utils.LogError("Utils.SetTextBlockTextFormat", "Text is nil\n" .. debug.traceback())
        return
    end
    if fmt == nil then
        CPP:SetTextBlock(txtCtrl,"")
        Utils.LogError("Utils.SetTextBlockTextFormat", "Fmt is nil\n" .. debug.traceback())
        return
    end
    if param1 == nil then
        CPP:SetTextBlock(txtCtrl, fmt)
    else
        CPP.SetTextBlockFormat(txtCtrl, fmt, param1, ...)
    end
end

function Utils.SetTextBlockColorByColorId(txtCtrl, colorId)
    local cfg = ColorCfg:Get(colorId)
    if not cfg then
        return 
    end
    local R,G,B,A = Utils.GetRGBANumByColorString(cfg.color)
    Utils.SetTextBlockColor(txtCtrl, R, G, B, A)
end

---@param txtCtrl UTextBlock
---@param R number [0,1]
---@param G number [0,1]
---@param B number [0,1]
---@param A number [0,1]
function Utils.SetTextBlockColor(txtCtrl, R, G, B, A)
    if (txtCtrl == nil) then
        return ;
    end
    if A == nil then
        A = 1
    end
    ---@type FSlateColor
    local slateColor = txtCtrl.ColorAndOpacity
    slateColor.SpecifiedColor = UKismetMathLibrary.MakeColor(R, G, B, A)
    txtCtrl:SetColorAndOpacity(slateColor)
end

---@param txtCtrl UTextBlock
---@param color FLinearColor
function Utils.SetTextBlockColorByFColor(txtCtrl, color)
    if (txtCtrl == nil) then
        return ;
    end
    ---@type FSlateColor
    local slateColor = txtCtrl.ColorAndOpacity
    slateColor.SpecifiedColor = color
    txtCtrl:SetColorAndOpacity(slateColor)
end

---@param txtCtrl UTextBlock
---@param R number [0,1]
---@param G number [0,1]
---@param B number [0,1]
---@param A number [0,1]
function Utils.SetTextBlockShaderColor(txtCtrl, R, G, B, A)
    if (txtCtrl == nil) then
        return ;
    end
    if A == nil then
        A = 1
    end
    local slateColor = txtCtrl.ShadowColorAndOpacity
    slateColor = UKismetMathLibrary.MakeColor(R, G, B, A)
    txtCtrl:SetShadowColorAndOpacity(slateColor)
end

---@param txtCtrl UTextBlock
---@param size number
function Utils.SetTextBlockFontSize(txtCtrl, size)
    if (txtCtrl == nil) then
        return ;
    end
    local font = txtCtrl.Font;
    font.Size = size;
    txtCtrl:SetFont(font);
end

---@param txtCtrl UTextBlock
---@param Typeface string
function Utils.SetTextBlockFontTypeface(txtCtrl, Typeface)
    if (txtCtrl == nil) then
        return ;
    end
    local font = txtCtrl.Font
    font.TypefaceFontName = Typeface
    txtCtrl:SetFont(font)
end

function Utils.GetRichColorText(color, str)
    return "<span color=\"" .. color .. "\">" .. str .. "</>"
end

function Utils.FindLastStrPos(str, key)
    local i, j
    local k = 0
    repeat
        i = j
        j, k = string.find(str, key, k + 1, true)
    until j == nil
    return i
end

---@alias 动态设置UImage的图片
---@param image UImage
---@param path string 图片的路径, sprite相对路径
function Utils.SetUImageSpriteTexture(image, path, isMatchSize)
    ClientResMgr:SetUImageSpriteTexture(image, path, isMatchSize);
end

---@alias 动态设置UImage的图片
---@param image UImage
---@param path string 图片的路径, sprite相对路径
function Utils.SetUImageUITexture(image, path)
    ClientResMgr:SetUImageTexture(image, path);
end

---@param image UImage
function Utils.SetMultiUImageUITexture(image, id)
    local cfg = MultiTextureCfg:Get(id)
    if (cfg) then
        if (PlatformMgr.IsOverSea()) then
            Utils.SetUImageUITexture(image, cfg.oversea)
        else
            Utils.SetUImageUITexture(image, cfg.domestic)
        end
    end
end

function Utils.SetUImageQuality(image, quality, index)
    index = index or "icon"
    Utils.SetUImageSpriteTexture(image, ItemQualityCfg:Get(quality)[index])
end

function Utils.GetRGBNumByColorString(color)
    color = color:gsub("#", "")
    local R = tonumber(string.sub(color, 1, 2), 16)
    local G = tonumber(string.sub(color, 3, 4), 16)
    local B = tonumber(string.sub(color, 5, 6), 16)
    return R / 255, G / 255, B / 255
end

function Utils.GetRGBANumByColorString(color)
    color = color:gsub("#", "")
    local R = tonumber(string.sub(color, 1, 2), 16)
    local G = tonumber(string.sub(color, 3, 4), 16)
    local B = tonumber(string.sub(color, 5, 6), 16)
    local A = tonumber(string.sub(color, 7, 8), 16)
    if (A == nil) then
        A = 255;
    end
    return R / 255, G / 255, B / 255, A / 255
end

---检查是否有特殊字符
---@param str string
function Utils.CheckSpecialChar(str)
    for i = 1, #NamePatternCfg do
        local content = NamePatternCfg:Get(i).content;
        if string.find(str, content) then
            return true;
        end
    end
    return false;
end

---特殊字符 替换
---@param str string
---@param repl string  替换的字符串
function Utils.ReplaceSpecialChar(str, repl)
    for i = 1, #NamePatternCfg do
        local content = NamePatternCfg:Get(i).content;
        str = (string.gsub(str, content, repl))
    end
    return str;
end

---@param times number ms
function Utils.FormatTime(times)
    times = times // 1000;
    local h = times // 3600;
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    if (h > 0) then
        return string.format("%02d:%02d:%02d", h, m, s) --""..h..":"..m..":"..s
    else
        return string.format("%02d:%02d", m, s) --""..m..":"..s
    end
end

---@param time string
function Utils.GetTimeStamp(time)
    local list = ArrayList:new()
    for timeNum in string.gmatch(time, "%d+") do
        list:add(timeNum)
    end
    return os.time({
        year = list:get(1), month = list:get(2), day = list:get(3), hour = list:get(4), min = list:get(5), sec = list:get(6)
    })
end

---@param times number second
function Utils.FormatSecond(time)
    local stringIndex = time / 86400 > 1 and 31 or time / 3600 > 1 and 32 or time / 60 > 1 and 33 or 34
    local timeNum = time / 86400 > 1 and math.floor(time / 86400) or time / 3600 > 1 and math.floor(time / 3600) or time / 60 > 1 and math.floor(time / 60) or time
    return Utils.TextFormat(Utils.GetMultLanguageString(Utils.GetString(stringIndex)), math.floor(timeNum))
end

function Utils.FormatNum(num)
    local t1, t2 = math.modf(num)
    if t2 > 0 then
        return num
    else
        return t1
    end
end

function Utils.FormatNumByComma(numTmp)
    local resultNum = numTmp
    if type(numTmp) == "number" then
        local inter, point = math.modf(numTmp)

        local strNum = tostring(inter)
        local newStr = ""
        local numLen = string.len(strNum)
        local count = 0
        for i = numLen, 1, -1 do
            if count % 3 == 0 and count ~= 0  then
                newStr = string.format("%s,%s", string.sub( strNum,i,i), newStr)
            else
                newStr = string.format("%s%s", string.sub( strNum,i,i), newStr)
            end
            count = count + 1
        end

        if point > 0 then
            local strPoint = string.format( "%.2f", point )
            resultNum = string.format("%s%s",newStr, string.sub( strPoint,2, string.len( strPoint )))
        else
            resultNum = newStr
        end
    end

    return resultNum
end

---@return string @服务器调用
function Utils.SrvGetString(id)
    local strCfg = Strings:Get(id)
    if (strCfg ~= nil) then
        return strCfg.content
    end
    return ""
end

---@return string
function Utils.GetString(id)
    return Strings:Get(id) and Strings:Get(id).content or "字符串配置错误Id:" .. id
end

function Utils.TextFormat(...)
    return CPP.TextFormat(...)
end

function Utils.GetMultLanguageString(str)
    return ULuaManger.GetStringWithTranslation(str)
end

function Utils.GetMultLanguageString_check(str)
    return Utils.GetMultLanguageString(str)
end

function Utils.print_t(t)
    local print_r_cache = {}
    local function sub_print_r(t, indent)
        if (print_r_cache[tostring(t)]) then
            print(indent .. "*" .. tostring(t))
        else
            print_r_cache[tostring(t)] = true
            if (type(t) == "table") then
                for pos, val in pairs(t) do
                    if (type(val) == "table") then
                        print(indent .. "[" .. pos .. "] => " .. tostring(t) .. " {")
                        sub_print_r(val, indent .. string.rep(" ", string.len(pos) + 8))
                        print(indent .. string.rep(" ", string.len(pos) + 6) .. "}")
                    elseif (type(val) == "string") then
                        print(indent .. "[" .. pos .. '] => "' .. val .. '"')
                    else
                        print(indent .. "[" .. pos .. "] => " .. tostring(val))
                    end
                end
            else
                print(indent .. tostring(t))
            end
        end
    end
    if (type(t) == "table") then
        print(tostring(t) .. " {")
        sub_print_r(t, "  ")
        print("}")
    else
        sub_print_r(t, "  ")
    end
    print()
end

---@param times number s format xx小时xx分钟
function Utils.FormatTimeOfHHMM(times)
    local h = times // 3600;
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    if h > 0 then
        return "" .. h .. Utils.GetMultLanguageString(Utils.GetString(114)) .. m .. Utils.GetMultLanguageString(Utils.GetString(115))
    elseif m > 0 then
        return "" .. m .. Utils.GetMultLanguageString(Utils.GetString(115)) .. s .. Utils.GetMultLanguageString(Utils.GetString(116))
    else
        return "" .. s .. Utils.GetMultLanguageString(Utils.GetString(116))
    end
end

---@param times number s format xx天xx小时xx分钟
function Utils.FormatTimeOfDDHHMMSS(times)
    local d = times // (3600 * 24)
    times = times % (3600 * 24);
    local h = times // 3600;
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    if d > 0 then
        return "" .. d .. Utils.GetMultLanguageString(Utils.GetString(75)) .. h .. Utils.GetMultLanguageString(Utils.GetString(76)) .. m .. Utils.GetMultLanguageString(Utils.GetString(77))
    elseif h > 0 then
        return "" .. h .. Utils.GetMultLanguageString(Utils.GetString(76)) .. m .. Utils.GetMultLanguageString(Utils.GetString(77))
    elseif m > 0 then
        return "" .. m .. Utils.GetMultLanguageString(Utils.GetString(77)) .. s .. Utils.GetMultLanguageString(Utils.GetString(78))
    else
        return "" .. s .. Utils.GetMultLanguageString(Utils.GetString(78))
    end
end

---@param times number s format hh:mm
function Utils.FormatTimeOfHHMM2(times)
    local h = times // 3600 % 24;
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    return string.format("%02d:%02d", h, m)
end

---@param times number s format hh:mm:ss
function Utils.FormatTimeOfHHMMSS(times)
    local h = times // 3600;
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    return string.format("%02d:%02d:%02d", h, m, s)
end

---@param times number s format mm:ss
function Utils.FormatTimeOfMMSS(times)
    times = times % 3600;
    local m = times // 60;
    local s = times % 60;
    return string.format("%02d:%02d", m, s)
end

function Utils.AddOperateBtn(hitActor)
    if (not hitActor) then
        return
    end
    if not BattleUtils.GetSelfPlayer() then
        return
    end

    EventUtils.TriggerEvent(EventUtils.AddOperateBtn, hitActor)
end

function Utils.RefreshOperateBtn(hitActor)
    if (not hitActor) then
        return ;
    end
    if not BattleUtils.GetSelfPlayer() then
        return
    end
    if(not BattleUtils.GetSelfPlayer().lastOutHits:contains(hitActor))then
        return;
    end
    EventUtils.TriggerEvent(EventUtils.AddOperateBtn, hitActor)
end

function Utils.RemoveOperateBtn(hitActor)
    if (not hitActor) then
        return ;
    end
    if not BattleUtils.GetSelfPlayer() then
        return
    end

    EventUtils.TriggerEvent(EventUtils.RemoveOperateBtn, hitActor)
end

function Utils.HideMenuBtn()
    local pBattlePanel = UIWidgetMgr.GetPanel("WBP_Battle_m")
    if (pBattlePanel) then
        pBattlePanel:HideBtnMenus();
    end
end

function Utils.ShowMenuBtn()
    local pBattlePanel = UIWidgetMgr.GetPanel("WBP_Battle_m")
    if pBattlePanel then
        pBattlePanel:ShowBtnMenus();
    end
end

function Utils.RefreshPlantTime(time, seedId, slateVisibility)
    if (PlatformMgr.nPlatformId == PlatformMgr.PC) then
        if BattlePanel then
            BattlePanel:SetPlantTime(time, seedId, slateVisibility);
        end
    else
        local pBattlePanel = UIWidgetMgr.GetPanel("WBP_Battle_m")
        if pBattlePanel then
            pBattlePanel:SetPlantTime(time, seedId, slateVisibility);
        end
    end
end

---@param time1 number
---@param time2 number
---@return boolean
function Utils.IsSameDay(time1, time2)
    return Utils.GetTotalDays(time1) == Utils.GetTotalDays(time2)
end

---@private
---@param time number
---@return number
function Utils.GetTotalDays(time)
    return math.floor((time + TimeZoneOff) / DayOfSeconds)
end

function Utils.FormatUnixTime2Date(unixTime)
    if unixTime and unixTime >= 0 then
        local tb = {}
        tb.year = tonumber(os.date("%Y", unixTime))
        tb.month = tonumber(os.date("%m", unixTime))
        tb.day = tonumber(os.date("%d", unixTime))
        tb.hour = tonumber(os.date("%H", unixTime))
        tb.minute = tonumber(os.date("%M", unixTime))
        tb.second = tonumber(os.date("%S", unixTime))
        return tb
    end
end

---@alias DelayCallback fun(self:table,...)
---@param t table
---@param fDelayTime number
---@param func DelayCallback
function Utils.Delay(t, fDelayTime, func, ...)
    local args = { ... }
    Global.g_Ticker:RegisterTick(t, function(selfObj, _, _)
        func(selfObj, CPP.UnPack(args))
        return 0
    end, fDelayTime * 1000)
end

---Set a timer to execute delegate. Setting an existing timer will reset that timer with updated parameters.
---@param Delegate Delegate
---@param Time number
---@param bLooping boolean
---@param InitialStartDelay number @[opt] 
---@param InitialStartDelayVariance number @[opt] 
---@return FTimerHandle
function Utils.SetTimer(Delegate, Time, bLooping, InitialStartDelay, InitialStartDelayVariance)
    return UE4.UKismetSystemLibrary.K2_SetTimerDelegate(Delegate, Time, bLooping, InitialStartDelay, InitialStartDelayVariance);
end

---Clears a set timer.
---@param WorldContextObject UObject
---@param Handle FTimerHandle
function Utils.ClearTimer(WorldContextObject, handle)
    UE4.UKismetSystemLibrary.K2_ClearTimerHandle(WorldContextObject, handle);
end

---@param sex boolean
function Utils.GetSexType(sex)
    if sex then
        return SexType.Man
    else
        return SexType.Woman
    end
end

---@param dropItem FixDrop
function Utils.GetDropItemCfg(dropItem)
    return Utils.GetDropItemCfgByType(dropItem.dropType, dropItem.id)
end

function Utils.GetDropItemCfgByType(dropType, itemId)
    local cfg
    if dropType == DropType.Item then
        cfg = ItemCfg:Get(itemId)
    else
        cfg = DropTypeIconCfg:Get(dropType)
    end
    return cfg
end

function Utils.SendGMCommand(gmstr)
    local msg = CGMCommand:New();
    msg.cmd = gmstr;
    print("send gm:" .. msg.cmd)
    if (OnlineMgr.IsInBattle) then
        OnlineMgr:SendBs(msg);
    else
        OnlineMgr:SendGs(msg);
    end
end

---@param time1 number
---@param time2 number
---@return boolean
function Utils.IsSameTotalWeek5(time1, time2)
    return Utils.GetTotalWeek_5(time1) == Utils.GetTotalWeek_5(time2)
end

---@private
---@param time number
---@return number
function Utils.GetTotalWeek_5(time)
    return (time + TimeZoneOff - GameDayChangeSeconds - 4 * DayOfSeconds) / WeekOfSeconds
end

---@param time1 number
---@param time2 number
---@return boolean
function Utils.IsSameWeek(time1, time2)
    return Utils.GetWeek(time1) == Utils.GetWeek(time2)
end

---@private
---@param time number
---@return number
function Utils.GetWeek(time)
    return (time + TimeZoneOff - 4 * DayOfSeconds) / WeekOfSeconds
end

---@return number
function Utils.GetLocalDay()
    return Utils.GetLocalDayByNow(Utils.GetTime())
end

---@param time number
function Utils.GetLocalDayByNow(time)
    local weekNum = os.date("*t", time).wday - 1
    if weekNum == 0 then
        weekNum = 7
    end
    return weekNum
end

---@return number @自定义天变化秒数
function Utils.GameDayChangeSeconds()
    return GameDayChangeSeconds
end

---@return number @一周的秒数
function Utils.WeekOfSeconds()
    return WeekOfSeconds
end

---@return number @零点
function Utils.GetZeroTime()
    return Utils.GetZeroTimeByTime(Utils.GetTime())
end

---@param time number
---@return number @零点
function Utils.GetZeroTimeByTime(time)
    return time - (time + TimeZoneOff) % DayOfSeconds
end

function Utils.StringStartWith(String, Start)
    return string.sub(String, 1, string.len(Start)) == Start
end

function Utils.StringEndWith(String, End)
    return string.sub(String, -string.len(End)) == End
end

---@param v1 FVector
---@param v2 FVector
---@return number
function Utils.SquareDistance(v1, v2)
    return (v1.X - v2.X) * (v1.X - v2.X) + (v1.Y - v2.Y) * (v1.Y - v2.Y) + (v1.Z - v2.Z) * (v1.Z - v2.Z)
end

---@param v1 FVector
---@param v2 FVector
---@return number
function Utils.SquareDistance2D(v1, v2)
    return (v1.X - v2.X) * (v1.X - v2.X) + (v1.Y - v2.Y) * (v1.Y - v2.Y)
end

---@param beginPosition FVector @起点位置
---@param endPosition FVector @终点位置
---@return number @距离
function Utils.GetDistanceByFVector(beginPosition, endPosition)
    return Utils.GetDistance(beginPosition.X, endPosition.X, beginPosition.Y, endPosition.Y, beginPosition.Z, endPosition.Z)
end

---@param beginPosition FVector @起点位置
---@param endPosition FVector @终点位置
---@return number @距离
function Utils.GetDistanceByFVector2D(beginPosition, endPosition)
    return Utils.GetDistance(beginPosition.X, endPosition.X, beginPosition.Y, endPosition.Y, 0, 0)
end

---@param beginPosition Vector3 @起点位置
---@param endPosition Vector3 @终点位置
---@return number @距离
function Utils.GetDistanceByVector3(beginPosition, endPosition)
    return Utils.GetDistance(beginPosition:GetX(), endPosition:GetX(), beginPosition:GetY(), endPosition:GetY(), beginPosition:GetZ(), endPosition:GetZ())
end

---@param beginPosition Vector @起点位置
---@param endPosition Vector @终点位置
---@return number @距离
function Utils.GetDistanceByVector(beginPosition, endPosition)
    return Utils.GetDistance(beginPosition.x, endPosition.x, beginPosition.y, endPosition.y, beginPosition.z, endPosition.z)
end

---@param beginPositionX number
---@param endPositionX number
---@param beginPositionZ number
---@param endPositionZ number
---@return number @距离
function Utils.GetDistance(beginPositionX, endPositionX, beginPositionY, endPositionY, beginPositionZ, endPositionZ)
    return math.sqrt((endPositionX - beginPositionX) * (endPositionX - beginPositionX) + (endPositionY - beginPositionY) * (endPositionY - beginPositionY) + (endPositionZ - beginPositionZ) * (endPositionZ - beginPositionZ))
end

---@param beginPosition FVector @起点位置
---@param endPosition FVector @终点位置
---@return FVector @距离
function Utils.GetDirectionByFVector(beginPosition, endPosition)
    ---@type FVector
    local direction = endPosition - beginPosition
    local directionX, directionY, directionZ = Utils.GetSafeNormal(direction.X, direction.Y, direction.Z)
    return UE4.FVector(directionX, directionY, directionZ)
end

---@param beginPosition Vector3 @起点位置
---@param endPosition Vector3 @终点位置
---@return Vector3
function Utils.GetDirectionByVector3(beginPosition, endPosition)
    ---@type Vector3
    local direction = endPosition - beginPosition
    local directionX, directionY, directionZ = Utils.GetSafeNormal(direction:GetX(), direction:GetY(), direction:GetZ())
    local newDirection = Vector3:new()
    newDirection:Set(directionX, directionY, directionZ)
    return newDirection
end

---@param directionX number
---@param directionY number
---@param directionZ number
---@return number,number,number
function Utils.GetSafeNormal(directionX, directionY, directionZ)
    local squareSum = Utils.Size(directionX, directionY, directionZ)
    if (squareSum == 1) then
        return directionX, directionY, directionZ
    end

    if (squareSum == 0) then
        return 0, 0, 0
    end

    local scale = 1.0 / math.sqrt(squareSum)
    directionX = directionX * scale
    directionY = directionY * scale
    directionZ = directionZ * scale
    return directionX, directionY, directionZ
end

---@param directionX number
---@param directionY number
---@param directionZ number
---@return number
function Utils.Size(directionX, directionY, directionZ)
    return directionX * directionX + directionY * directionY + directionZ * directionZ
end

---@param fVector FVector
---@param vector Vector
function Utils.FVectorToVector(fVector, vector)
    if (fVector ~= nil and vector ~= nil) then
        vector.x = fVector.X
        vector.y = fVector.Y
        vector.z = fVector.Z
    end
end

---@param vector Vector
---@param fVector FVector
function Utils.Vector3ToFVector(vector, fVector)
    if (fVector ~= nil and vector ~= nil) then
        fVector.X = vector.x
        fVector.Y = vector.y
        fVector.Z = vector.z
    end
end

---@param fRotator FRotator
---@param rototor Rotator
function Utils.FRotator2Rotator(fRotator, rotator)
    if (fRotator ~= nil and rotator ~= nil) then
        rotator.pitch = fRotator.Pitch
        rotator.yaw = fRotator.Yaw
        rotator.roll = fRotator.Roll
    end
end

---@param rotator Rotator
---@param fRotator FRotator
function Utils.Rotator2FRotator(rotator, fRotator)
    if (fRotator ~= nil and rotator ~= nil) then
        fRotator.Pitch = rotator.pitch
        fRotator.Yaw = rotator.yaw
        fRotator.Roll = rotator.roll
    end
end

---@param fVector FVector
---@return number
function Utils.GetFVectorSize(fVector)
    return math.sqrt(fVector.X * fVector.X + fVector.Y * fVector.Y + fVector.Z * fVector.Z)
end

---@param v1 FVector
---@param v2 FVector
function Utils.GetAngleBetweenVectors2D(v1, v2)
    v1 = UE4.UKismetMathLibrary.Normal(v1);
    v2 = UE4.UKismetMathLibrary.Normal(v2);
    return UE4.UKismetMathLibrary.DegAcos(v1:CosineAngle2D(v2))
end

---@param v1 FVector
---@param v2 FVector
function Utils.GetAngleBetweenVectors(v1, v2)
    v1 = UE4.UKismetMathLibrary.Normal(v1);
    v2 = UE4.UKismetMathLibrary.Normal(v2);
    return UE4.UKismetMathLibrary.DegAcos(v1:Dot(v2))
end

function Utils.GetAngleBetweenVectorsPlusOrMinus2D(v1, v2)
    v1 = UE4.UKismetMathLibrary.Normal(v1);
    v2 = UE4.UKismetMathLibrary.Normal(v2);
    local angle = UE4.UKismetMathLibrary.DegAcos(v1:CosineAngle2D(v2))
    local dir = v1:Cross(v2);
    if dir.Z < 0 then
        angle = -angle;
    end
    return angle;
end
---@param mesh UMeshComponent
---@param socketName string
function Utils.GetSocketPos(mesh, socketName)
    if mesh then
        return mesh:GetSocketLocation(socketName);
    end
    return nil
end

---@param min number
---@param max number
---@param value number
---@return number
function Utils.Clamp(min, max, value)
    if (value < min) then
        value = min
    elseif (value > max) then
        value = max
    end
    return value
end

function Utils.FormatFileSize(size)
    if (size < 1024) then
        return "" .. size .. "B"
    end
    if (size < 1024 * 1024) then
        return string.format("%0.2fK", size / 1024)
    end
    return string.format("%0.2fM", size / 1024 / 1024);
end

function Utils.Restart()
    if PlatformMgr.IsPc()then
        Utils.QuitGame();
    else
        CPP:RestartApp();
    end
end

function Utils.QuitGame()
    UE4.UKismetSystemLibrary.QuitGame(Global.g_gameInstance, nil, EQuitPreference.Quit, true);
end

---@param needShuffleList ArrayList
function Utils.ShuffleList(needShuffleList)
    for j = needShuffleList:size(), 1, -1 do
        local randomIndex = math.random(j)
        needShuffleList = needShuffleList:swap(j, randomIndex)
    end
end

function Utils.GetBsSeverTimeZone()
    return BattleShopMgr:GetTimeZone()
end

function Utils.GetBsSeverTime()
    return BattleMgr:GetBsSeverTime()
end

function Utils.GetBsSeverRealTime()
    return Utils.GetBsSeverTime() - (Utils.GetTimeZone() - WatchMgr.BsInfo.timeZone*3600)
end

function Utils.GetGsSeverTimeZone()
    return ShopMgr:GetTimeZone()
end

function Utils.GetGsSeverTime()
    return Utils.GetTime()
end

---@param posX number
---@param posY number
---@param radius number
---@param useFixRadius boolean 
function Utils.RandomPoint(posX, posY, radius, useFixRadius)
    local randomRadius = radius
    if(not useFixRadius) then
        randomRadius = math.random(0, math.floor(radius))
    end
    local randomAngle = math.rad(math.random(0, 360))
    return posX + randomRadius * math.sin(randomAngle), posY + randomRadius * math.cos(randomAngle), randomRadius
end

---@param actor AActor
function Utils.CloseActor(actor, hideChild)
    if not actor or not UE4.UKismetSystemLibrary.IsValid(actor) then
        return ;
    end
    actor:SetActorHiddenInGame(true);
    actor:SetActorTickEnabled(false);
    actor:SetActorEnableCollision(false);
    if hideChild == nil or hideChild then
        local childActors = TArray(AActor);
        actor:GetAttachedActors(childActors);
        local outActorsNum = childActors:Length();
        for j = 1, outActorsNum do
            ---@type AActor
            local a = childActors:GetRef(j);
            if a ~= actor then
                Utils.CloseActor(a, hideChild);
            end
        end
    end
end

---@param actor AActor
function Utils.DestroyActor(actor, hideChild)
    if hideChild == nil or hideChild then
        local childActors = TArray(AActor);
        actor:GetAttachedActors(childActors);
        local outActorsNum = childActors:Length();
        for j = 1, outActorsNum do
            ---@type AActor
            local a = childActors:GetRef(j);
            if a ~= actor then
                Utils.DestroyActor(a, hideChild);
            end
        end
    end
    actor:K2_DestroyActor()
end

---@param actor AActor
function Utils.OpenActor(actor, openRender, openTick, openCollision)
    if not actor then
        return;
    end
    actor:SetActorHiddenInGame(not openRender);
    actor:SetActorTickEnabled(openTick);
    actor:SetActorEnableCollision(openCollision);
    local childActors = TArray(AActor);
    actor:GetAttachedActors(childActors);
    local outActorsNum = childActors:Length();
    for j = 1, outActorsNum do
        ---@type AActor
        local a = childActors:GetRef(j);
        if a ~= actor then
            Utils.OpenActor(a, openRender, openTick, openCollision)
        end
    end
end

function Utils.SetActorHidden(Actor,bNewHidden, bSetChild)
    if not Actor then
        return;
    end
    Actor:SetActorHiddenInGame(bNewHidden);
    if bSetChild then
        local ChildActors = TArray(AActor);
        Actor:GetAttachedActors(ChildActors);
        local OutActorsNum = ChildActors:Length();
        for j = 1, OutActorsNum do
            ---@type AActor
            local ChildActor = ChildActors:GetRef(j);
            ChildActor:SetActorHiddenInGame(bNewHidden);
        end
    end
end

---@param component USceneComponent
function Utils.SetComponentCollision(component, isEnable, setChild)
    if component == nil then
        return ;
    end
    if isEnable == nil then
        isEnable = true
    end
    local enable = isEnable and UE4.ECollisionEnabled.QueryOnly or UE4.ECollisionEnabled.NoCollision
    if component.SetCollisionEnabled then
        component:SetCollisionEnabled(enable);
    end
    setChild = setChild == nil and true or setChild
    if not setChild then
        return
    end
    local childActors = TArray(USceneComponent);
    component:GetChildrenComponents(true, childActors);
    local outActorsNum = childActors:Length();
    for j = 1, outActorsNum do
        ---@type AActor
        local a = childActors:GetRef(j);
        if a.SetCollisionEnabled then
            a:SetCollisionEnabled(enable)
        end
    end
end

---@param txtCtrl UTextBlock
---@param format string "%0.2f"
---@param value number [0,1]
function Utils.SetTextPercent(txtCtrl, format, value)
    if (value >= 1) then
        txtCtrl:SetText("100%")
    else
        local str = string.format(format, value * 100)
        txtCtrl:SetText(str)
    end
end

---@param t UObject @func target object
---@param func function
---@param time number
---@return FTimerHandle
function Utils.SetTimerByFunc(t, func, time)
    return UE4.UKismetSystemLibrary.K2_SetTimerDelegate({ t, func }, time, false, 0, 0)
end

function Utils.GetTimeHHMMByStamp(stamp)
    local oneDaySeconds = stamp % 86400
    return string.format("%02d", math.floor(oneDaySeconds / 3600)) .. ":" .. string.format("%02d", math.floor((oneDaySeconds % 3600) / 60))
end

function Utils.GetHourWithTimeZone()
    return math.floor((Utils.GetTime() - Utils.GetZeroTime()) / 3600)
end

---@return number @一小时的秒数
function Utils.HourOfMillSeconds()
    return HourOfMillSeconds
end

---@param string string @todo 铭感词检查
function Utils.IsValidString(string)
    return true
end

function Utils.LogFatal(moduleName, formatStr, ...)
    CPP:Log(1, moduleName, string.format(formatStr, ...))
end

function Utils.LogError(moduleName, formatStr, ...)
    CPP:Log(2, moduleName, string.format(formatStr, ...))
end

function Utils.LogWarning(moduleName, formatStr, ...)
    CPP:Log(3, moduleName, string.format(formatStr, ...))
end

function Utils.LogInfo(moduleName, formatStr, ...)
    CPP:Log(4, moduleName, string.format(formatStr, ...))
end

---@return UNiagaraComponent
function Utils.PlayEffectAtPosByPath(path, pos, rotator, world, bSync)
    local WorldContextObject = Global.g_gameInstance:GetWorld();
    if BuildUtils.Test then
        WorldContextObject = BattleUtils.SelfPlayer;
    end
    if world then
        WorldContextObject = world;
    end
    if bSync then
        local e = Global.g_resMgr:Load(path);
        if not e then
            print("PlayEffectAtPos  error:", path);
            return nil;
        end
        return UE4.UNiagaraFunctionLibrary.SpawnSystemAtLocation(WorldContextObject, e, pos, rotator, UE4.FVector(1, 1, 1), true, true, ENCPoolMethod.AutoRelease, true);
    else
        local tempPos =UE4.FVector(pos.X, pos.Y, pos.Z)
        local tempRotator = rotator;
        if tempRotator then
            tempRotator =UE4.FRotator(rotator.Pitch, rotator.Yaw, rotator.Roll)
        end
        Global.g_luaMgr:LoadResAsync(path, { WorldContextObject, function(s, _, b)
            if (not b) or (not b:IsValid()) or (not s) then
                return ;
            end
            UE4.UNiagaraFunctionLibrary.SpawnSystemAtLocation(s, b, tempPos, tempRotator, UE4.FVector(1, 1, 1), true, true, ENCPoolMethod.AutoRelease, true);
        end }, 0, true)
    end
end

---@return UNiagaraComponent
function Utils.PlayEffectAtPosById(id, pos, rotator, world, bSync)
    if id == -1 then
        return nil;
    end
    if not EffectCfg:Get(id) then
        print("PlayEffectAtPosById  error:", id);
        return nil;
    end
    return Utils.PlayEffectAtPosByPath(EffectCfg:Get(id).path, pos, rotator, world, bSync);
end

---@return UNiagaraComponent
function Utils.PlayEffectAttachedByPath(path, AttachToComponent, AttachPointName, Location, Rotation, bSync)
    if bSync then
        local e = Global.g_resMgr:Load(path);
        if not e then
            print("PlayEffectAttachedByPath  error:", path);
            return nil;
        end
        return UE4.UNiagaraFunctionLibrary.SpawnSystemAttached(e, AttachToComponent, AttachPointName, Location, Rotation, UE4.EAttachLocation.SnapToTarget, true, true, ENCPoolMethod.ManualRelease, true);
    else
        local tempPos = Location;
        if Location then
            tempPos =UE4.FVector(Location.X, Location.Y, Location.Z)
        end
        local tempRotator = Rotation;
        if Rotation then
            tempRotator =UE4.FRotator(Rotation.Pitch, Rotation.Yaw, Rotation.Roll)
        end
        Global.g_luaMgr:LoadResAsync(path, { AttachToComponent, function(s, _, b)
            if (not b) or (not b:IsValid()) or (not s) then
                print("PlayEffectAttachedByPath  error:", _);
                return ;
            end
            UE4.UNiagaraFunctionLibrary.SpawnSystemAttached(b, s, AttachPointName, tempPos, tempRotator, UE4.EAttachLocation.SnapToTarget, true, true, ENCPoolMethod.ManualRelease, true);
        end })
    end

end

---@return UNiagaraComponent
function Utils.PlayEffectAttachedById(id, AttachToComponent, AttachPointName, Location, Rotation, bSync)
    if id == -1 then
        return nil;
    end
    if not EffectCfg:Get(id) then
        print("PlayEffectAttachedById  error:", id);
        return nil;
    end
    local path = EffectCfg:Get(id).path;
    return Utils.PlayEffectAttachedByPath(path, AttachToComponent, AttachPointName, Location, Rotation, bSync);
end

---@param string string
---@return number
function Utils.GetStringLength(string)
    return Global.g_luaMgr:GetStringAsciiLen(string)
end

---@param cfgTable table
---@return ArrayList
function Utils.GetCfgAsList(cfgTable)
    local cfgList = ArrayList:new()
    local cfgIds = ArrayList:new()
    local cfgId2Cfg = HashMap:new()
    for k, v in pairs(cfgTable) do
        if (type(k) == "number" or type(k) == "string") then
            cfgIds:add(k)
            cfgId2Cfg:add(k, v)
        end
    end
    cfgIds:sort()
    for _, cfgId in pairs(cfgIds) do
        cfgList:add(cfgId2Cfg:get(cfgId))
    end
    return cfgList
end

---@param str string
---@return boolean
function Utils.IsStringNilOrEmpty(str)
    return str == nil or str == ""
end

---@param startPos FVector
---@param endPos FVector
---@param distance number
---@return FVector
function Utils.MoveTowards(startPos, endPos, distance)
    local totalDistance = Utils.GetDistanceByFVector(startPos, endPos)
    if (totalDistance == 0) then
        return UE4.FVector(startPos.X, startPos.Y, startPos.Z)
    else
        local a = distance / totalDistance
        local b = 1 - a
        return UE4.FVector(a * endPos.X + b * startPos.X, a * endPos.Y + b * startPos.Y, a * endPos.Z + b * endPos.Z)
    end
end

---@param startPos Vector
---@param endPos Vector
---@param distance number
---@return number,number,number
function Utils.MovePosTowards(startPos, endPos, distance)
    local totalDistance = Utils.GetDistanceByVector(startPos, endPos)
    if (totalDistance == 0) then
        return startPos.x, startPos.y, startPos.z
    else
        local a = distance / totalDistance
        local b = 1 - a
        return a * endPos.x + b * startPos.x, a * endPos.y + b * startPos.y, a * endPos.z + b * startPos.z
    end
end

Utils.m_progressBarSmooth = {}

---@describe linear speed is 0.25/s(4 seconds will from 0 to 1)
---@param circleBarWidget WB_ProgressBarCircular
function Utils.ChangeCircleBarValueSmooth(circleBarWidget, finalPercentValue, maxValue, valueChangeCallback, valueChangeCallbackTable)
    local t = Utils.m_progressBarSmooth[circleBarWidget]
    if t then
        Global.g_Ticker:UnRegisterTick(circleBarWidget, Utils.ChangeCircleBarValueSmooth_Impl)
    else
        t = {}
        Utils.m_progressBarSmooth[circleBarWidget] = t
    end

    t.maxValue = maxValue
    t.bAdd = circleBarWidget.Percent < finalPercentValue
    t.endPercent = finalPercentValue
    t.valueChangeCallback = valueChangeCallback
    t.valueChangeCallbackTable = valueChangeCallbackTable
    t.addnum = math.abs(t.endPercent - circleBarWidget.Percent) / 60;
    if (t.endPercent <= 0) then
        t.addnum = t.addnum * 5;
    end
    Global.g_Ticker:RegisterTick(circleBarWidget, Utils.ChangeCircleBarValueSmooth_Impl, 0)
end

function Utils.ChangeCircleBarValueSmooth_Impl(circleBarWidget)
    local t = Utils.m_progressBarSmooth[circleBarWidget]
    if not t then
        return -1
    end

    circleBarWidget:SetPercent(circleBarWidget.Percent + t.addnum * (t.bAdd and 1 or -1))
    if math.abs(circleBarWidget.Percent - t.endPercent) < 0.005 then
        circleBarWidget:SetPercent(t.endPercent)
        t.valueChangeCallback(t.valueChangeCallbackTable, t.endPercent, t.maxValue)
        Utils.m_progressBarSmooth[circleBarWidget] = nil
        return -1
    end

    t.valueChangeCallback(t.valueChangeCallbackTable, circleBarWidget.Percent, t.maxValue)

    return 17
end

function Utils.GetMoneyTypeIcon(moneyType)

    for k, v in pairs(DropTypeIconCfg) do
        if (type(k) == "number") then
            if (v.moneyType == moneyType) then
                return v.icon
            end
        end
    end
    return ""
end

---截取中英混合的UTF8字符串，endIndex可缺省
function Utils.SubStringUTF8(str, startIndex, endIndex)
    if startIndex < 0 then
        startIndex = Utils.SubStringGetTotalIndex(str) + startIndex + 1;
    end

    if endIndex ~= nil and endIndex < 0 then
        endIndex = Utils.SubStringGetTotalIndex(str) + endIndex + 1;
    end

    if endIndex == nil then
        return string.sub(str, Utils.SubStringGetTrueIndex(str, startIndex));
    else
        return string.sub(str, Utils.SubStringGetTrueIndex(str, startIndex), Utils.SubStringGetTrueIndex(str, endIndex + 1) - 1);
    end
end

---获取中英混合UTF8字符串的真实字符数量
function Utils.SubStringGetTotalIndex(str)
    local curIndex = 0;
    local i = 1;
    local lastCount = 1;
    repeat
        lastCount = Utils.SubStringGetByteCount(str, i)
        i = i + lastCount;
        curIndex = curIndex + 1;
    until (lastCount == 0);
    return curIndex - 1;
end

function Utils.SubStringGetTrueIndex(str, index)
    local curIndex = 0;
    local i = 1;
    local lastCount = 1;
    repeat
        lastCount = Utils.SubStringGetByteCount(str, i)
        i = i + lastCount;
        curIndex = curIndex + 1;
    until (curIndex >= index);
    return i - lastCount;
end

---返回当前字符实际占用的字符数
function Utils.SubStringGetByteCount(str, index)
    local curByte = string.byte(str, index)
    local byteCount = 1;
    if curByte == nil then
        byteCount = 0
    elseif curByte > 0 and curByte <= 127 then
        byteCount = 1
    elseif curByte >= 192 and curByte <= 223 then
        byteCount = 2
    elseif curByte >= 224 and curByte <= 239 then
        byteCount = 3
    elseif curByte >= 240 and curByte <= 247 then
        byteCount = 4
    end
    return byteCount;
end

function Utils.SubStringByLength(str, length)
    local len = string.len(str)
    if len > length then
        local index = 1
        repeat
            index = index + Utils.SubStringGetByteCount(str, index)
        until (index > length)
        str = string.sub(str, 1, index - 1)
    end
    return str
end

---@return DropTypeIconCfg
function Utils.GetMoneyTypeInfo(moneyType)

    for k, v in pairs(DropTypeIconCfg) do
        if (type(k) == "number") then
            if (v.moneyType == moneyType) then
                return v
            end
        end
    end
    return nil
end

function Utils.GetPreciseDecimal(num, n)
    if type(num) ~= "number" then
        return num;
    end
    n = n or 0;
    n = math.floor(n);
    if n < 0 then
        n = 0;
    end
    local decimal = 10 ^ n;
    local temp = math.floor(num * decimal);
    local result = temp / decimal;
    return result;
end

function Utils.GetFLinearColorByColorId(id)
    local colorR, colorG, colorB, colorA = Utils.GetRGBANumByColorString(ColorCfg:Get(id).color);
    local color = UKismetMathLibrary.MakeColor(colorR, colorG, colorB, colorA);
    return color;
end

---@param tag string
function Utils.StartCycleCounter(tag)
    FCycleCounter.Start(tag);
end

function Utils.StopCycleCounter()
    FCycleCounter.Stop();
end

function Utils.GetSkySystem()
    return Global.g_luaMgr:GetSkySystem();
end

---埋点，报SDK服务器
---@param eventName string
---@param paramName string
---@param paramValue string
---@param extraData string [Opt]
function Utils.ReportData(eventName, paramName, paramValue, extraData)
    CPP:Report(eventName, paramName, paramValue, extraData);
end

---埋点，报SDK服务器
---@param panelId string 界面ID
---@param buttonId string 按钮ID
---@param battleId string 战局ID，大厅传-1
---@param actionInfo string 动作 1点击，2曝光
---@param infoDes string 动作内容
local event_seq = 1;
function Utils.ReportButtonEvent(panelId, buttonId, battleId, actionInfo, infoDes)
    local Reportdata = { screen_id = panelId, button_id = buttonId, battle_id = battleId,
                         action = actionInfo, info = infoDes };
    Reportdata["server_id"] = CLoginMgr.GetRegions().serverId;
    if (RoleMgr._roleInfo) then
        Reportdata["role_id"] = RoleMgr._roleInfo.basicInfo.roleId;
        Reportdata["role_level"] = RoleMgr._roleInfo.basicInfo.level;
        Reportdata["rank"] = 0;
    end
    Reportdata["event_seq"] = event_seq;
    event_seq = event_seq + 1;
    CPP.ReportTable("page_flow", Reportdata);
    Utils.LogInfo("sdk", "ReportButtonEvent screen_id=%s buttonId=%s", panelId, buttonId)
end


function Utils.ReportSDKKillPlayer(body,killPos,enemyPos,downTimeStamp)
    local Reportdata = { };
    Reportdata["server_id"] = CLoginMgr.GetRegions().serverId;
    if (RoleMgr._roleInfo) then
        Reportdata["role_id"] = RoleMgr._roleInfo.basicInfo.roleId;
        Reportdata["role_name"] = RoleMgr._roleInfo.basicInfo.name;
        Reportdata["battle_id"] = CLoginMgr.GetRegions().serverId;
        Reportdata["battle_server_id"] = CLoginMgr.GetRegions().serverId;
        Reportdata["game_type"] = WarChooseMgr.gameInfo.mode;
        Reportdata["map"] = 1;
        Reportdata["game_lvl"] = WatchMgr:GetWorldLevel();
        Reportdata["team_id"] = BsTroopMgr:GetTeamId();
        Reportdata["crosshair_track_list"] =  BattleUtils.GetSelfPlayer().CrossHairDirList:totable();
        Reportdata["shot_stamp_list"] =  BattleUtils.GetSelfPlayer().ShotStampList:totable();
        Reportdata["hit_stamp_list"] =  BattleUtils.GetSelfPlayer().HitStampList:totable();
        Reportdata["down_stamp"] = downTimeStamp;
        Reportdata["body"] = body;
        Reportdata["killer_coordinate"] =tostring(killPos);
        Reportdata["enemy_coordinate"] = tostring(enemyPos);
        CPP.ReportTable("aim_flow", Reportdata);
        Utils.LogInfo("sdk", "ReportSDKKillPlayer")
    end
end

function Utils.GetBatteryLevel()
    return CPP:GetBatteryLevel();
end

---报异常日志到SDK后台服务器
---@param module string
---@param reason string
---@param stacktrace string
function Utils.UploadException(module, reason, stacktrace)
    CPP:UploadException(module, reason, stacktrace);
end

---上传日志文件到SDK后台服务器
---@param filePath string
---@param roleId number
---@param roleName string
---@param callback FScriptDelegate
function Utils.UploadLogFile(filePath, roleId, roleName, callback)
    CPP:UploadLog(filePath, roleId, roleName, callback);
end

---@param time
function Utils.OnReceiveBsTime(time)
    BsStartTime = time
    SyncBsTime = Utils.GetTime()
end

---@return number
function Utils.GetCurrentBsTime()
    return BsStartTime + Utils.GetTime() - SyncBsTime
end

function Utils.ChangeJsonToLuaStr(str)
    str = string.gsub(str, "<p>", "")
    str = string.gsub(str, "</p>", "\n")
    str = string.gsub(str, "<br>", "\n")
    local arr = ArrayList:new()
    str = Utils.GetImageUrlInString(str, arr)
    return str, arr
end

function Utils.GetImageUrlInString(str, array)
    local start, endL = string.find(str, "<img src=.+>")
    if start ~= nil then
        local url = string.sub(str, start, endL)
        local str2 = ""
        if start > 1 then
            str2 = string.sub(str, 1, start - 1)
        end
        if endL < string.len(str) then
            str2 = str2 .. string.sub(str, endL + 1, -1)
        end
        url = string.gsub(url, "<img src=\"", "")
        start = string.find(url, "\"")
        url = string.sub(url, 1, start - 1)
        array:add(url)
        return Utils.GetImageUrlInString(str2, array)
    else
        return str
    end
end

---@param pos FVector
---@return string
function Utils.FormatStatPos(pos)
    return string.format("{%f,%f,%f}", pos.X, pos.Y, pos.Z)
end

---@param hashMap HashMap
---@param key any
---@param value number
function Utils.UpdateHashMapValue(hashMap, key, value)
    local oldValue = hashMap:get(key)
    if (oldValue == nil) then
        hashMap:add(key, value)
    else
        hashMap:add(key, oldValue + value)
    end
end

---@param num1 number
---@param num2 number
---@param tolerance number
---@return boolean
function Utils.IsNumberNear(num1, num2, tolerance)
    if (not tolerance) then
        tolerance = 0
    end
    return math.abs(num1 - num2) <= tolerance
end

function Utils.IsAllSpaceInString(str)
    return string.gsub(str, "%s+$", ""):len() == 0
end

function Utils.OnWebViewUrlClosed(obj,url)
    --print("url is close ",url)
    EventMgr:Instance():TriggerEvent(PlatformMgr.OnCloseUrl, url)
end

function Utils.OnWebViewOpenUrlError(obj,url, code, msg, extraErrorCode,extraErrorMsg, strAdditionalInfo)
    Utils.LogError("sdk", "open %s error, %d:%s,%d:%s, %s ", url, code, msg, extraErrorCode,extraErrorMsg, strAdditionalInfo)
end

function Utils.FindStringWithOutCase(s1, s2)
    s1 = string.lower(s1)
    s2 = string.lower(s2)
    return string.find(s1, s2)
end

function Utils.SetUImageTextureByUrl(image, url)
    local downloadHandle = UE4.UAsyncTaskDownloadImage.DownloadImage(url)
    downloadHandle.OnSuccess:Add(Global.g_gameInstance, function(ins, texture)
        image:SetBrushFromTextureDynamic(texture)
    end)
end

---@param actor AActor
function Utils.IsBlockDoorActor(actor)
    if(actor == nil) then
        return false
    end
    if(actor:ActorHasTag("Vehicle") or actor:ActorHasTag("Rider")) then
        return true
    end
    if(actor:ActorHasTag("Player") and (not actor.bOffline)) then
        return true
    end
    return false
end

--m到n 连续数字 返回count个不同随机数 
---@return ArrayList
function Utils.RandomNumbers(m , n , count)
    if(n - m + 1 < count)then
        return ArrayList:new();
    end
    local tmp = ArrayList:new();
    for i = m , n , 1 do
        tmp:add(i);
    end

    local x = 0;
    local t = ArrayList:new();
    math.randomseed(os.time())
    while count > 0 do
        x = math.random(1,n - m + 1)
        t:add(tmp:get(x))
        tmp:removeAt(x)
        count = count - 1
        m = m + 1
    end
    return t

end

---@param strPermisson string
---@return boolean 是否显示了申请权限的弹窗，false表示没有弹窗，主要是因为权限被禁止并再不提示，需要三秒提示等操作
function Utils.RequestPermission(strPermisson)
    local str = Global.g_luaMgr:GetIni("Permission", strPermisson)
    local bFirst = string.len(str) == 0;
    local bShowUi = Global.g_luaMgr:RequestPermission(strPermisson, bFirst)
    Global.g_luaMgr:WriteIni("Permission", strPermisson, "1")
    return bShowUi;
end

--require("CoroutineMgr")
--local json = require("Json.json")
-----@param strContent string 翻译内容
-----@param strTargetLanguage string 目标语言简写
-----@param fun function 回调函数
-----@param obj any 回调附带参数
--function Utils.TranslateText(strContent, strTargetLanguage, fun, obj)
--    if(fun == nil)then
--        return
--    end
--    CoroutineMgr.Resume(CoroutineMgr.Create(),function()
--        local body=string.format("{\"TargetLanguage\":\"%s\",\"TextList\":[\"%s\"]}", strTargetLanguage, strContent);
--        local strResutl = FLuaHttpRequest.TranslateText("https://open.byteplusapi.com/?Action=TranslateText&Version=2020-06-01","POST",body);
--        local trans = json.decode(strResutl)
--        if(trans ~= nil and trans.TranslationList)then
--            fun(obj,trans.TranslationList[1].Translation)
--        else
--            Utils.LogError("sdk","TranslateText error %s", strResutl)
--        end
--    end);
--end


---埋点，报SDK服务器
---@param question_id string 新手答题表id
---@param question_order string 第几道题
---@param action string 跳过0 1选1 2选2 3选3
---@param result string 错误0 正确1
---@param recommended_battle string 推荐战局 新手0 
function Utils.ReportQuestionEvent(question_id, question_order, action, result, recommended_battle)
    local ReportData = { question_id = question_id, question_order = question_order, action = action,
                         result = result, recommended_battle = recommended_battle };

    ReportData["server_id"] = CLoginMgr.GetRegions().serverId;
    if (RoleMgr._roleInfo) then
        ReportData["role_id"] = RoleMgr._roleInfo.basicInfo.roleId;
        ReportData["role_level"] = RoleMgr._roleInfo.basicInfo.level;
        ReportData["rank"] = 0;
    end
    
    --Reportdata["event_seq"] = event_seq;
    --event_seq = event_seq + 1;
    CPP.ReportTable("question_flow", ReportData);
    Utils.LogInfo("sdk", "ReportQuestionEvent question_id=%s", question_id)
end

---判断传入的时间戳是不是今天
---@param timestamp number 时间戳
function Utils.isToday(timestamp)
    local oldTime = Utils.FormatUnixTime2Date(timestamp)
    local newTime = Utils.FormatUnixTime2Date(os.time())
    print(oldTime.day,newTime.day, oldTime.month, newTime.month, oldTime.year, newTime.year)
    if oldTime.day == newTime.day and oldTime.month == newTime.month and oldTime.year == newTime.year then
        return true
    end
    return false
end

---@param object UObject
---@return string
function Utils.GetObjectName(object)
    return UE4.UKismetSystemLibrary.GetObjectName(object)
end

function Utils.GetGEClassByName(name)
    if not Utils.cacheClass then
        Utils.cacheClass = {}
    end
    local class = Utils.cacheClass[name]
    if not class then
        class = UE4.UClass.Load("/Game/Blueprints/GAS/GE/" .. name)
        if not class then
            print("Utils.GetGEClassByName Path error:",name);
        end
        Utils.cacheClass[name] = class
    end
    return class
end

function Utils.GetGECDOByName(name)
    local GEClass = Utils.GetGEClassByName(name)
    if not GEClass then
        return nil
    end
    return Global.g_luaMgr:GetCDO(GEClass)
end

function Utils.GetIAClassByName(name)
    if not Utils.IACacheClass then
        Utils.IACacheClass = {}
    end
    local class = Utils.IACacheClass[name]
    if not class then
        local path = "/Game/Input/IA/" .. name .. "." .. name
        class = Global.g_resMgr:Load(path)
        if not class then
            print("Utils.GetIAClassByName Path error:",name);
        end
        Utils.IACacheClass[name] = class
    end
    return class
end

function Utils.GetGamePlayTagContainerByStr(str)
    local tag = UE4.FGameplayTag()
    tag.TagName = str
    return UBlueprintGameplayTagLibrary.MakeGameplayTagContainerFromTag(tag);
end

function Utils.GetGamePlayTagByStr(str)
    local tag = UE4.FGameplayTag()
    tag.TagName = str
    return tag;
end

--must be assigned in ini
function Utils.RequireGameplayTagByStr(str)
    return Global.g_luaMgr:GetGameplayTagByString(str)
end

function Utils.SetMouseInBattle(isInBattle)
    if isInBattle then
        Global.g_luaMgr:SetInputMode(false,true);
        if BattleUtils.GetSelfPlayer().MyPlayerController then
            BattleUtils.GetSelfPlayer().MyPlayerController:SetShowMouseCursor(false)
        else
            BattleUtils.GetSelfPlayer():GetController().bShowMouseCursor = false
        end
    else
        Global.g_luaMgr:SetInputMode(true,true);
        if BattleUtils.GetSelfPlayer() then
            if BattleUtils.GetSelfPlayer().MyPlayerController then
                BattleUtils.GetSelfPlayer().MyPlayerController:SetShowMouseCursor(true)
            else
                if BattleUtils.GetSelfPlayer():GetController() then
                    BattleUtils.GetSelfPlayer():GetController().bShowMouseCursor = true
                end
            end
        end
    end
end

local function LoadGECDO(Path)
    local class = Utils.GetGEClassByName(Path)
    Global.g_luaMgr:GetCDO(class)
end

---@private
function Utils.InitAllGE()
    for k,v in pairs(GECfg) do
        if type(k) == "number" then
            LoadGECDO(v.GE)
        end
    end
end

function Utils.GetPosAlongDir(origin,dir,distance)
    return UKismetMathLibrary.Add_VectorVector(origin, UKismetMathLibrary.Multiply_VectorFloat(dir,distance))
end

---@param widget UNiagaraSystemWidget
function Utils.SetEffectActive(widget, isActive)
    ---@type UNiagaraComponent
    local component = widget:GetNiagaraComponent()
    component:SetActive(isActive)
end

---@param effect UNiagaraComponent
function Utils.HideAndDelayRemoveEffect(effect)

    effect:SetActive(false)
    Utils.Delay(effect, 1, function()
        if BattleUtils.IsValid(effect) then
            effect:ReleaseToPool();
        end
    end)
end

---@param GE UGameplayEffect
function Utils.IsGEHasTag(GE, tag)
    local tags = UBlueprintGameplayTagLibrary.BreakGameplayTagContainer(GE.InheritableOwnedTagsContainer.CombinedTags)
    if tags:Contains(tag) then
        return true
    end
    tags = UBlueprintGameplayTagLibrary.BreakGameplayTagContainer(GE.InheritableGameplayEffectTags.CombinedTags)
    if tags:Contains(tag) then
        return true
    end
    return false
end

---@param tArray TArray
---@return ArrayList
function Utils.TArrayToArrayList(tArray)
    local arrayList = ArrayList:new()
    local length = tArray:Length()
    for index = 1, length do
        arrayList:add(tArray:Get(index))
    end
    return arrayList
end

---@param obj any
---@param context any
function Utils.AddObjContext(obj,context)
    if Utils.ObjContextTable[obj] ~= nil then
        Utils.LogInfo("ObjContext","Old context is not nil!"..tostring(Utils.ObjContextTable[obj]))
    end
    Utils.ObjContextTable[obj] = context
end

function Utils.RemoveObjContext(obj)
    Utils.ObjContextTable[obj] = nil
end

function Utils.GetObjContext(obj)
    return Utils.ObjContextTable[obj]
end

---Plays an animation montage. Returns the length of the animation montage in seconds. Returns 0.f if failed to play.
---@param AnimInstance UAnimInstance
---@param MontageToPlay UAnimMontage
---@param InPlayRate number @[opt] 
---@param ReturnValueType EMontagePlayReturnType @[opt] 
---@param InTimeToStartMontageAt number @[opt] 
---@param bStopAllMontages boolean @[opt] 
---@param FinishTickOption EVisibilityBasedAnimTickOption
---@return number
function Utils.Montage_Play(AnimInstance,MontageToPlay, InPlayRate, ReturnValueType, InTimeToStartMontageAt, bStopAllMontages,FinishTickOption,bFinishCloseMeshTick)
    if not AnimInstance or (not MontageToPlay) then
        Utils.LogError("Utils:Montage_Play","Error");
        return;
    end
    if not InPlayRate then
        InPlayRate =1;
    end
    if not bFinishCloseMeshTick then
        bFinishCloseMeshTick =true;
    end
    Global.g_Ticker:ClearAllObjectTick(AnimInstance);
    local Mesh = AnimInstance:GetOwningComponent();
    Mesh:SetComponentTickEnabled(true);
    local AllTime = (MontageToPlay:GetPlayLength()+MontageToPlay:GetDefaultBlendOutTime())*InPlayRate*1.05;
    local CacheBasedAnimTickOption = Mesh.VisibilityBasedAnimTickOption;
    Mesh.VisibilityBasedAnimTickOption = UE4.EVisibilityBasedAnimTickOption.AlwaysTickPoseAndRefreshBones;
    if not FinishTickOption then
        FinishTickOption = CacheBasedAnimTickOption;
    end
    if Global.bServer then
        Global.g_Ticker:RegisterTick(AnimInstance,function()
            if BattleUtils.IsValid(Mesh) then
                Mesh.VisibilityBasedAnimTickOption = FinishTickOption;
                if bFinishCloseMeshTick==true then
                    Mesh:SetComponentTickEnabled(false);
                end
            end
        end,AllTime*1000)
    else
        Global.g_Ticker:RegisterTick(AnimInstance,function()
            if BattleUtils.IsValid(Mesh) then
                Mesh.VisibilityBasedAnimTickOption = FinishTickOption;
                if bFinishCloseMeshTick==true then
                    Mesh:SetComponentTickEnabled(false);
                end
            end
        end,AllTime*1000)
    end
    return AnimInstance:Montage_Play(MontageToPlay, InPlayRate, ReturnValueType, InTimeToStartMontageAt, bStopAllMontages);
end

function Utils.SetActorAllMeshRenderCustomDepth(actor , value)
    if BattleUtils.IsValid(BattleUtils.GetSelfPlayer()) then
        BattleUtils.GetSelfPlayer():SetPostProcessIsEnable(value)
    end
    if(actor:Cast(Utils.DoorBPName))then
        actor.SkeletalMesh:SetRenderCustomDepth(value);
    else
        local componentList = actor:K2_GetComponentsByClass(UE4.UMeshComponent)
        for i = 1, componentList:Length() do
            componentList:GetRef(i):SetRenderCustomDepth(value);
        end
    end

end

function Utils.GetWeakKeyValueTable()
    local o = {}
    setmetatable(o,mt)
    return o
end
---@param HitResult FHitResult
function Utils.CopyHitResult(HitResult)
    ---@type FHitResult
    local TempHitResult = UE4.FHitResult()
    TempHitResult.FaceIndex = HitResult.FaceIndex;
    TempHitResult.Time = HitResult.Time
    TempHitResult.Distance = HitResult.Distance
    TempHitResult.Location = HitResult.Location
    TempHitResult.ImpactPoint = HitResult.ImpactPoint
    TempHitResult.Normal = HitResult.Normal
    TempHitResult.ImpactNormal = HitResult.ImpactNormal
    TempHitResult.TraceStart = HitResult.TraceEnd
    TempHitResult.PenetrationDepth = HitResult.PenetrationDepth
    TempHitResult.Item = HitResult.Item
    TempHitResult.ElementIndex = HitResult.ElementIndex
    TempHitResult.bBlockingHit = HitResult.bBlockingHit
    TempHitResult.bStartPenetrating = HitResult.bStartPenetrating
    TempHitResult.PhysMaterial = HitResult.PhysMaterial
    TempHitResult.Actor = HitResult.Actor
    TempHitResult.Component = HitResult.Component
    TempHitResult.BoneName = HitResult.BoneName
    TempHitResult.MyBoneName = HitResult.MyBoneName
    return TempHitResult;
end

---@param sourceArray ArrayList
---@param resultArray ArrayList
---@param num number
function Utils.GetRandomNumInArray(sourceArray, resultArray, num)
    if (sourceArray:size() <= num) then
        resultArray:addAll(sourceArray)
    else
        local p = HashMap:new()
        local n = sourceArray:size()
        for i = 1, num do
            local index = math.random(i, n)
            while (p:contains(index)) do
                index = p:get(index)
            end
            resultArray:add(sourceArray:get(index))
            p:add(index, i)
        end
    end
end

return Utils
