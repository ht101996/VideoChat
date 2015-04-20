package com.arcsoft.ais.arcvc.service;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.arcsoft.ais.arcvc.utils.NetworkUtils;
import com.arcsoft.ais.arcvc.utils.Configer;
import com.arcsoft.ais.arcvc.utils.Global;
import com.arcsoft.ais.arcvc.utils.SdCardUtils;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.IBinder;
import android.util.Log;

public class BootupService extends Service {

	public static String initIniFile(AssetManager assetManager, String sdCardPath, Context context) throws IOException {
		String gpid;
		String macAddress;
		// write gpid to this gpid.ini file
		File dataDirPath = new File(SdCardUtils.APP_DATA_PATH);
		if (!dataDirPath.exists()) {
			dataDirPath.mkdirs();
		}
		if (!dataDirPath.exists()){
			Log.i(Global.TAG, "create dir error!dataDirPath:" + dataDirPath);
		}
		
		File initFile = new File(SdCardUtils.APP_DATA_PATH, "deviceId.ini");
		if (!initFile.exists()) {
			macAddress = NetworkUtils.getLocalMacAddressFromIp(context);
			Log.i(Global.TAG, "macAddress:" + macAddress);
			writeFileFromString(macAddress, initFile);
			Log.i(Global.TAG, "macAddress:" + macAddress);
		} else {
			macAddress = readFileByLines(initFile);
		}
		Configer.setValue("device_id", macAddress);

		initFile = new File(SdCardUtils.APP_DATA_PATH, "gpid.ini");
		if (!initFile.exists()) {
			// gpid = UUID.randomUUID().toString().replace("-", "")+"ffffffff";
			gpid = macAddress + macAddress + macAddress + "ffff";
			Log.i(Global.TAG, "gpid:" + gpid);
			writeFileFromString(gpid, initFile);
		} else {
			gpid = readFileByLines(initFile);
		}
		Configer.setValue("peer_id", gpid);
		// writeFileFromAssert(assetManager.open("gpid.ini"), initFile);
		initFile = new File(SdCardUtils.APP_DATA_PATH, "ConnectManager.ini");
		if (!initFile.exists()) {
			writeFileFromAssert(assetManager.open("ConnectManager.ini"), initFile);
		}

		initFile = new File(SdCardUtils.APP_DATA_PATH, "msgc.ini");
		if (!initFile.exists()) {
			writeFileFromAssert(assetManager.open("msgc.ini"), initFile);
		}
		return gpid;
	}

	public static String readFileByLines(File file) {
		String tempString = null;
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(new FileReader(file));
			// 读一行
			tempString = reader.readLine();
			Log.i(Global.TAG, "tempString : " + tempString);
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if (reader != null) {
				try {
					reader.close();
				} catch (IOException e1) {
				}
			}
		}
		return tempString;
	}

	private static void writeFileFromAssert(InputStream is, File file) throws IOException {
		OutputStream os = new FileOutputStream(file);
		byte[] data = new byte[is.available()];
		is.read(data);
		os.write(data);
		is.close();
		os.close();
	}

	private static void writeFileFromString(String string, File file) throws IOException {
		OutputStream os = new FileOutputStream(file);
		os.write(string.getBytes());
		os.close();
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

}
