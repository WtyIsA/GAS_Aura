package com.epicgames.unreal;


import com.google.gson.Gson;
import com.ss.bytertc.engine.GameRTCEngine;
import com.ss.bytertc.engine.handler.IGameRTCEventHandler;
import com.ss.bytertc.engine.utils.LogUtil;

import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class RTCEventHandler implements IGameRTCEventHandler {
    private GameRTCEngine mVoiceEngine;
    public void SetRtcEngine(GameRTCEngine pVoice){
        mVoiceEngine = pVoice;
    }
    @Override
    public void onJoinRoomResult(String roomId, String userId, JoinRoomErrorCode joinRoomErrorCode, boolean isRejoined, int elapsed) {
        NativeCallBack.OnJoinRoomResult(roomId, userId, joinRoomErrorCode.value(), isRejoined);        
    }

    @Override
    public void onLeaveRoom(String roomId) {
        NativeCallBack.OnLeaveRoom();
    }

    @Override
    public void onUserJoined(String roomId, String userId) {
        NativeCalls.UELogLog("[rtc]voice userjoin " + userId);
    }

    @Override
    public void onUserLeave(String roomId, String userId, UserLeaveReasonType userLeaveReasonType) {
        NativeCalls.UELogLog("[rtc]voice userleave " + userId);
    }

    @Override
    public void onMicrophoneEnabled(String roomId, String userId, boolean bEnable) {
        NativeCallBack.OnMicrophoneEnabled(userId, bEnable);
    }

    @Override
    public void onAudioSendEnabled(String roomId, String userId, boolean bEnable) {

    }

    @Override
    public void onSpeakerphoneEnabled(String roomId, String userId, boolean bEnable) {

    }

    @Override
    public void onAudioVolumeIndication(String roomId, AudioVolumeInfoArray speakers, int totalVolume) {

        List<String> list = new ArrayList<String>();
        for(int i = 0; i < speakers.volumes.length; i++){
            if(speakers.volumes[i] > 0){
                list.add(speakers.userNames[i]);
            }
        }
        NativeCallBack.OnSpeakerList( list);
    }

    @Override
    public void onConnectionStateChanged(ConnectionState connectionState) {
        NativeCalls.UELogLog("[rtc]voice onConnectionStateChanged,connectionState=" + connectionState.value());
    }

    @Override
    public void onNetworkQuality(String roomId, String userId, NetworkQuality networkQuality, NetworkQuality networkQuality1) {

    }

    @Override
    public void onLogReport(String s, JSONObject jsonObject) {

    }

    @Override
    public void onEngineWarning(EngineWarnCode engineWarnCode) {
        NativeCalls.UELogWarning("[rtc]onEngineWarning,errorcode=" + engineWarnCode.value());
    }

    @Override
    public void onRoomWarning(String roomId, RoomWarnCode roomWarnCode) {
        NativeCalls.UELogWarning("[rtc]onRoomWarning roomid=" + roomId + ",errorcode=" + roomWarnCode.value());
    }

    @Override
    public void onRoomError(String roomId, RoomErrorCode roomErrorCode) {
        NativeCalls.UELogError("[rtc]onRoomError roomid=" + roomId + ",errorcode=" + roomErrorCode.value());
    }

    @Override
    public void onLoggerMessage(LogUtil.LogLevel logLevel, String s, Throwable throwable) {

    }

}
