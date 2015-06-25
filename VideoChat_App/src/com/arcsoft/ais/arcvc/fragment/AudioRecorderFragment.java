package com.arcsoft.ais.arcvc.fragment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import com.arcsoft.ais.arcvc.utils.P2PClientManager;

import android.app.Fragment;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaRecorder.AudioSource;
import android.os.Environment;
import android.util.Log;

public class AudioRecorderFragment extends Fragment {
	private static final String tag = AudioRecorderFragment.class.getSimpleName();
	private int bufferSize;
	private int audioSampleRate;
	private MediaCodec encoder;
	private AudioRecord recorder;
	private short audioFormat = AudioFormat.ENCODING_PCM_16BIT;
	private short channelConfig = AudioFormat.CHANNEL_IN_MONO;
	private boolean bSendingAudio = false;
	private boolean setEncoder(int rate)
	{
		audioSampleRate = rate;
		encoder = MediaCodec.createEncoderByType("audio/mp4a-latm");
		MediaFormat format = new MediaFormat();
		format.setString(MediaFormat.KEY_MIME, "audio/mp4a-latm");
		format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
		format.setInteger(MediaFormat.KEY_SAMPLE_RATE, rate);// rate = 44100
		format.setInteger(MediaFormat.KEY_BIT_RATE, rate * 16);// 64kbps?
//		format.setInteger(MediaFormat.KEY_IS_ADTS, 1);//lots driver unsupported, throws IllegalStateException
		format.setInteger(MediaFormat.KEY_AAC_PROFILE,
				MediaCodecInfo.CodecProfileLevel.AACObjectLC);
		encoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
		return true;
	}
    private int findAudioRecord()
    {
        for (int rate : new int[]{44100})
        {
            try
            {
                bufferSize = AudioRecord.getMinBufferSize(rate, channelConfig, audioFormat);

                if (bufferSize != AudioRecord.ERROR_BAD_VALUE)
                {
                    // check if we can instantiate and have a success
                    recorder = new AudioRecord(AudioSource.MIC, rate, channelConfig, audioFormat, bufferSize);

                    if (recorder.getState() == AudioRecord.STATE_INITIALIZED)
                    {
                        Log.v(tag, "===========final rate :"+ rate + "Hz, bits: " + audioFormat + ", channel: " + channelConfig);

                        return rate;
                    }
                }
            }
            catch (Exception e)
            {
                Log.v("error", "" + rate);
            }

        }
        return -1;
    }
    private String filePath = Environment.getExternalStorageDirectory().getPath().concat(File.separator).concat("sending.m4p");
    
