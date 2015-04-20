package com.arcsoft.ais.arcvc.service;

import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;

import com.arcsoft.ais.arcvc.activity.VideoActivity;

import android.hardware.Camera;
import android.media.MediaRecorder;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.view.View;

public class VideoActivityService {
	
	public static Camera getFrontCamera() {
		Camera camera = Camera.open();
		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
		int cameraCount = Camera.getNumberOfCameras(); // get cameras number
		for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
			Camera.getCameraInfo(camIdx, cameraInfo); // get camerainfo
			if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
				camera = Camera.open(camIdx);
				break;
			}
		}
		return camera;
	}
	
	public static void startCameraStreaming(VideoActivity va) throws IOException {
		System.out.println("-----------------startCameraStreaming");
//		H264Nal nalu = new H264Nal();
//		byte[] buffer = new byte[65535 + 80];
//		InputStream fin = new FileInputStream("/storage/emulated/0/Android/data/com.arcsoft.ais.arcvc/remtoe.h264");
//		int dataPointer;
//		// Find 1st NAL
//		int[] cacheRead = new int[4];
//		cacheRead[0] = fin.read();
//		cacheRead[1] = fin.read();
//		cacheRead[2] = fin.read();
//		cacheRead[3] = fin.read();
//		
//		while(!(
//				cacheRead[0] == 0x00 &&
//				cacheRead[1] == 0x00 &&
//				cacheRead[2] == 0x00 &&
//				cacheRead[3] == 0x01 
//				)) {
//			 cacheRead[0] = cacheRead[1];
//			 cacheRead[1] = cacheRead[2];
//			 cacheRead[2] = cacheRead[3];
//			 cacheRead[3] = fin.read();
//		} // while
//    	
//		int i = 1;
//		boolean hasMoreNAL = true;
//		while (hasMoreNAL) {
//			dataPointer = 4;
//			// Find next NAL
//			cacheRead[0] = fin.read();
//			if(cacheRead[0]==-1) hasMoreNAL = false;
//			cacheRead[1] = fin.read();
//			if(cacheRead[1]==-1) hasMoreNAL = false;
//			cacheRead[2] = fin.read();
//			if(cacheRead[2]==-1) hasMoreNAL = false;
//			cacheRead[3] = fin.read();
//			if(cacheRead[3]==-1) hasMoreNAL = false;
//			while (!(cacheRead[0] == 0x00 && cacheRead[1] == 0x00 && cacheRead[2] == 0x00 && cacheRead[3] == 0x01) && hasMoreNAL) {
//				buffer[dataPointer++] = (byte) cacheRead[0];
//				cacheRead[0] = cacheRead[1];
//				cacheRead[1] = cacheRead[2];
//				cacheRead[2] = cacheRead[3];
//				cacheRead[3] = fin.read();
//				if (cacheRead[3] == -1)
//					hasMoreNAL = false;
//			} // while
//			byte[] naluData = Arrays.copyOfRange(buffer, 4, dataPointer);
//			nalu.setType(naluData[0] & 0x1f);
//			nalu.setRefIdc((naluData[0] & 0x60) >>> 5);
//			nalu.setPayload(naluData);
//			nalu.setPayloadLength(naluData.length);
//			p2p.send264Packet("", nalu);
//			i++;
//		}
//		fin.close();
		
	}
	
}
