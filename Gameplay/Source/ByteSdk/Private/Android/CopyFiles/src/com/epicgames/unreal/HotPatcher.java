package com.epicgames.unreal;

import android.annotation.TargetApi;
import android.os.Build;
import android.text.TextUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class HotPatcher {
    public static Logger Log = new Logger("UE", "HotPatcher");
    private static File lastSoDir = null;

    public static synchronized boolean installNativeLibraryPath(ClassLoader classLoader, File folder)
            throws Throwable {
        if (classLoader == null || folder == null || !folder.exists()) {
            Log.error("classLoader or folder is illegal " + folder);
            return false;
        }
        final int sdkInt = Build.VERSION.SDK_INT;
        final boolean aboveM = (sdkInt == 25 && getPreviousSdkInt() != 0) || sdkInt > 25;
        if (aboveM) {
            try {
                V25.install(classLoader, folder);
            } catch (Throwable throwable) {
                try {
                    V23.install(classLoader, folder);
                } catch (Throwable throwable1) {
                    V14.install(classLoader, folder);
                }
            }
        } else if (sdkInt >= 23) {
            try {
                V23.install(classLoader, folder);
            } catch (Throwable throwable) {
                V14.install(classLoader, folder);
            }
        } else if (sdkInt >= 14) {
            V14.install(classLoader, folder);
        }
        lastSoDir = folder;
        return true;
    }

    /**
     * 把自定义的native库path插入nativeLibraryDirectories最前面，即使安装包libs目录里面有同名的so，也优先加载指定路径的外部so
     */
    private static final class V25 {
        private static void install(ClassLoader classLoader, File folder) throws Throwable {
            Field pathListField = ReflectUtil.findField(classLoader, "pathList");
            Object dexPathList = pathListField.get(classLoader);
            Field nativeLibraryDirectories = ReflectUtil.findField(dexPathList, "nativeLibraryDirectories");

            List<File> libDirs = (List<File>) nativeLibraryDirectories.get(dexPathList);
            //去重
            if (libDirs == null) {
                libDirs = new ArrayList<>(2);
            }
            final Iterator<File> libDirIt = libDirs.iterator();
            while (libDirIt.hasNext()) {
                final File libDir = libDirIt.next();
                if (folder.equals(libDir) || folder.equals(lastSoDir)) {
                    libDirIt.remove();
                    Log.debug( "[HotPatcher]dq libDirIt.remove()" + folder.getAbsolutePath());
                    break;
                }
            }

            libDirs.add(0, folder);
            //system/lib
            Field systemNativeLibraryDirectories = ReflectUtil.findField(dexPathList, "systemNativeLibraryDirectories");
            List<File> systemLibDirs = (List<File>) systemNativeLibraryDirectories.get(dexPathList);

            //判空
            if (systemLibDirs == null) {
                systemLibDirs = new ArrayList<>(2);
            }
            Log.debug( "[HotPatcher]dq systemLibDirs,size=" + systemLibDirs.size());

            Method makePathElements = ReflectUtil.findMethod(dexPathList, "makePathElements", List.class);
            libDirs.addAll(systemLibDirs);

            Object[] elements = (Object[]) makePathElements.invoke(dexPathList, libDirs);
            Field nativeLibraryPathElements = ReflectUtil.findField(dexPathList, "nativeLibraryPathElements");
            nativeLibraryPathElements.setAccessible(true);
            nativeLibraryPathElements.set(dexPathList, elements);
        }
    }

    private static final class V23 {
        private static void install(ClassLoader classLoader, File folder) throws Throwable {
            Field pathListField = ReflectUtil.findField(classLoader, "pathList");
            Object dexPathList = pathListField.get(classLoader);

            Field nativeLibraryDirectories = ReflectUtil.findField(dexPathList, "nativeLibraryDirectories");
            List<File> libDirs = (List<File>) nativeLibraryDirectories.get(dexPathList);

            //去重
            if (libDirs == null) {
                libDirs = new ArrayList<>(2);
            }
            final Iterator<File> libDirIt = libDirs.iterator();
            while (libDirIt.hasNext()) {
                final File libDir = libDirIt.next();
                if (folder.equals(libDir) || folder.equals(lastSoDir)) {
                    libDirIt.remove();
                    Log.debug( "[HotPatcher]dq libDirIt.remove() " + folder.getAbsolutePath());
                    break;
                }
            }

            libDirs.add(0, folder);
            Field systemNativeLibraryDirectories =
                    ReflectUtil.findField(dexPathList, "systemNativeLibraryDirectories");
            List<File> systemLibDirs = (List<File>) systemNativeLibraryDirectories.get(dexPathList);

            //判空
            if (systemLibDirs == null) {
                systemLibDirs = new ArrayList<>(2);
            }
            Log.debug("[HotPatcher]dq systemLibDirs,size=" + systemLibDirs.size());

            Method makePathElements = ReflectUtil.findMethod(dexPathList, "makePathElements", List.class, File.class, List.class);
            ArrayList<IOException> suppressedExceptions = new ArrayList<>();
            libDirs.addAll(systemLibDirs);

            Object[] elements = (Object[]) makePathElements.invoke(dexPathList, libDirs, null, suppressedExceptions);
            Field nativeLibraryPathElements = ReflectUtil.findField(dexPathList, "nativeLibraryPathElements");
            nativeLibraryPathElements.setAccessible(true);
            nativeLibraryPathElements.set(dexPathList, elements);
        }
    }

    private static final class V14 {
        private static void install(ClassLoader classLoader, File folder) throws Throwable {
            Field pathListField = ReflectUtil.findField(classLoader, "pathList");
            Object dexPathList = pathListField.get(classLoader);

            ReflectUtil.expandFieldArray(dexPathList, "nativeLibraryDirectories", new File[]{folder});
        }
    }

    @TargetApi(Build.VERSION_CODES.M)
    private static int getPreviousSdkInt() {
        try {
            return Build.VERSION.PREVIEW_SDK_INT;
        } catch (Throwable ignore) {
        }
        return 1;
    }

    public static boolean FileisExist(String path){
        if(!path.isEmpty()){
            File f = new File(path);
            if(f.exists())
                return true;
        }
        return false;
    }

    public static boolean DeleteFile(File file){
        if(file == null || !file.exists())
            return false;
        if(file.isDirectory()){
            for(File item:file.listFiles()){
                DeleteFile(item);
            }
        }
        file.delete();
        return true;
    }

    public static boolean DeleteFile(String path){
        File f = new File(path);
        if(f.exists())
            return DeleteFile(f);
        return false;
    }

    public static boolean CopyFile( String fileName, String toPath) {
        boolean copyIsFinish = false;
        try {
            InputStream is = new FileInputStream(fileName);
            File file = new File(toPath);
            file.createNewFile();
            FileOutputStream fos = new FileOutputStream(file);
            byte[] temp = new byte[1024];
            int i = 0;
            int nTatolLen = 0;
            while ((i = is.read(temp)) > 0) {
                fos.write(temp, 0, i);
                nTatolLen += i;
            }
            fos.close();
            is.close();
            copyIsFinish = true;
            Log.verbose("[HotPatcher]copyFile complete, len="+nTatolLen);
        } catch (IOException e) {
            e.printStackTrace();
            Log.error( "[HotPatcher]copyFileFromAssets IOException "+e.toString());
        }
        return copyIsFinish;
    }

    public static long GetFileSize(File file){
        long size = 0;
        if(file.exists()){
            try {
                FileInputStream fis = new FileInputStream(file);
                size = fis.available();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return size;
    }

    public static long GetFileSize(String filePath){
        File file = new File(filePath);
        return GetFileSize(file);
    }

    public static boolean UnZip(final String filePath,final String destDir)
    {
        ZipFile zip = null;
        try{
            zip = new ZipFile(filePath);
        }catch (IOException el){
            el.printStackTrace();
            Log.error( "[HotPatcher]Unzip "+el.toString());
            return false;
        }
        Enumeration<?> entries = zip.entries();
        byte[] buffer = new byte[1024];
        String sWriteExceptionMsg = "";
        while (entries.hasMoreElements()) {
            ZipEntry entry = (ZipEntry) entries.nextElement();
            String entryName = entry.getName();
            File file = new File(destDir + File.separator + entryName);

            if (entry.isDirectory()) {
                if (!file.exists()) {
                    file.mkdirs();
                }
            } else
            {
                if (file.getParentFile() != null && !file.getParentFile().exists()) {
                file.getParentFile().mkdirs();
            }

                InputStream is = null;
                FileOutputStream os = null;
                try {
                    is = zip.getInputStream(entry);
                    os = new FileOutputStream(file);
                    int readLen = 0;
                    while ((readLen = is.read(buffer)) > 0) {
                        os.write(buffer, 0, readLen);
                    }
                } catch (FileNotFoundException e) {
                    sWriteExceptionMsg = e.getMessage();
                    break;
                } catch (Exception e) {
                    sWriteExceptionMsg = e.getMessage();
                    break;
                } finally {
                    try {
                        if (null != os) {
                            os.close();
                        }
                        if (null != is) {
                            is.close();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                        sWriteExceptionMsg += e.getMessage();
                        break;
                    }
                }
            }
        }
        try {
            zip.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (TextUtils.isEmpty(sWriteExceptionMsg)) {
            return true;
        } else {
            Log.verbose("[java]HotPatcher error " + sWriteExceptionMsg);
            return false;
        }
    }
}
