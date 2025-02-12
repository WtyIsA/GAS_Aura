package notchtools.geek.com.notchtools.core;

import android.app.Activity;
import android.content.Context;
import android.view.Window;

import notchtools.geek.com.notchtools.helper.NotchStatusBarUtils;


/**
 * @author zhangzhun
 * @date 2018/11/4
 */

public abstract class AbsNotchScreenSupport implements INotchSupport {

    NotchProperty mNotchproperty = null;
    @Override
    public int getStatusHeight(Window window) {
        return NotchStatusBarUtils.getStatusBarHeight(window.getContext());
    }

    @Override
    public void fullScreenDontUseStatus(Activity activity, OnNotchCallBack notchCallBack) {
        NotchStatusBarUtils.setFullScreenWithSystemUi(activity.getWindow(), false);
        onBindCallBackWithNotchProperty(activity, notchCallBack);
    }

    @Override
    public void fullScreenDontUseStatusForPortrait(Activity activity, OnNotchCallBack notchCallBack) {
        fullScreenDontUseStatus(activity, notchCallBack);
    }

    @Override
    public void fullScreenDontUseStatusForLandscape(Activity activity, OnNotchCallBack notchCallBack) {
        NotchStatusBarUtils.setFullScreenWithSystemUi(activity.getWindow(), false);
        onBindCallBackWithNotchProperty(activity, notchCallBack);
        if (isNotchScreen(activity.getWindow())) {
            NotchStatusBarUtils.hideFakeNotchView(activity.getWindow());
        }
    }

    @Override
    public void fullScreenUseStatus(Activity activity, OnNotchCallBack notchCallBack) {
       // NotchStatusBarUtils.setFullScreenWithSystemUi(activity.getWindow(), false);
        onBindCallBackWithNotchProperty(activity, getNotchHeight(activity.getWindow()), notchCallBack);
    }

    @Override
    public void translucentStatusBar(Activity activity) {
        translucentStatusBar(activity, null);
    }

    @Override
    public void translucentStatusBar(Activity activity, OnNotchCallBack onNotchCallBack) {
        NotchStatusBarUtils.hideFakeNotchView(activity.getWindow());
        NotchStatusBarUtils.setStatusBarTransparent(activity.getWindow(), onNotchCallBack);
    }

    protected void onBindCallBackWithNotchProperty(Activity activity, OnNotchCallBack notchCallBack) {
        onBindCallBackWithNotchProperty(activity, getNotchHeight(activity.getWindow()), notchCallBack);
    }

    protected void onBindCallBackWithNotchProperty(Activity activity, int marginTop, OnNotchCallBack notchCallBack) {
        if (notchCallBack != null) {
            if(mNotchproperty == null) {
                mNotchproperty = new NotchProperty();
                mNotchproperty.setNotchHeight(getNotchHeight(activity.getWindow()));
                mNotchproperty.setNotch(isNotchScreen(activity.getWindow()));
                mNotchproperty.setMarginTop(marginTop);
            }

            mNotchproperty.setNotchHide(isHideNotch(activity.getApplicationContext()));
            if (notchCallBack != null) {
                notchCallBack.onNotchPropertyCallback(mNotchproperty);
            }
        }
    }

    @Override
    public boolean  isHideNotch(Context activity){
        return false;
    }
}
