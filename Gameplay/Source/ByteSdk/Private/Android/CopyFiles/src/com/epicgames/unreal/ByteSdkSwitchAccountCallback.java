package com.epicgames.unreal;

import com.bytedance.ttgame.rocketapi.account.ISwitchCallback;
import com.bytedance.ttgame.rocketapi.account.UserInfoResult;
import com.google.gson.Gson;

import androidx.annotation.Nullable;

public class ByteSdkSwitchAccountCallback implements ISwitchCallback<UserInfoResult> {
    public static Logger Log = new Logger("UE", "GameActivity");
    @Override
    public void onSuccess(@Nullable UserInfoResult result) {
        Gson gson = new Gson();
        String jsonData = gson.toJson(result.data.getExtraData());
        NativeCallBack.AccountCallback(NativeCallBack._accountSwitch, true, result.data.getToken(), jsonData, "");
    }

    @Override
    public void onFailed(@Nullable UserInfoResult exception) {
        if(exception == null){
            NativeCallBack.AccountCallback(NativeCallBack._accountSwitch, false, "-1", "UserInfoResult is null","-1");
            return;
        }
        Log.verbose("sdk SwitchAccount failed " + exception.gsdkError.toString());
        NativeCallBack.AccountCallback(NativeCallBack._accountSwitch, false, String.valueOf(exception.gsdkError.getCode()), exception.gsdkError.getMessage(),String.valueOf(exception.gsdkError.getExtraErrorCode()));
    }

    @Override
    public void onLogout(@Nullable UserInfoResult result) {
    }
}
