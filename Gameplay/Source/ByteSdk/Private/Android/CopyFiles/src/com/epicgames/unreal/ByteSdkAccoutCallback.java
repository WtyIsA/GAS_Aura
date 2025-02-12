package com.epicgames.unreal;

import com.bytedance.ttgame.rocketapi.account.IAccountCallback;
import com.bytedance.ttgame.rocketapi.account.ISwitchCallback;
import com.bytedance.ttgame.rocketapi.account.UserInfoResult;
import com.google.gson.Gson;

import androidx.annotation.Nullable;

public class ByteSdkAccoutCallback implements IAccountCallback<UserInfoResult> {
    private int _accoutType;
    public ByteSdkAccoutCallback(int nType){
        _accoutType = nType;
    }
    public static int userType = 0;

    public static Logger Log = new Logger("UE", "GameActivity");
    @Override
    public void onSuccess(@Nullable UserInfoResult result) {
        String jsonData = "{}";
        String strToken = "";
        if(_accoutType == NativeCallBack._accountUserCenter){
            if(result != null){
                if(result.operationType == 8){
                    NativeCallBack.AccountCallback(NativeCallBack._accountLogout, true, strToken, jsonData, "");
                }
            }
            return;
        }

        if(result != null && result.data != null) {
            Gson gson = new Gson();
            jsonData = gson.toJson(result.data.getExtraData());
            strToken = result.data.getToken();
            userType = result.data.getExtraData().getUserType();
        }
        NativeCallBack.AccountCallback(_accoutType, true, strToken, jsonData, "");
    }

    @Override
    public void onFailed(@Nullable UserInfoResult exception) {
        if(exception == null){
            NativeCallBack.AccountCallback(_accoutType, false, "-1", "UserInfoResult is null","-1");
            return;
        }
        Log.verbose("sdk login failed " + exception.gsdkError.toString());
        NativeCallBack.AccountCallback(_accoutType, false, String.valueOf(exception.gsdkError.getCode()), exception.gsdkError.getMessage(),String.valueOf(exception.gsdkError.getExtraErrorCode()));
    }
}

