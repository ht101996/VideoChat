package com.es.app.videochat.recorder;

import com.es.app.videochat.recorder.ESRecordListener.OnEncoderListener;


public abstract class ESVideoEncoder {

	OnEncoderListener encoderListener;
	
	abstract public boolean setupCodec(ESVideoQuality quality);
	abstract public void start(boolean async);
	abstract public void stop();
	abstract public void release();
	abstract public void putData(VideoFrameItem videoFrame);
	
	public OnEncoderListener getEncoderListener() {
		return encoderListener;
	}
	public void setEncoderListener(OnEncoderListener encoderListener) {
		this.encoderListener = encoderListener;
	}

	static void rotate270(byte[] in, int offin, byte[] out, int offout, int w, int h)
    {
       int k =offout;
       for(int j =0; j <w; j++)
       {
           int idx =w -1 -j;
           for(int i =0; i <h; i++)
           {
              out[k] =in[offin +i *w +idx];
              k   ++;
           }
       }
    }
   
    static void rotate90(byte[] in, int offin, byte[] out, int offout, int w, int h)
    {
       int k =offout;
       for(int j =0; j <w; j++)
       {
           for(int i =0; i <h; i++)
           {
              out[k] =in[offin +(w -1 -i) *w +j];
              k   ++;
           }
       }
    }
   
    public static byte[] YV12toYUV420Planar_r270(byte[] input, byte[] output, int width, int height)
    {
        final int frameSize = width * height;
        final int qFrameSize = frameSize/4;
 
        rotate270(input, 0, output, 0, width, height);
        rotate270(input, frameSize, output, frameSize + qFrameSize, width /2, height /2);
        rotate270(input, frameSize + qFrameSize, output, frameSize, width /2, height /2);
        return output;
    }
    
    
}
