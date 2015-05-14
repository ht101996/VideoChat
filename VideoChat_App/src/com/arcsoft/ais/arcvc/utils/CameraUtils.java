package com.arcsoft.ais.arcvc.utils;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import android.app.Activity;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.media.MediaRecorder;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;

import com.arcsoft.ais.arcvc.bean.VideoConfig;
import com.arcsoft.ais.arcvc.jni.AACNal;
import com.arcsoft.ais.arcvc.jni.H264Nal;

public class CameraUtils {

	private static final String TAG =  "CameraUtils";
	private static final Map<String, VideoConfig> videoConfigs = new HashMap<String, VideoConfig>();
	private static Camera frontCamera ;
	private static List<Size> supportedPreviewSizes;
	private static String bestResolution;
	private static int cameraIndex = 0;
	private static boolean usingCam = false;
	private static long timeuse = 0;
	
//	public static int searchCameraIndex(){
//		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
//		int cameraCount = Camera.getNumberOfCameras(); // get cameras number
//		for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
//			Camera.getCameraInfo(camIdx, cameraInfo); // get camerainfo
//			if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
//				setCameraIndex(cameraIndex);
//				break;
//			}
//		}
//		Log.i(Global.TAG, "CameraUtils ----cameraIndex---------!"+-cameraIndex);
//		return cameraIndex;
//	}
	public static Camera getNewCameraInstance() {
		Log.i(Global.TAG, "CameraUtils ----_getFrontCamera----------Start!");
		Camera camera = null;
//		searchCameraIndex();
		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
		int cameraCount = Camera.getNumberOfCameras(); // get cameras number
		for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
			Camera.getCameraInfo(camIdx, cameraInfo); // get camerainfo
			if (cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
				camera = Camera.open(camIdx);
				cameraIndex = camIdx ;
				break;
			}
		}
		Log.i(Global.TAG, "CameraUtils ----getCameraIndex()---------=="+getCameraIndex());
//		camera = Camera.open(getCameraIndex());
		Parameters parameters = camera.getParameters();
		supportedPreviewSizes = parameters.getSupportedPreviewSizes();
		if (!SdCardUtils.vcConfigExists())
			initVcConfig(supportedPreviewSizes);
		bestResolution = getBestResolution();
		loadVideoConfigs();
		frontCamera = camera;
		Log.i(Global.TAG, "CameraUtils ----_getFrontCamera----------end!");
		return camera;
	}
	
	private static void loadVideoConfigs() {
		Log.i(Global.TAG, "CameraUtils ----loadVideoConfigs----------start!");
		Document doc = SdCardUtils.getVcConfig();
		Element root = (Element) doc.getDocumentElement().getFirstChild();
		NodeList list = root.getElementsByTagName("Resolution");
		videoConfigs.clear();
		for (int i = 0; i < list.getLength(); i++) {
			Element e = (Element) list.item(i);
			VideoConfig vc = new VideoConfig();
			if (e.getAttribute("sps") != null && e.getAttribute("sps").length() > 0 && e.getAttribute("pps") != null && e.getAttribute("pps").length() > 0) {
				String size = e.getAttribute("size");
				String[] tmp = size.split("x");
				vc.setPps(hexStringToBytes(e.getAttribute("pps")));
				vc.setSps(hexStringToBytes(e.getAttribute("sps")));
				vc.setWidth(Integer.parseInt(tmp[0]));
				vc.setHeight(Integer.parseInt(tmp[1]));
				videoConfigs.put(size, vc);
			}
		}
		//Log.i(Global.TAG, "CameraUtils ----loadVideoConfigs----------end!");
	}

	public static Camera getFrontCamera() {
		return frontCamera;
	}

	public static List<Size> getSupportedPreviewSizes() {
		return supportedPreviewSizes;
	}

	public static void initVcConfig(List<Size> list) {
		Document document = SdCardUtils.getVcConfig();
		Element root = document.createElement("Android");
		Element cameras = document.createElement("Cameras");
		for (Size size : list) {
			Element previewSize = document.createElement("Resolution");
			previewSize.setAttribute("size", size.width + "x" + size.height);
			cameras.appendChild(previewSize);
		}
		root.appendChild(cameras);
		document.appendChild(root);
		SdCardUtils.writeVcConfig(document);
	}

	public static String getBestResolution() {
		int width = 0;
		int height = 0;
		for (int i = 0; i < supportedPreviewSizes.size(); i++) {
			Size size = supportedPreviewSizes.get(i);
			if (size.width == 320) {//320
				width = size.width;
				height = size.height;
				break;
			}
			
		}
//		Size size = supportedPreviewSizes.get(supportedPreviewSizes.size() - 1);
//		width = size.width;
//		height = size.height;
		return width + "x" + height;
	}

	public static void getSPSPPS(SurfaceView sv) throws IOException {
		long timeStart = System.currentTimeMillis();
		MediaRecorder mMediaRecorder = new MediaRecorder();
		frontCamera.unlock();
		mMediaRecorder.setCamera(frontCamera);
		mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
		mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
		mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
		mMediaRecorder.setVideoFrameRate(15);
		String[] tmp = bestResolution.split("x");
		mMediaRecorder.setVideoSize(Integer.parseInt(tmp[0]), Integer.parseInt(tmp[1]));
//		SurfaceHolder surfaceHolder = sv.getHolder() ;
//		surfaceHolder.addCallback(callback);
		mMediaRecorder.setPreviewDisplay(sv.getHolder().getSurface());
		mMediaRecorder.setMaxDuration(1000);
		mMediaRecorder.setMaxFileSize(0);
		// mMediaRecorder.setOutputFile(sender.getFileDescriptor());
		mMediaRecorder.setOutputFile(SdCardUtils.APP_DATA_PATH + "/sample.vc");
		try {
			mMediaRecorder.prepare();
			mMediaRecorder.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		try {
			Thread.sleep(1500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		mMediaRecorder.stop();
		mMediaRecorder.reset();
		mMediaRecorder.release();
		
		Map<String, byte[]> map = getSPSAndPPS(SdCardUtils.APP_DATA_PATH + "/sample.vc");
		File file = new File(SdCardUtils.APP_DATA_PATH + "/sample.vc");
		file.delete();
		Document doc = SdCardUtils.getVcConfig();
		Element root = (Element) doc.getDocumentElement().getFirstChild();
		NodeList list = root.getElementsByTagName("Resolution");
		for (int i = 0; i < list.getLength(); i++) {
			Element e = (Element) list.item(i);
			VideoConfig vc = new VideoConfig();
			if (e.getAttribute("size").equals(bestResolution)) {
				String size = e.getAttribute("size");
				tmp = size.split("x");
				vc.setPps(map.get("pps"));
				vc.setSps(map.get("sps"));
				vc.setWidth(Integer.parseInt(tmp[0]));
				vc.setHeight(Integer.parseInt(tmp[1]));
				videoConfigs.put(size, vc);
				e.setAttribute("sps", bytesToHexString(map.get("sps")));
				e.setAttribute("pps", bytesToHexString(map.get("pps")));
				break;
			}
		}
		SdCardUtils.writeVcConfig(doc);
		Log.v(TAG, "getSPSPPS cost time:"+(System.currentTimeMillis() - timeStart));
	}

	private static Map<String, byte[]> getSPSAndPPS(String fileName) throws IOException {
		Map<String, byte[]> map = new HashMap<String, byte[]>();
		File file = new File(fileName);
		FileInputStream fis = new FileInputStream(file);

		int fileLength = (int) file.length();
		byte[] fileData = new byte[fileLength];
		fis.read(fileData);
		
		fis.close();

		// 'a'=0x61, 'v'=0x76, 'c'=0x63, 'C'=0x43
		byte[] avcC = new byte[] { 0x61, 0x76, 0x63, 0x43 };

		// avcC的起始位置
		int avcRecord = 0;
		for (int ix = 0; ix < fileLength; ++ix) {
			if (fileData[ix] == avcC[0] && fileData[ix + 1] == avcC[1] && fileData[ix + 2] == avcC[2] && fileData[ix + 3] == avcC[3]) {
				// 找到avcC，则记录avcRecord起始位置，然后退出循环。
				avcRecord = ix + 4;
				break;
			}
		}
		if (0 == avcRecord) {
			System.out.println("没有找到avcC，请检查文件格式是否正确");
			return null;
		}

		// 加7的目的是为了跳过
		// (1)8字节的 configurationVersion
		// (2)8字节的 AVCProfileIndication
		// (3)8字节的 profile_compatibility
		// (4)8 字节的 AVCLevelIndication
		// (5)6 bit 的 reserved
		// (6)2 bit 的 lengthSizeMinusOne
		// (7)3 bit 的 reserved
		// (8)5 bit 的numOfSequenceParameterSets
		// 共6个字节，然后到达sequenceParameterSetLength的位置
		int spsStartPos = avcRecord + 6;
		byte[] spsbt = new byte[] { fileData[spsStartPos], fileData[spsStartPos + 1] };
		int spsLength = bytes2Int(spsbt);
		byte[] SPS = new byte[spsLength];
		// 跳过2个字节的 sequenceParameterSetLength
		spsStartPos += 2;
		System.arraycopy(fileData, spsStartPos, SPS, 0, spsLength);
		map.put("sps", SPS);
		// 底下部分为获取PPS
		// spsStartPos + spsLength 可以跳到pps位置
		// 再加1的目的是跳过1字节的 numOfPictureParameterSets
		int ppsStartPos = spsStartPos + spsLength + 1;
		byte[] ppsbt = new byte[] { fileData[ppsStartPos], fileData[ppsStartPos + 1] };
		int ppsLength = bytes2Int(ppsbt);
		byte[] PPS = new byte[ppsLength];
		ppsStartPos += 2;
		System.arraycopy(fileData, ppsStartPos, PPS, 0, ppsLength);
		map.put("pps", PPS);
		return map;
	}

	private static int bytes2Int(byte[] bt) {
		int ret = bt[0];
		ret <<= 8;
		ret |= bt[1];
		return ret;
	}
	
	public static String bytesToHexString(byte[] src) {

		StringBuilder stringBuilder = new StringBuilder("");
		if (src == null || src.length <= 0) {
			return null;
		}
		for (int i = 0; i < src.length; i++) {
			int v = src[i] & 0xFF;
			String hv = Integer.toHexString(v);
			if (hv.length() < 2) {
				stringBuilder.append(0);
			}
			stringBuilder.append(hv);
		}
		return stringBuilder.toString();
	}

	public static byte[] hexStringToBytes(String hexString) {

		if (hexString == null || hexString.equals("")) {
			return null;
		}
		hexString = hexString.toUpperCase();
		int length = hexString.length() / 2;
		char[] hexChars = hexString.toCharArray();
		byte[] d = new byte[length];
		for (int i = 0; i < length; i++) {
			int pos = i * 2;
			d[i] = (byte) (charToByte(hexChars[pos]) << 4 | charToByte(hexChars[pos + 1]));
		}
		return d;
	}

	private static byte charToByte(char c) {
		return (byte) "0123456789ABCDEF".indexOf(c);
	}
	
	public static VideoConfig getVideoConfig() {
		return videoConfigs.get(bestResolution);
	}

	public static void startVideoRecording() {
		//Log.i(Global.TAG, "startVideoRecording====" + "starting....");
		(new Thread() {
			public void run() {
				VideoConfig vc = CameraUtils.getVideoConfig();
				if (vc == null) {
					//Log.i(Global.TAG, "startVideoRecording====" + "VideoConfig vc == null ");
					return;
				}
				byte[] h264sps = vc.getSps();
				byte[] h264pps = vc.getPps();
				byte[] buffer = new byte[65535];
				InputStream fis = null;
				try {
					fis = SocketUtils.getVideoReceiverSocket().getInputStream();
				} catch (IOException e) {
					e.printStackTrace();
					return;
				}
				DataInputStream dis = new DataInputStream(fis);
				H264Nal nalu = new H264Nal();
				
				try {
					// 要跳过前面的字节，到mdat为止
					byte[] mdat = new byte[] { 0x6d, 0x64, 0x61, 0x74 };
					byte[] buff = new byte[4];
					while (true) {
						buff[0] = buff[1];
						buff[1] = buff[2];
						buff[2] = buff[3];
						buff[3] = dis.readByte();
						if (Arrays.equals(mdat, buff))
							break;
					}
					nalu.setType(h264sps[0] & 0x1f);
					nalu.setRefIdc((h264sps[0] & 0x60) >>> 5);
					nalu.setPayload(h264sps);
					nalu.setPayloadLength(h264sps.length);
					P2PClientManager.getP2PClientInstance().send264Packet2("sps", nalu);
//					Log.d(Global.TAG, "send264Packet, naluData.length==" + nalu.getPayloadLength());
					nalu.setType(h264pps[0] & 0x1f);
					nalu.setRefIdc((h264pps[0] & 0x60) >>> 5);
					nalu.setPayload(h264pps);
					nalu.setPayloadLength(h264pps.length);
					P2PClientManager.getP2PClientInstance().send264Packet2("pps", nalu);
//					P2PClientManager.getP2PClientInstance().sendAACPacket(gpid, nalu)
//					Log.d(Global.TAG, "send264Packet, naluData.length==" + nalu.getPayloadLength());
				} catch (Exception e) {
					String cause = e.getMessage();
					cause = e.getCause().toString();
					e.printStackTrace();
					return;
				}
				
				
				while (true) {
					//Log.i(Global.TAG, "recording====" + "loop starting....");
					//Log.i(Global.TAG, "recording====" + "loop usingCam...." + usingCam);
					//Log.i(Global.TAG, "recording====" + "loop receiver...." + SocketUtils.getReceiverSocket());
					if (!usingCam || SocketUtils.getVideoReceiverSocket() == null)
						break;
					try {
						//Log.i(Global.TAG, "recording====" + "loop dis.available()...." + dis.available());
						//Log.i(Global.TAG, "recording====" + "loop buffer.length...." + buffer.length);
						int h264length = dis.readInt();
						//Log.i(Global.TAG, "recording====" + "loop h264length==" + h264length);
						if (h264length > buffer.length) {
//							Log.e(Global.TAG, "recording====h264length: " + h264length + ", loop h264length > buffer.length");
							while (dis.available() > 0) {
								if (dis.available() > buffer.length) {
									dis.read(buffer, 0, buffer.length);
								} else {
									dis.read(buffer, 0, dis.available());
								}
							}
							continue;
						}
						long start = System.currentTimeMillis();
						dis.read(buffer, 0, h264length);
						long end = System.currentTimeMillis();
						timeuse += end - start;
						Log.i(Global.TAG, "video encode cost time: " + timeuse);
						byte[] naluData = Arrays.copyOfRange(buffer, 0, h264length);
						if (naluData.length <= 0) {
//							Log.e(Global.TAG, "recording====" + "loop naluData.length==" + naluData.length);
							continue ;
						}

						if ((naluData[0] & 0x1f) == 5) { // IDR
							nalu.setType(h264sps[0] & 0x1f);
							nalu.setRefIdc((h264sps[0] & 0x60) >>> 5);
							nalu.setPayload(h264sps);
							nalu.setPayloadLength(h264sps.length);
							P2PClientManager.getP2PClientInstance().send264Packet2("sps", nalu);
							Log.d(Global.TAG, "send264Packet, naluData.length==" + nalu.getPayloadLength());
							nalu.setType(h264pps[0] & 0x1f);
							nalu.setRefIdc((h264pps[0] & 0x60) >>> 5);
							nalu.setPayload(h264pps);
							nalu.setPayloadLength(h264pps.length);
							P2PClientManager.getP2PClientInstance().send264Packet2("pps", nalu);
						}
						nalu.setType(naluData[0] & 0x1f);
						nalu.setRefIdc((naluData[0] & 0x60) >>> 5);
						nalu.setPayload(naluData);
						nalu.setPayloadLength(naluData.length);
						P2PClientManager.getP2PClientInstance().send264Packet2("img", nalu);
//						Log.d(Global.TAG, "send264Packet, naluData.length==" + nalu.getPayloadLength());
					} catch (Exception e) {
						e.printStackTrace();
//						break;
					}
				}
				Log.d(Global.TAG, "recording====" + "loop ended....");
			}
		}).start();
	}

    public static int getCameraDisplayOrientationDegrees(Activity activity,
             android.hardware.Camera camera) {
        android.hardware.Camera.CameraInfo info =
                new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(getCameraIndex(), info);
        int rotation = activity.getWindowManager().getDefaultDisplay()
                .getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0: degrees = 0; break;
            case Surface.ROTATION_90: degrees = 90; break;
            case Surface.ROTATION_180: degrees = 180; break;
            case Surface.ROTATION_270: degrees = 270; break;
        }
    
        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        return result ;
    }
	public static void startAudioRecording() {
		//Log.i(Global.TAG, "startVideoRecording====" + "starting....");
		(new Thread() {
			public void run() {
				//Log.i(Global.TAG, "startVideoRecording====" + "VideoConfig vc == null ");
				byte[] buffer = new byte[65535];
				InputStream fis = null;
				try {
					fis = SocketUtils.getAudioReceiverSocket().getInputStream();
				} catch (IOException e) {
					e.printStackTrace();
					return;
				}
				DataInputStream dis = new DataInputStream(fis);
				AACNal nalu = new AACNal();
				
				while (true) {
//					Log.i(Global.TAG, "recording====" + "audio loop starting....");
//					Log.i(Global.TAG, "recording====" + "audio loop usingCam...." + usingCam);
//					Log.i(Global.TAG, "recording====" + "audio loop receiver...." + SocketUtils.getAudioReceiverSocket());
					if (!usingCam || SocketUtils.getAudioReceiverSocket() == null)
						break;
					try {
//						int aaclength = dis.readInt();
						int aaclength = dis.available();
//						Log.i(Global.TAG, "recording====" + "audio loop aaclength==" + aaclength);
						if (aaclength <= 0) {
							continue;
						}
						Log.i(Global.TAG, "recording====" + "audio loop buffer.length==" + buffer.length);
						Log.i(Global.TAG, "recording====" + "audio loop dis.available()==" + dis.available());
					
						if (aaclength > buffer.length) {
							Log.e(Global.TAG, "audio loop recording====aaclength: " + aaclength + ", loop aaclength > buffer.length");
							while (dis.available() > 0) {
								if (dis.available() > buffer.length) {
									dis.read(buffer, 0, buffer.length);
								} else {
									dis.read(buffer, 0, dis.available());
								}
							}
							//continue;
						}
						long start = System.currentTimeMillis();
						dis.read(buffer, 0, aaclength);
						long end = System.currentTimeMillis();
						timeuse += end - start;
						Log.i(Global.TAG, "audio encode cost time: " + timeuse);
						byte[] naluData = Arrays.copyOfRange(buffer, 0, aaclength);
						if (naluData.length <= 0) {
//							Log.e(Global.TAG, "recording====" + "loop naluData.length==" + naluData.length);
							continue ;
						}
				
						nalu.setPayload(naluData);
						nalu.setPayloadLength(naluData.length);
						P2PClientManager.getP2PClientInstance().sendAACPacket("", nalu);
//						Log.d(Global.TAG, "sendAACPacket, naluData.length==" + nalu.getPayloadLength());
					} catch (Exception e) {
						e.printStackTrace();
//						break;
					}
				}
				Log.d(Global.TAG, "audio recording====" + "loop ended....");
			}
		}).start();
	}

	/**
	 * 
	 * @param surfaceView_myself
	 * @param mMediaRecorder 视频采集
	 * @param aMediaRecorder 音频采集
	 * @param myCamera
	 * @return
	 */

	public static MediaRecorder initMediaRecorder(SurfaceView surfaceView_myself,
			MediaRecorder mMediaRecorder,Camera myCamera) {
		//FOR VIDEO	
		if (mMediaRecorder == null) {
			mMediaRecorder = new MediaRecorder();
		} else {
			mMediaRecorder.reset();
		}
		//FOR AUDIO
//		if(aMediaRecorder==null){
//			aMediaRecorder = new MediaRecorder();
//		} else {
//			aMediaRecorder.reset();
//		}
		
		mMediaRecorder.setCamera(myCamera);
		mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
		mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
		mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
		mMediaRecorder.setVideoFrameRate(30);//15 25
		String[] tmp = CameraUtils.bestResolution.split("x");
		mMediaRecorder.setVideoSize(Integer.parseInt(tmp[0]), Integer.parseInt(tmp[1]));
		mMediaRecorder.setPreviewDisplay(surfaceView_myself.getHolder().getSurface());
		mMediaRecorder.setMaxDuration(0);
		mMediaRecorder.setMaxFileSize(0);
		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getReceiverSocket()==" + SocketUtils.getVideoReceiverSocket());
		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getSenderSocket()==" + SocketUtils.getVideoSenderSocket());
		mMediaRecorder.setOutputFile(SocketUtils.getVideoSenderSocket().getFileDescriptor());
//		mMediaRecorder.setOutputFile("/sdcard/out.h264");
		return mMediaRecorder;
	}

	/**
	 * 
	 * @param surfaceView_myself
	 * @param mMediaRecorder 视频采集
	 * @param aMediaRecorder 音频采集
	 * @param myCamera
	 * @return
	 */

	public static MediaRecorder initAudioRecorder(MediaRecorder mMediaRecorder) {
		//FOR VIDEO	
		if (mMediaRecorder == null) {
			mMediaRecorder = new MediaRecorder();
		} else {
			mMediaRecorder.reset();
		}
		
//		ContentValues values=new ContentValues(3);
//		values.put(MediaStore.MediaColumns.TITLE,"aa");
//		values.put(MediaStore.MediaColumns.DATE_ADDED,System.currentTimeMillis());


		mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
		mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
		mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
		mMediaRecorder.setAudioChannels(1);
		mMediaRecorder.setAudioEncodingBitRate(128000);
		mMediaRecorder.setAudioSamplingRate(8000);//8000  44100

		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getReceiverSocket()==" + SocketUtils.getAudioSenderSocket());
		Log.i(Global.TAG, " --------------initMediaRecorder !SocketUtils.getSenderSocket()==" + SocketUtils.getAudioSenderSocket());
		mMediaRecorder.setOutputFile(SocketUtils.getAudioSenderSocket().getFileDescriptor());
//		mMediaRecorder.setOutputFile("/sdcard/test2.aac");
		return mMediaRecorder;
	}

	
	public static boolean isUsingCam() {
		return usingCam;
	}

	public static void setUsingCam(boolean usingCamParam) {
		usingCam = usingCamParam;
	}

	public static int getCameraIndex() {
		return cameraIndex;
	}

	public static void setCameraIndex(int cameraIndex) {
		CameraUtils.cameraIndex = cameraIndex;
	}

}
