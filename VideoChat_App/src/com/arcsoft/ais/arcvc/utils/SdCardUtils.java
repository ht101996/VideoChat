package com.arcsoft.ais.arcvc.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

public class SdCardUtils {

	public static final String PACK_NAME = "/Android/data/com.arcsoft.ais.arcvc";
	public static final String APP_DATA_PATH = getSdCardPath() + PACK_NAME;
	public static final String VC_CONFIG = APP_DATA_PATH + "/vc.xml";

	public static String getSdCardPath() {
		return Environment.getExternalStorageDirectory().getAbsolutePath();
	}

	public static Document getVcConfig() {
		try {
			File file = new File(VC_CONFIG);
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			DocumentBuilder builder = factory.newDocumentBuilder();
			if (!file.exists()) {
				return builder.newDocument();
			} else {
				return builder.parse(file);
			}
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	public static synchronized void writeVcConfig(Document document) {
		try {
			File file = new File(VC_CONFIG);
			if (file.exists())
				file.delete();
			FileOutputStream fos = new FileOutputStream(VC_CONFIG);
			TransformerFactory tsf = TransformerFactory.newInstance();
			DOMSource source = new DOMSource(document);
			Transformer tf = tsf.newTransformer();
			tf.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
			tf.setOutputProperty(OutputKeys.INDENT, "  ");
			StreamResult result = new StreamResult(fos);
			tf.transform(source, result);
			fos.close();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static boolean vcConfigExists() {
		File file = new File(VC_CONFIG);
		return file.exists();
	}

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
	
	private static String readFileByLines(File file) {
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
}
