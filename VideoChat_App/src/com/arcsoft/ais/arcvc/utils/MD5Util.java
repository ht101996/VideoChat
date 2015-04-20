package com.arcsoft.ais.arcvc.utils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;


public class MD5Util {
protected static char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	                                  'a', 'b', 'c', 'd', 'e', 'f' };
protected static MessageDigest messagedigest = null;
static{
   try{
    messagedigest = MessageDigest.getInstance("MD5");
   }catch(NoSuchAlgorithmException e){
    System.err.println(MD5Util.class.getName()+"初始化失败，MessageDigest不支持MD5Util。");
//	LOG.e(e.getMessage(), e);
   }
}

public static void main(String[] args) throws IOException {
   long begin = System.currentTimeMillis();

  // File big = new File("D:/testdata/北盟外语(www.bmwy.net)-忍者神龟.3gp");
   //String md5=getFileMD5String(big);
   String md5=getMD5String("000000");
   long end = System.currentTimeMillis();
//   LOG.info("md5:"+md5+" time:"+((end-begin))+"ms");
}

/**
* 
* @param file
* @return
* @throws IOException
*/
public static String getFileMD5String(File file) {
	FileInputStream in = null;
	try {
		in = new FileInputStream(file);
	} catch (FileNotFoundException e) {
//		LOG.error(e.getMessage(), e);
	}
	FileChannel ch = in.getChannel();
	MappedByteBuffer byteBuffer = null;
	try {
		byteBuffer = ch
				.map(FileChannel.MapMode.READ_ONLY, 0, file.length());
	} catch (IOException e) {
//		LOG.error(e.getMessage(), e);
	}
	messagedigest.update(byteBuffer);
	return bufferToHex(messagedigest.digest());
}

public static String getMD5String(String s) {
   return getMD5String(s.getBytes());
}

public static String getMD5String(byte[] bytes) {
   messagedigest.update(bytes);
   return bufferToHex(messagedigest.digest());
}

private static String bufferToHex(byte bytes[]) {
   return bufferToHex(bytes, 0, bytes.length);
}

private static String bufferToHex(byte bytes[], int m, int n) {
   StringBuffer stringbuffer = new StringBuffer(2 * n);
   int k = m + n;
   for (int l = m; l < k; l++) {
    appendHexPair(bytes[l], stringbuffer);
   }
   return stringbuffer.toString();
}

private static void appendHexPair(byte bt, StringBuffer stringbuffer) {
   char c0 = hexDigits[(bt & 0xf0) >> 4];
   char c1 = hexDigits[bt & 0xf];
   stringbuffer.append(c0);
   stringbuffer.append(c1);
}

public static boolean checkPassword(String password, String md5PwdStr) {
   String s = getMD5String(password);
   return s.equals(md5PwdStr);
}
}