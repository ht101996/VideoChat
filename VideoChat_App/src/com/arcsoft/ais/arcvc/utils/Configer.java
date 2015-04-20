package com.arcsoft.ais.arcvc.utils;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import com.arcsoft.coreapi.sdk.CoreVCAPI;

import android.content.res.AssetManager;
import android.util.Log;

public class Configer {

	private static Properties properties = new Properties();

	static {
		System.loadLibrary("p2pcrypto");
		System.loadLibrary("p2pssl");
		System.loadLibrary("corevcsdk");
		System.loadLibrary("corevcsdkwrapper");
	}

	public static void init(AssetManager assetManager) {
		InputStream in = null;
		try {
			in = assetManager.open("config.ini");
			properties.load(in);
		} catch (Exception e1) {
			e1.printStackTrace();
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	public static String getValue(String key) {
		String value = properties.getProperty(key);
		if ("".equals(value) || value == null) {
			Log.e(Global.TAG, "there is no value of this key: " + key);
		}

//		Log.i(Global.TAG, "getValue key: " + key);
//		Log.i(Global.TAG, "getValue value: " + value);
//		Log.i(Global.TAG, "getValue properties: " + properties);
		return value;
	}

	public static String setValue(String key, String value) {
//		if ("".equals(key) || key == null) {
//			Log.i(Global.TAG, "this key is null or blank!");
//		}
		if ("".equals(value) || value == null) {
			Log.e(Global.TAG, "there is no value of this key: " + key);
		}
		properties.put(key, value);
//		Log.i(Global.TAG, "setValue key: " + key);
//		Log.i(Global.TAG, "setValue value: " + value);
//		Log.i(Global.TAG, "setValue properties: " + properties);
		return value;
	}
}
