package com.arcsoft.videochat.codec;

import java.io.File;
import java.io.FileOutputStream;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.util.Arrays;

import com.arcsoft.ais.arcvc.jni.H264Nal;
import com.arcsoft.ais.arcvc.utils.P2PClientManager;
import com.es.app.videochat.recorder.ESVideoEncoder;
import com.es.app.videochat.recorder.ESVideoQuality;
import com.es.app.videochat.recorder.MediaCodecUtils;
import com.es.app.videochat.recorder.TimeMonitor;
import com.es.app.videochat.recorder.VideoFrameItem;
import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;

import android.graphics.ImageFormat;
import android.media.MediaCodec;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Environment;
import android.util.Log;

public final class ESNativeH264Encoder implements Runnable {
	private final String tag = ESNativeH264Encoder.class.getSimpleName();
	
	private EncoderListener encoderListener;
	
	private final byte[] spsHeader = new byte[] {0x00, 0x00, 0x00, 0x01, 0x67};
	private final byte[] ppsHeader = new byte[] {0x00, 0x00, 0x00, 0x01, 0x68};
	private MediaCodec mediaCodec = null;  
	private MediaFormat mediaFormat = null;
	
    byte[] m_info = null;
    
    ESVideoQuality videoQuality = null;
    
    private BufferInfo bufferInfo = new BufferInfo();
    
    private byte[] inputFrame = null;
//    private byte[] outputH264Frame = null;
    
    private volatile Object mutex = new Object();;
    
    private Thread encodeThread = null;
    private boolean bThreadStop = false;

    private int colorFormat = 0;
    
    private int inputDataImageFormat;//only support ImageFormat.NV21, ImageFormat.YV12.
    
    public static byte[] YV12toYUV420PackedSemiPlanar(final byte[] input, final byte[] output, final int width, final int height) {
        /* 
         * COLOR_TI_FormatYUV420PackedSemiPlanar is NV12
         * We convert by putting the corresponding U and V bytes together (interleaved).
         */
        final int frameSize = width * height;
        final int qFrameSize = frameSize/4;

        System.arraycopy(input, 0, output, 0, frameSize); // Y

        for (int i = 0; i < qFrameSize; i++) {
            output[frameSize + i*2] = input[frameSize + i + qFrameSize]; // Cb (U)
            output[frameSize + i*2 + 1] = input[frameSize + i]; // Cr (V)
        }
        return output;
    }
   
    public static byte[] NV21toYUV420PackedSemiPlanar(final byte[] input, final byte[] output, final int width, final int height) {
        /* 
         * COLOR_TI_FormatYUV420PackedSemiPlanar is NV12.(plane[0]->Y, plane[1]->UV)
         * So we just have to reverse U and V.
         */
        final int frameSize = width * height;
        final int qFrameSize = frameSize/4;

        System.arraycopy(input, 0, output, 0, frameSize); // Y

        for (int i = 0; i < qFrameSize; i++) {
            output[frameSize + i*2] = input[frameSize + i*2 + 1]; // Cb (U)
            output[frameSize + i*2 + 1] = input[frameSize + i*2]; // Cr (V)
        }
        return output;
    }
    
