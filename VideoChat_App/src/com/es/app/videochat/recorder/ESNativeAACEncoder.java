package com.es.app.videochat.recorder;

import java.nio.ByteBuffer;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaCodec.BufferInfo;
import android.util.Log;

public class ESNativeAACEncoder extends ESAudioEncoder implements Runnable {

	public final static String TAG = "ES.ESNativeAACEncoder";
	
	private MediaCodec mediaCodec;  
	private BufferInfo bufferInfo = new BufferInfo();
	private volatile Object mutex = null;
	
	long audioStartTime = 0;
	int frameCount = 0;
	int audioBytesReceived = 0;
	
	private Thread encodeThread = null;
	
	@SuppressWarnings("unused")
	@Override
	public boolean setupCodec(ESAudioQuality audioQuality) {
		// TODO Auto-generated method stub
		
		final String mimeType = "audio/mp4a-latm";
		final int bufferSize = AudioRecord.getMinBufferSize(audioQuality.samplingRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT)*2;
		
		final int kSampleRates[] = { 8000, 11025, 22050, 44100, 48000 };
        final int kBitRates[] = { 64000, 128000 };
        
		mediaCodec = MediaCodec.createEncoderByType(mimeType);
		MediaFormat format = new MediaFormat();
		format.setString(MediaFormat.KEY_MIME, mimeType);
		format.setInteger(MediaFormat.KEY_BIT_RATE, audioQuality.bitRate);
		format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
		format.setInteger(MediaFormat.KEY_SAMPLE_RATE, audioQuality.samplingRate);
		format.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
		format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, bufferSize);
		
		try {
			mediaCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
			mediaCodec.start();
			return true;

		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return false;
	}
	
	@Override
	public void stop() {
		// TODO Auto-generated method stub
		
		if(encodeThread != null)
		{			
			encodeThread.interrupt();
			try {
				encodeThread.join();
			} catch (Exception e) {
				// TODO: handle exception
			}
		}
		
		mediaCodec.stop();  
        mediaCodec.release();  
        mediaCodec = null;
	}
	
