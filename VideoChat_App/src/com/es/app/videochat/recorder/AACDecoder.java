package com.es.app.videochat.recorder;


import java.nio.ByteBuffer;
import java.util.concurrent.LinkedBlockingQueue;


import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.util.Log;

public class AACDecoder extends AudioDecoderBase implements Runnable{
	private final String Tag = AACDecoder.class.getSimpleName();
	private final String mimeType = "audio/mp4a-latm";
	private final int Rate = 44100;//8000;//
	
	private AudioTrack player;
	private MediaCodec audioCodec;  
	private short audioFormat = AudioFormat.ENCODING_PCM_16BIT;
    private short channelConfig = AudioFormat.CHANNEL_IN_MONO;
	
//	long firstReceivedTime = 0;
	boolean bStop = true;
	  
	public AACDecoder() {
		
	}
	
	private MediaFormat getMediaFormat() {
		MediaFormat format = new MediaFormat();
        format.setString(MediaFormat.KEY_MIME, "audio/mp4a-latm");
        format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
        format.setInteger(MediaFormat.KEY_SAMPLE_RATE, Rate);
        format.setInteger(MediaFormat.KEY_BIT_RATE, Rate * 16);
        format.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        return format;
	}
	
	private boolean usingThread = false;
	public boolean start() {
		if(!initDecoder() || !initPlayer())
			return false;
		innerStart();
		if(usingThread) 
			(new Thread(this)).start();
		return true;
	}
	
	private boolean initDecoder() {
		try{
			MediaFormat format = getMediaFormat();
			audioCodec = MediaCodec.createDecoderByType("audio/mp4a-latm");
			audioCodec.configure(format, null, null, 0);
		}catch(Exception e){
			return false;
		}
		return true;
	}
    private boolean initPlayer()
    {
        int bufferSizePlayer = AudioTrack.getMinBufferSize(Rate, AudioFormat.CHANNEL_OUT_MONO, audioFormat);
        
        Log.d("====buffer Size player ", String.valueOf(bufferSizePlayer));

        player = new AudioTrack(AudioManager.STREAM_MUSIC, Rate, AudioFormat.CHANNEL_OUT_MONO, audioFormat, bufferSizePlayer, AudioTrack.MODE_STREAM);
        if (player.getState() == AudioTrack.STATE_INITIALIZED)
        {

            return true;
        }
        else
        {
            return false;
        }

    }
	