    public static byte[] YV12toYUV420Planar(byte[] input, byte[] output, int width, int height) {
        /* 
         * COLOR_FormatYUV420Planar is I420 which is like YV12, but with U and V reversed.
         * So we just have to reverse U and V.
         */
        final int frameSize = width * height;
        final int qFrameSize = frameSize/4;

        System.arraycopy(input, 0, output, 0, frameSize); // Y
        System.arraycopy(input, frameSize, output, frameSize + qFrameSize, qFrameSize); // Cr (V)
        System.arraycopy(input, frameSize + qFrameSize, output, frameSize, qFrameSize); // Cb (U)

        return output;
    }
    public static byte[] NV21toYUV420Planar(byte[] input, byte[] output, int width, int height) {
        /* 
         * COLOR_FormatYUV420Planar is I420 which is like YV12, but with U and V reversed.(plane[0]->Y, plane[1]->U, plane[2]->V)
         * We convert by putting the corresponding U and V bytes together (interleaved), and reverse U and V.
         */
        final int frameSize = width * height;
        final int qFrameSize = frameSize/4;

        System.arraycopy(input, 0, output, 0, frameSize); // Y
        
        for (int i = 0; i < qFrameSize; i++) {
            output[frameSize + i] = input[frameSize + i*2 + 1]; // Cb (U)
            output[frameSize + qFrameSize + i] = input[frameSize + i*2]; // Cr (V)
        }
        return output;
    }
    
   
    /**
     * 
     * @param imageFormat support ImageFormat.NV21 or ImageFormat.YV12.
     */
    public ESNativeH264Encoder(int imageFormat) {
    	inputDataImageFormat = imageFormat;
    }
	public EncoderListener getEncoderListener() {
		return encoderListener;
	}
	public void setEncoderListener(EncoderListener encoderListener) {
		this.encoderListener = encoderListener;
	}
	public boolean setupCodec(ESVideoQuality videoQuality)
	{
		String mimeType = "video/avc";
        MediaCodecInfo codecInfo = MediaCodecUtils.selectCodec(mimeType);
        if(codecInfo == null) {
        	 Log.d(tag, "Can't find media codec correspondint to MIME:" + mimeType);
        	 return false;
        }
        Log.d(tag, "Found " + codecInfo.getName() + " supporting " + mimeType);
        
        MediaCodecInfo.CodecCapabilities capabilities = codecInfo.getCapabilitiesForType(mimeType);
        for (int i = 0; i < capabilities.colorFormats.length && colorFormat == 0; i++) {
            int format = capabilities.colorFormats[i];
            switch (format) {
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedSemiPlanar:
            case MediaCodecInfo.CodecCapabilities.COLOR_TI_FormatYUV420PackedSemiPlanar:
                colorFormat = format;
                break;
            default:
                Log.d(tag, "Skipping unsupported color format " + format);
                break;
            }
        }
		
		if(colorFormat == 0)
			return false;
		
		this.videoQuality = videoQuality.clone();
		
		int width, height;
		if(videoQuality.orientation == 90 || videoQuality.orientation == 270)
		{
			width = videoQuality.resY;
			height = videoQuality.resX;
		}
		else 
		{
			width = videoQuality.resX;
			height = videoQuality.resY;
		}
		if (codecInfo.getName().equals("OMX.TI.DUCATI1.VIDEO.H264E")) {
            // This codec doesn't support a width not a multiple of 16,
            // so round down.
			Log.d(tag, "Codec " + "OMX.TI.DUCATI1.VIDEO.H264E");
            width &= ~15;
        }
        
		int stride = width;
		int sliceHeight = height;
        if (codecInfo.getName().startsWith("OMX.Nvidia.")) {
            stride = (stride + 15)/16*16;
            sliceHeight = (sliceHeight + 15)/16*16;
            Log.d(tag, "Codec " + "OMX.Nvidia");
        }
        if(codecInfo.getName().startsWith("OMX.qcom"))
        {
        	
        }
	
        int frameSize = width * height * 3 / 2;
        inputFrame = new byte[frameSize];
//        outputH264Frame = new byte[frameSize];
		
		mediaCodec = MediaCodec.createEncoderByType(mimeType);  
		
        mediaFormat = MediaFormat.createVideoFormat(mimeType, width, height);  
        
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, videoQuality.bitrate);  
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, videoQuality.framerate);  
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, colorFormat);      
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 75); //关键帧间隔时间 单位s  
        mediaFormat.setInteger("stride", stride);
        mediaFormat.setInteger("slice-height", sliceHeight);  
        
        
        return true;
	}
	
	
	public void stop() {
		bThreadStop = true;
		encodeThread = null;
//		if(encodeThread != null)
//		{
//			try {
//				encodeThread.join();
//			} catch (Exception e) {
//			}
//			encodeThread = null;
//		}
		if(mediaCodec != null)
			mediaCodec.stop();  
       
	}
	

	public void release() {
		if(mediaCodec != null)
			 mediaCodec.release();  
        mediaCodec = null;
	}
	
	
	public void start(boolean async)
	{
		
		try {
            mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);  
            mediaCodec.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		if(async)
		{
			if(null == encodeThread)
			{
				bThreadStop = false;
				encodeThread = new Thread(this);
				encodeThread.start();
			}
		}
	}
	String outputFile = Environment.getExternalStorageDirectory().getAbsolutePath()
			.concat(File.separator).concat("chatdump").concat(File.separator).concat("testouputstream");
	int outputindex = 1;
	private void output(byte[] input) {
		FileOutputStream fos;
		try{
			///if(input[4] == 0x65 || input[4] == 0x67 || input[4] == 0x68) {
			fos = new FileOutputStream( new File(outputFile.concat(outputindex+".txt")), true);
			fos.write(input);
			fos.flush();
			fos.close();
		//	}
			outputindex ++;
		}catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	public void run() {
		
		while (!bThreadStop) {
			int index = -1;
			boolean isDataReady = false;
			
			synchronized(mutex)
			{
				try {
					mutex.wait();
					ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
							        			
					while(!Thread.interrupted())
					{
						index = mediaCodec.dequeueOutputBuffer(bufferInfo, 100000);
						if (index>=0 ){
//							System.out.println("<--cxd, encode video presentationTimeUs:"+(float)bufferInfo.presentationTimeUs / 1000000);
							TimeMonitor.videoEncodePresentationTimeUs = bufferInfo.presentationTimeUs;
							isDataReady = true;
							break;
						} else if (index == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
							outputBuffers = mediaCodec.getOutputBuffers();
						} else if (index == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
//							MediaMuxerWrapper.setVideoTrack(mediaCodec.getOutputFormat());
//							MediaMuxerWrapper.start();
						} else if (index == MediaCodec.INFO_TRY_AGAIN_LATER) {
							isDataReady = false;
							break;
						} else {
							isDataReady = false;
							break;
						}
					}
					
					if(isDataReady)
					{
						ByteBuffer outputBuffer = outputBuffers[index];
//						MediaMuxerWrapper.setVideoData(outputBuffer, bufferInfo);
						byte[] outData = new byte[bufferInfo.size];
						outputBuffer.get(outData);
						
//						boolean dataReady = sendToMediaCodec(outData);
//						if(dataReady) {
//							sendImagePacket(outData, bufferInfo.presentationTimeUs);//TODO outData->outputH264Frame
//						}
//						else {
//							getSPSAndPPS();
//							sendSPSAndPPSPacket();
//						}
						
						catchSPSAndPPSInfo(outData);
						sendImagePacket(outData, bufferInfo.presentationTimeUs);//TODO outData->outputH264Frame

						
					    mediaCodec.releaseOutputBuffer(index, false);
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
				
			}
		}
	}
	
	private byte[] h264spsBytes; 
	private byte[] h264ppsBytes;
	private void getSPSAndPPS() {
		if(m_info == null || m_info.length < 5)
			return;
		
		byte[] buff = new byte[5];
		int pos = 0;
		int sspPos = 0, ppsPos = 0;
		while(pos < m_info.length){
			System.arraycopy(m_info, pos, buff, 0, 5);
			if(Arrays.equals(buff, spsHeader))
				sspPos = pos;
			if(Arrays.equals(buff, ppsHeader))
				ppsPos = pos;
			pos++;
		}
		if (sspPos > 0 || ppsPos > 0) {
			int spsLength, ppsLength = 0;
			if (ppsPos > sspPos) {
				spsLength = ppsPos-sspPos;
				ppsLength = m_info.length-ppsPos;
				
			} else {
				spsLength = m_info.length-sspPos;
				ppsLength = sspPos-ppsPos;
			
			}
			h264spsBytes = new byte[spsLength];
			h264ppsBytes = new byte[ppsLength];
			System.arraycopy(m_info, sspPos, h264spsBytes, 0, spsLength);
			System.arraycopy(m_info, ppsPos, h264ppsBytes, 0, ppsLength);
		}
		else
			Log.e(tag, "Found SPS PPS info failed!");
	}
	
	private void sendSPSAndPPSPacket() {
		if(h264spsBytes != null &&  h264ppsBytes != null) {
			set264Packet("sps", h264spsBytes, 0);
			set264Packet("pps", h264ppsBytes, 0);
		}
	}
	
	private void sendImagePacket(byte[] outData, long timestamp) {
        int h264length = outData.length;
        byte[] naluData = Arrays.copyOfRange(outData, 0, h264length);
        if ((naluData[0] & 0x1f) == 5) { // IDR
        	sendSPSAndPPSPacket();
		}
        set264Packet("img", naluData, timestamp);
	}
	
	//TODO remove type
	private void set264Packet(String type, byte[] naluData, long timestamp) {
//		H264Nal nalu = new H264Nal();
//		nalu.setType(naluData[0] & 0x1f);
//		nalu.setRefIdc((naluData[0] & 0x60) >>> 5);
//		nalu.setPayload(naluData);
//		nalu.setPayloadLength(naluData.length);
//		P2PClientManager.getP2PClientInstance().send264Packet2(type, nalu, timestamp);
	    
		if(encoderListener != null)
	    	encoderListener.onH264EncodeFinished(type, naluData, timestamp);
	}
	
	private void catchSPSAndPPSInfo(byte[] outData) {
		if(m_info != null) 
			return;
    	byte[] buff = new byte[5];
        System.arraycopy(outData, 0 ,buff, 0, 5);
        if(Arrays.equals(buff, ppsHeader) || Arrays.equals(buff, spsHeader))
        {
            m_info = new byte[outData.length];
            System.arraycopy(outData, 0, m_info, 0, outData.length);
        }
	}
	private boolean sendToMediaCodec(byte[] outData) {
		int pos = 0; 
		boolean isDataReady = true;
	  

	    if(m_info != null)
	    {
//	        System.arraycopy(outData, 0, outputH264Frame, pos, outData.length);
	        pos += outData.length;
	    }
	    else //保存pps sps 只有开始时 第一个帧里有， 保存起来后面用
	    {
//	        ByteBuffer spsPpsBuffer = ByteBuffer.wrap(outData);
	    	byte[] buff = new byte[5];
	        System.arraycopy(outData, 0 ,buff, 0, 5);
//	        if (spsPpsBuffer.getInt() == 0x00000001)
	        if(Arrays.equals(buff, ppsHeader) || Arrays.equals(buff, spsHeader))
	        {
	            m_info = new byte[outData.length];
	            System.arraycopy(outData, 0, m_info, 0, outData.length);
	        }
	        else
	            isDataReady = false;
	    }
	    
	    if(isDataReady)
	    {
		    if(outData[4] == 0x65) //key frame   编码器生成关键帧时只有 00 00 00 01 65 没有pps sps， 要加上  
			{  
		    	byte[] keyFrameData = new byte[pos];
//				System.arraycopy(outputH264Frame, 0, keyFrameData, 0, pos);  
//				System.arraycopy(m_info, 0,  outputH264Frame, 0, m_info.length);  
//				System.arraycopy(keyFrameData, 0,  outputH264Frame, m_info.length, pos);  
				pos += m_info.length;  
			}	
		   
//		    output(outputH264Frame);
//		    if(encoderListener != null)
//		    	encoderListener.onEncodeFinished(outputH264Frame, pos);
	    }
	    return isDataReady;
	}
	
	public void dataTransefer(byte[] data)
	{
		switch (colorFormat) {
		case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
		case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar:
		{
			switch (inputDataImageFormat) {
			case ImageFormat.YV12:
				YV12toYUV420Planar(data, inputFrame, videoQuality.resX, videoQuality.resY);
				break;
			case ImageFormat.NV21:
				NV21toYUV420Planar(data, inputFrame, videoQuality.resX, videoQuality.resY);
				break;
			default:
				break;
			}
		}
			break;
		case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar://using
        case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedSemiPlanar:
        case MediaCodecInfo.CodecCapabilities.COLOR_TI_FormatYUV420PackedSemiPlanar:
        {
        	switch (inputDataImageFormat) {
			case ImageFormat.YV12:
				YV12toYUV420PackedSemiPlanar(data, inputFrame, videoQuality.resX, videoQuality.resY);
				break;
			case ImageFormat.NV21:
				NV21toYUV420PackedSemiPlanar(data, inputFrame, videoQuality.resX, videoQuality.resY);
				break;
			default:
				break;
			}
        	
        }
        	break;
		default:
		{
			return;
		}
		}
	}
	
	public void putData(VideoFrameItem videoFrame) {
		if(mediaCodec != null)
		{
			byte[] data =videoFrame.getDataBytes();
			int length = data.length;
			dataTransefer(data);//TODO should more efficient
			byte[] rotateData = rotateYUV420Degree90(inputFrame,videoQuality.resX,videoQuality.resY);
			System.arraycopy(rotateData, 0, inputFrame, 0, length);
			
			
			final ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers(); 
			int inputBufferIndex;
			final int timeout = 1000000/videoQuality.framerate;
			
            do
                inputBufferIndex = mediaCodec.dequeueInputBuffer(timeout);
            while (inputBufferIndex < 0);
			
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
            inputBuffer.clear();
            inputBuffer.put(inputFrame,0,inputFrame.length);
            mediaCodec.queueInputBuffer(inputBufferIndex, 0, inputFrame.length, System.nanoTime()/1000, 0);  
            synchronized (mutex) {
				mutex.notifyAll();
		    }
		}
	}
	
	private byte[] rotateYUV420Degree90(byte[] data, int imageWidth, int imageHeight) 
    {
        byte [] yuv = new byte[imageWidth*imageHeight*3/2];
        // Rotate the Y luma
        int i = 0;
        for(int x = 0;x < imageWidth;x++)
        {
            for(int y = imageHeight-1;y >= 0;y--)                               
            {
                yuv[i] = data[y*imageWidth+x];
                i++;
            }
        }
        // Rotate the U and V color components 
        i = imageWidth*imageHeight*3/2-1;
        for(int x = imageWidth-1;x > 0;x=x-2)
        {
            for(int y = 0;y < imageHeight/2;y++)                                
            {
                yuv[i] = data[(imageWidth*imageHeight)+(y*imageWidth)+x];
                i--;
                yuv[i] = data[(imageWidth*imageHeight)+(y*imageWidth)+(x-1)];
                i--;
            }
        }
        return yuv;
    }
}