	@Override
	public void start()
	{
		if(null == encodeThread)
		{
			encodeThread = new Thread(this);
			encodeThread.start();
		}
	}
	@Override
	public void putData(byte[] data, int length) {
		
		if(mediaCodec != null)
		{
			if(0 == audioBytesReceived)
				audioStartTime = System.nanoTime();
			
			audioBytesReceived += length;
			
			if(null == mutex)
				mutex = new Object();
			
			final ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers(); 
			int inputBufferIndex;
			
            do
            {
                inputBufferIndex = mediaCodec.dequeueInputBuffer(0);
                if(Thread.interrupted())
                	break;
            }
            while (inputBufferIndex < 0);
			
            long presentationTimeUs = (System.nanoTime() - audioStartTime) / 1000;
            
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
            inputBuffer.clear();
            inputBuffer.put(data,0,length);
            mediaCodec.queueInputBuffer(inputBufferIndex, 0, length, presentationTimeUs, 0);  
			
            synchronized (mutex) {
				mutex.notifyAll();
		    }
		}
	}
	@SuppressWarnings("unused")
	public void encode()
	{
		int index = -1;
		Boolean isDataReady = false;
		
		ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();	               
		while (true) {
			
			index = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
			if (index>=0 ){
				isDataReady = true;
				break;
			} else if (index == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
				outputBuffers = mediaCodec.getOutputBuffers();
			} else if (index == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
				MediaFormat mediaFormat = mediaCodec.getOutputFormat();
				Log.i(TAG,mediaFormat.toString());
			} else if (index == MediaCodec.INFO_TRY_AGAIN_LATER) {
				Log.i(TAG,"No buffer available...");
				isDataReady = false;
				break;
			} else {
				isDataReady = false;
				break;
			}
		}
		if((bufferInfo.flags&MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0)
		{
		    mediaCodec.releaseOutputBuffer(index, false);
			isDataReady = false;
		}
		if(isDataReady)
		{
			ByteBuffer outputBuffer = outputBuffers[index];
			
			byte[] data= null;
			
			if(true)
			{
				int outBitsSize   = bufferInfo.size;
			    int outPacketSize = outBitsSize + 7;    // 7 is ADTS size

			    outputBuffer.position(bufferInfo.offset);
			    outputBuffer.limit(bufferInfo.offset + outBitsSize);
			    
			    data = new byte[outPacketSize];  //space for ADTS header included
		        addADTStoPacket(data, outPacketSize);
		        outputBuffer.get(data, 7, outBitsSize);
		        outputBuffer.position(bufferInfo.offset);
		        if(encoderListener != null)
			    {
		        	encoderListener.onEncodeFinished(data, outPacketSize);
			    }
			}
			else
			{
				outputBuffer.position(bufferInfo.offset);
			    outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
			    
				data = new byte[bufferInfo.size];
				outputBuffer.get(data);
				if(encoderListener != null)
			    {
					encoderListener.onEncodeFinished(data, bufferInfo.size);
			    }
			}
		    mediaCodec.releaseOutputBuffer(index, false);
		}
	}
	@SuppressWarnings("unused")
	@Override
	public void run() {
		// TODO Auto-generated method stub
		
		
		while (!Thread.interrupted()) {

			int index = -1;
			Boolean isDataReady = false;
			
			if(null == mutex)
				continue;
			
			synchronized(mutex)
			{
				try {
				
					mutex.wait();
					
					ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();	               
					while (true) {
						
						index = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
						if (index>=0 ){
							isDataReady = true;
							break;
						} else if (index == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
							outputBuffers = mediaCodec.getOutputBuffers();
						} else if (index == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
							MediaFormat mediaFormat = mediaCodec.getOutputFormat();
							Log.i(TAG,mediaFormat.toString());
						} else if (index == MediaCodec.INFO_TRY_AGAIN_LATER) {
							Log.i(TAG,"No buffer available...");
							isDataReady = false;
							break;
						} else {
							isDataReady = false;
							break;
						}
					}
					if((bufferInfo.flags&MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0)
					{
					    mediaCodec.releaseOutputBuffer(index, false);
						isDataReady = false;
					}
					if(isDataReady)
					{
						ByteBuffer outputBuffer = outputBuffers[index];
						
						byte[] data= null;
						
						if(true)
						{
							int outBitsSize   = bufferInfo.size;
						    int outPacketSize = outBitsSize + 7;    // 7 is ADTS size
		
						    outputBuffer.position(bufferInfo.offset);
						    outputBuffer.limit(bufferInfo.offset + outBitsSize);
						    
						    data = new byte[outPacketSize];  //space for ADTS header included
					        addADTStoPacket(data, outPacketSize);
					        outputBuffer.get(data, 7, outBitsSize);
					        outputBuffer.position(bufferInfo.offset);
					        if(encoderListener != null)
						    {
					        	encoderListener.onEncodeFinished(data, outPacketSize);
						    }
						}
						else
						{
							data = new byte[bufferInfo.size];
							outputBuffer.get(data);
							if(encoderListener != null)
						    {
								encoderListener.onEncodeFinished(data, bufferInfo.size);
						    }
						}
						
					    mediaCodec.releaseOutputBuffer(index, false);	
					}
					
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	
	public void addADTStoPacket(byte[] packet, int packetLen) {
	    int profile = 2;  //AAC LC
	                      //39=MediaCodecInfo.CodecProfileLevel.AACObjectELD;
	    int freqIdx = 4;  //44.1KHz
	    int chanCfg = 2;  //CPE

	    // fill in ADTS data
	    packet[0] = (byte)0xFF;
	    packet[1] = (byte)0xF9;
	    packet[2] = (byte)(((profile-1)<<6) + (freqIdx<<2) +(chanCfg>>2));
	    packet[3] = (byte)(((chanCfg&3)<<6) + (packetLen>>11));
	    packet[4] = (byte)((packetLen&0x7FF) >> 3);
	    packet[5] = (byte)(((packetLen&7)<<5) + 0x1F);
	    packet[6] = (byte)0xFC;
	}
}
