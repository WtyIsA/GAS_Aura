package notchtools.geek.com.notchtools.phone;

import android.app.Activity;
import androidx.annotation.RequiresApi;

import android.content.Context;
import android.provider.Settings;
import android.util.Log;
import android.view.DisplayCutout;
import android.view.View;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;


import notchtools.geek.com.notchtools.core.AbsNotchScreenSupport;
import notchtools.geek.com.notchtools.core.OnNotchCallBack;
import notchtools.geek.com.notchtools.helper.NotchStatusBarUtils;


/**
 * targetApi>=28才能使用API，有的手机厂商在P上会放弃O适配方案，暂时针对P手机不做特殊处理
 * @author zhangzhun
 * @date 2018/11/5
 */

public class PVersionNotchScreenWithFakeNotch extends AbsNotchScreenSupport {

    @RequiresApi(api = 28)
    @Override
    public boolean isNotchScreen(Window window) {
        WindowInsets windowInsets = window.getDecorView().getRootWindowInsets();
        if (windowInsets == null) {
            return false;
        }

        DisplayCutout displayCutout = windowInsets.getDisplayCutout();
        if(displayCutout == null || displayCutout.getBoundingRects() == null){
            return false;
        }
        return true;
    }

    @RequiresApi(api = 28)
    @Override
    public int getNotchHeight(Window window) {
        int notchHeight = 0;
        WindowInsets windowInsets = window.getDecorView().getRootWindowInsets();
        if (windowInsets == null) {
            return 0;
        }

        DisplayCutout displayCutout = windowInsets.getDisplayCutout();
        if(displayCutout == null || displayCutout.getBoundingRects() == null){
            return 0;
        }
        else {
            notchHeight = Math.max(displayCutout.getSafeInsetTop(), displayCutout.getSafeInsetRight());
            notchHeight = Math.max(displayCutout.getSafeInsetLeft(), notchHeight);
        }
        isHideNotch(window.getContext());

        return notchHeight;
    }

    @RequiresApi(api = 28)
    @Override
    public void fullScreenDontUseStatus(final Activity activity, OnNotchCallBack notchCallBack) {
        super.fullScreenDontUseStatus(activity, notchCallBack);
        if (!isNotchScreen(activity.getWindow())) {
            return;
        }
        WindowManager.LayoutParams attributes = activity.getWindow().getAttributes();
        if(attributes.layoutInDisplayCutoutMode != WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES) {
            attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            activity.getWindow().setAttributes(attributes);
            NotchStatusBarUtils.showFakeNotchView(activity.getWindow());
        }
    }

    @RequiresApi(api = 28)
    @Override
    public void fullScreenUseStatus(Activity activity, OnNotchCallBack notchCallBack) {
        WindowManager.LayoutParams attributes = activity.getWindow().getAttributes();
        if(attributes.layoutInDisplayCutoutMode != WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES) {
            attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            activity.getWindow().setAttributes(attributes);
            activity.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
            activity.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
            activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
        }
        super.fullScreenUseStatus(activity, notchCallBack);
    }

    public boolean isHideNotch(Context activity) {  
        return Settings.Global.getInt(activity.getContentResolver(),
                "force_black", 0) == 1;
    }
}