	@Override
	public void run() {
		try
        {
            while (!bStop) {
//            	decodeAndPlay(inputByteQueue.poll());
            }
        }
        catch (Exception e)
        {
        	e.printStackTrace();
        }
	}
	private void decodeAndPlay(byte[] data, long presentationTimeUs) {
		try
        {
//        	Log.d(Tag," decodeAndPlay start.");
        	if(data == null)
        		return;
        	
        	ByteBuffer[] inputBuffers = audioCodec.getInputBuffers();
        	ByteBuffer[] outputBuffers = audioCodec.getOutputBuffers();
        	ByteBuffer inputBuffer, outputBuffer;
        	byte[] outData;
            int inputBufferIndex = audioCodec.dequeueInputBuffer(-1);
            Log.d(Tag,"mediaCodec input buffer index= " + inputBufferIndex+", input data length:"+data.length);
            if (inputBufferIndex >= 0)
            {
            	inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();
                inputBuffer.put(data);
                audioCodec.queueInputBuffer(inputBufferIndex, 0, data.length, presentationTimeUs, 0);
            }

            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = audioCodec.dequeueOutputBuffer(bufferInfo, 0);

            while (outputBufferIndex >= 0)
            {
//            	TimeMonitor.audioDecodePresentationTimeUs = bufferInfo.presentationTimeUs;
//            	TimeMonitor.printDecodeInterval();
            	System.out.println("<--cxd, decode audio in java presentationTimeUs:"+(float)bufferInfo.presentationTimeUs/1000000 );
            	
                outputBuffer = outputBuffers[outputBufferIndex];

                outputBuffer.position(bufferInfo.offset);
                outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                outData = new byte[bufferInfo.size];
                outputBuffer.get(outData);
                player.write(outData, 0, outData.length);
                audioCodec.releaseOutputBuffer(outputBufferIndex, false);
                outputBufferIndex = audioCodec.dequeueOutputBuffer(bufferInfo, 0);

            }
        }
        catch (Exception e)
        {
        	e.printStackTrace();
        }
	}
	
//	private void decodeAndPlayLoop() {
//        try
//        {
//            while (!bStop) {
//            	decodeAndPlay(inputByteQueue.poll());
//            }
//
//            stop();
//
//        }
//        catch (Exception e)
//        {
//        	e.printStackTrace();
//        }
//    
//	}
	
//	long startTime = 0;
//	private void doDecode2(MediaFormat format) {
//		
//		String mime = format.getString(MediaFormat.KEY_MIME);
//		int channels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
//		int sampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
//        ByteBuffer[] codecInputBuffers  = mediaCodec.getInputBuffers();
//        ByteBuffer[] codecOutputBuffers = mediaCodec.getOutputBuffers();
//        
//        // configure AudioTrack
//		int channelConfiguration = channels == 1 ? AudioFormat.CHANNEL_OUT_MONO : AudioFormat.CHANNEL_OUT_STEREO;
//        int minSize = AudioTrack.getMinBufferSize( sampleRate, channelConfiguration, AudioFormat.ENCODING_PCM_16BIT);
//        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate, channelConfiguration, 
//        		AudioFormat.ENCODING_PCM_16BIT, minSize, AudioTrack.MODE_STREAM);
//      
//		// start playing, we will feed the AudioTrack later
//        audioTrack.play();
//        
//        // start decoding
//        final long kTimeOutUs = 1000;
//        MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
//        boolean sawInputEOS = false;
//        boolean sawOutputEOS = false;
//        int noOutputCounter = 0;
//        int noOutputCounterLimit = 10;
//        
////        while (!sawOutputEOS && noOutputCounter < noOutputCounterLimit) {
//        	Log.e(Tag, "cxd loop start...");
////        	noOutputCounter++;
//        	// read a buffer before feeding it to the decoder
//            if (!sawInputEOS) {
//            	int inputBufIndex = mediaCodec.dequeueInputBuffer(kTimeOutUs);
//                if (inputBufIndex >= 0) {
//                    ByteBuffer inputBuffer = codecInputBuffers[inputBufIndex];
//                    byte[] inputBytes = inputByteQueue.poll();
//                  
//                    if (inputBytes == null || !startWithHearder(inputBytes)) {
//                        
//                        
//                    } else {
//                    	inputBuffer.clear();
//                    	inputBuffer.put(inputBytes,56, inputBytes.length-56);//remove header
//                    	if(presentationTimeUs == 0)
//                    		startTime = System.currentTimeMillis();
//                    	else
//                    		presentationTimeUs = 1000*(System.currentTimeMillis() - startTime);//extractor.getSampleTime();
//                    }
//   System.out.println("cxd, presentationTimeUs="+presentationTimeUs);
//                	mediaCodec.queueInputBuffer(inputBufIndex, 56, inputBytes.length-56, presentationTimeUs, presentationTimeUs == 0 ? MediaCodec.BUFFER_FLAG_CODEC_CONFIG : 0);
//                	
//                    
//                } else {
//                	Log.e(Tag, "cxd inputBufIndex " +inputBufIndex);
//                }
//            } // !sawInputEOS
//try{
//            // decode to PCM and push it to the AudioTrack player
//            int res = mediaCodec.dequeueOutputBuffer(info, 100000);
//            Log.d(Tag, "cxd  dequeue from codec, return "+res);
//            if (res >= 0) {
//                if (info.size > 0)  noOutputCounter = 0;
//                
//                int outputBufIndex = res;
//                ByteBuffer buf = codecOutputBuffers[outputBufIndex];
//
//                final byte[] chunk = new byte[info.size];
//                buf.get(chunk);
//                buf.clear();
//                if(chunk.length > 0){        	
//                	audioTrack.write(chunk,0,chunk.length);
//                	/*if(this.state.get() != PlayerStates.PLAYING) {
//                		if (events != null) handler.post(new Runnable() { @Override public void run() { events.onPlay();  } }); 
//            			state.set(PlayerStates.PLAYING);
//                	}*/
//                	
//                }
//                mediaCodec.releaseOutputBuffer(outputBufIndex, false);
//                if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
//                    Log.d(Tag, "cxd saw output EOS.");
//                    sawOutputEOS = true;
//                }
//            } else if (res == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
//                codecOutputBuffers = mediaCodec.getOutputBuffers();
//                Log.d(Tag, "cxd output buffers have changed.");
//            } else if (res == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
//                MediaFormat oformat = mediaCodec.getOutputFormat();
//                Log.d(Tag, "cxd output format has changed to " + oformat);
//            } else {
//                Log.d(Tag, "cxd dequeueOutputBuffer returned " + res);
//            }
////        }
//}catch(Exception e)
//{
//	e.printStackTrace();
//}
//        Log.d(Tag, "cxd stopping...");
//	}
	
	
	public void stop() {
		if(audioCodec != null) {
        	audioCodec.stop();
        	audioCodec.release();
        	audioCodec = null;
		}
		
		if(player != null) {
			player.stop();
	    	player.release();
	    	player = null;	
	    }
		bStop = true;
	}

	private void innerStart() {
		if(audioCodec != null) {
        	audioCodec.start();
		}
		
		if(player != null) {
			player.play();
	    }
		bStop = false;
	}
	
	
//	private LinkedBlockingQueue<byte[]> inputByteQueue = new LinkedBlockingQueue<byte[]>(10);
//	public void onFrame(byte[] buf, int offset, int length, double timestamp) {
////		output(buf);
//		try {
//			byte[] newRevData = new byte[length];
//			System.arraycopy(buf, 0, newRevData, 0, length);
//			if(usingThread)
//				inputByteQueue.put(newRevData);//TODO
//			else 
//				decodeAndPlay(newRevData);
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		  
////		ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
////		int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
////		if (inputBufferIndex >= 0) {
////			ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
////			inputBuffer.clear();
////			inputBuffer.put(buf, offset, length);
////			mediaCodec.queueInputBuffer(inputBufferIndex, 0, length,
////					mCount * 1000000 / 8000, 0);
////			mCount++;
////		}
////
////		MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
////		int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
////		while (outputBufferIndex >= 0) {
////			mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
////			outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
////		}
//	}

	@Override
	public void decode(byte[] data, long timestamp) {
		decodeAndPlay(data, timestamp);		
	}
}