    public void stopAudioRecorder() {
    	bSendingAudio = false;
    }
	public boolean startAudioRecorder() {
		int rate = findAudioRecord();
        if (rate == -1) 
        	return false;
        boolean encoderReady = setEncoder(rate);
	    if (!encoderReady)
	         return false;
	    bSendingAudio = true;
		Thread IOrecorder = new Thread(new Runnable()
        {
        	private void addADTStoPacket(byte[] packet, int packetLen) {
        	    int profile = 2;  //AAC LC
        	                      //39=MediaCodecInfo.CodecProfileLevel.AACObjectELD;
        	    int freqIdx = 11;  //44.1KHz
        	    int chanCfg = 1;  //CPE

        	    // fill in ADTS data
        	    packet[0] = (byte)0xFF;
        	    packet[1] = (byte)0xF1;
        	    packet[2] = (byte)(((profile-1)<<6) + (freqIdx<<2) +(chanCfg>>2));
        	    packet[3] = (byte)(((chanCfg&0x3)<<6) + (packetLen>>11));
        	    packet[4] = (byte)((packetLen&0x7FF) >> 3);
        	    packet[5] = (byte)(((packetLen&7)<<5) + 0x1F);
        	    packet[6] = (byte)0xFC;
        	
        	}
            public void run()
            {
                byte[] buffer1 = new byte[bufferSize];

                ByteBuffer[] inputBuffers;
                ByteBuffer[] outputBuffers;

                ByteBuffer inputBuffer;
                ByteBuffer outputBuffer;

                MediaCodec.BufferInfo bufferInfo;
                int inputBufferIndex;
                int outputBufferIndex;
                FileOutputStream fos = null;
                byte[] outData;

                try
                {
                    encoder.start();
                    recorder.startRecording();
                    bSendingAudio = true;
                    File file = new File(filePath);
                    fos = new FileOutputStream(file);;
                    while (bSendingAudio)
                    {
                        int read = recorder.read(buffer1, 0, bufferSize);
                        if(AudioRecord.ERROR_INVALID_OPERATION == read || AudioRecord.ERROR_BAD_VALUE == read)
                        	break;

                        inputBuffers = encoder.getInputBuffers();
                        outputBuffers = encoder.getOutputBuffers();
                        inputBufferIndex = encoder.dequeueInputBuffer(0);
                        if (inputBufferIndex >= 0)
                        {
                            inputBuffer = inputBuffers[inputBufferIndex];
                            inputBuffer.clear();

                            inputBuffer.put(buffer1);
                            long pts = getJitterFreePTS(System.nanoTime() / 1000, read / 2);
                            encoder.queueInputBuffer(inputBufferIndex, 0, buffer1.length, pts, 0);
                        }

                        bufferInfo = new MediaCodec.BufferInfo();
                        outputBufferIndex = encoder.dequeueOutputBuffer(bufferInfo, 0);

                        while (outputBufferIndex >= 0)
                        {
//                        	System.out.println("<--cxd, encode audio presentationTimeUs:"+(float)bufferInfo.presentationTimeUs / 1000000+",current time:"+System.nanoTime());
                        	
                            outputBuffer = outputBuffers[outputBufferIndex];
                            outputBuffer.position(bufferInfo.offset);
                            outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                            outData = new byte[bufferInfo.size];
                           
                            if(false) {
                            	int outPacketSize = bufferInfo.size + 7; 
                            	byte[] data = new byte[outPacketSize];  //space for ADTS header included
                                addADTStoPacket(data, outPacketSize);
                                outputBuffer.get(data, 7, bufferInfo.size);
                                outputBuffer.position(bufferInfo.offset);
                                fos.write(data, 0, outPacketSize);
                            }
                            outputBuffer.get(outData);
                            P2PClientManager.getP2PClientInstance().sendAACESData(outData, outData.length, bufferInfo.presentationTimeUs);
                            encoder.releaseOutputBuffer(outputBufferIndex, false);
                            outputBufferIndex = encoder.dequeueOutputBuffer(bufferInfo, 0);
                           
                        }
                        if (outputBufferIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) 
                        {
                             outputBuffers = encoder.getOutputBuffers();
                        } 
                        else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) 
                        {
                        }
                        // ----------------------;

                    }
//                    fos.close();
//                    encoder.stop();
//                    recorder.stop();
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                finally{
                	if(fos != null)
						try {
							fos.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
                	encoder.stop();
                    recorder.stop();
                }
            }
        });
		IOrecorder.start();
		return true;
	}
	
	 private long startPTS = 0;
	 private long totalSamplesNum = 0;

    /**
     * Ensures that each audio pts differs by a constant amount from the previous one.
     * @param bufferPts presentation timestamp in us
     * @param bufferSamplesNum the number of samples of the buffer's frame
     * @return
     */
    private long getJitterFreePTS(long bufferPts, long bufferSamplesNum) {
        long correctedPts = 0;
        long bufferDuration = (1000000 * bufferSamplesNum) / (audioSampleRate);
        bufferPts -= bufferDuration; // accounts for the delay of acquiring the audio buffer
        if (totalSamplesNum == 0) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
        }
        correctedPts = startPTS +  (1000000 * totalSamplesNum) / (audioSampleRate);
        if(bufferPts - correctedPts >= 2*bufferDuration) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
            correctedPts = startPTS;
        }
        totalSamplesNum += bufferSamplesNum;
        return correctedPts;
    }

}
