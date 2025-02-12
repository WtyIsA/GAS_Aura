package notchtools.geek.com.notchtools.phone;

import android.app.Activity;
import android.content.Context;
import androidx.annotation.RequiresApi;

import android.provider.Settings;
import android.view.DisplayCutout;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;
import notchtools.geek.com.notchtools.core.AbsNotchScreenSupport;
import notchtools.geek.com.notchtools.core.OnNotchCallBack;

/**
 * targetApi>=28才能使用API，有的手机厂商在P上会放弃O适配方案，暂时针对P手机不做特殊处理
 * @author zhangzhun
 * @date 2018/11/5
 */

public class PVersionNotchScreen extends AbsNotchScreenSupport {

    private static final String DISPLAY_NOTCH_STATUS = "display_notch_status";

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
        attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_NEVER;
        activity.getWindow().setAttributes(attributes);
    }

    @RequiresApi(api = 28)
    @Override
    public void fullScreenUseStatus(Activity activity, OnNotchCallBack notchCallBack) {
        WindowManager.LayoutParams attributes = activity.getWindow().getAttributes();
        if(attributes.layoutInDisplayCutoutMode != WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES) {
            attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            activity.getWindow().setAttributes(attributes);
        }
        super.fullScreenUseStatus(activity, notchCallBack);
    }

    @Override
    public boolean isHideNotch(Context context) {
        int isNotchSwitchOpen = Settings.Secure.getInt(context.getContentResolver(), DISPLAY_NOTCH_STATUS, 0);
        return isNotchSwitchOpen == 1;
    }
}
