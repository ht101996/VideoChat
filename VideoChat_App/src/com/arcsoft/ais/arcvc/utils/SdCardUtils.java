package com.arcsoft.ais.arcvc.utils;

import java.io.File;
import java.io.FileOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;

import android.os.Environment;

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

}
