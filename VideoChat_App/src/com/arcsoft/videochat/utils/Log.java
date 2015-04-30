package com.arcsoft.videochat.utils;


public class Log {
	
	/**
     * Priority constant for the println method; use Log.v.
     */
	private static final int LOG_LEVEL_VERBOSE = 6;

    /**
     * Priority constant for the println method; use Log.d.
     */
	private static final int LOG_LEVEL_DEBUG = 5;

    /**
     * Priority constant for the println method; use Log.i.
     */
	private static final int LOG_LEVEL_INFO = 4;

    /**
     * Priority constant for the println method; use Log.w.
     */
	private static final int LOG_LEVEL_WARN = 3;

    /**
     * Priority constant for the println method; use Log.e.
     */
	private static final int LOG_LEVEL_ERROR = 2;

	
	private static int logLevel= LOG_LEVEL_VERBOSE;
    public static int v(String tag, String msg) {
    	if (logLevel >= LOG_LEVEL_VERBOSE) return -1;
        return android.util.Log.v(tag, msg);
    }
    public static int v(String tag, String msg, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_VERBOSE) return -1;
        return android.util.Log.v(tag, msg, tr);
    }
    public static int d(String tag, String msg) {
    	if (logLevel >= LOG_LEVEL_DEBUG) return -1;
        return android.util.Log.d(tag, msg);
    }
    public static int d(String tag, String msg, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_DEBUG) return -1;
        return android.util.Log.d(tag, msg, tr);
    }
    public static int i(String tag, String msg) {
    	if (logLevel >= LOG_LEVEL_INFO) return -1;
        return android.util.Log.i(tag, msg);
    }
    public static int i(String tag, String msg, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_INFO) return -1;
        return android.util.Log.i(tag, msg, tr);
    }
    public static int w(String tag, String msg) {
    	if (logLevel >= LOG_LEVEL_WARN) return -1;
        return android.util.Log.w(tag, msg);
    }
    public static int w(String tag, String msg, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_WARN) return -1;
        return android.util.Log.w(tag, msg, tr);
    }
    public static int w(String tag, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_WARN) return -1;
    	return android.util.Log.w(tag, tr);
    }
    public static int e(String tag, String msg) {
    	if (logLevel >= LOG_LEVEL_ERROR) return -1;
    	return android.util.Log.e(tag, msg);
    }
    public static int e(String tag, String msg, Throwable tr) {
    	if (logLevel >= LOG_LEVEL_ERROR) return -1;
    	return android.util.Log.e(tag, msg, tr);
    }
}