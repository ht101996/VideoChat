package com.arcsoft.ais.arcvc.utils;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;

public class MyBitmap {

	/*
	 * byte[] data保存的是纯RGB的数据，而非完整的图片文件数据
	 */
	static public Bitmap createMyBitmap(int[] colors, int width, int height) {
		Bitmap bmp = null;

		try {
			if (colors != null) {
				bmp = Bitmap.createBitmap(colors, width, height, Bitmap.Config.ARGB_8888);
			}
		} catch (Exception e) {
			// TODO: handle exception

			return null;
		}

		return bmp;
	}
	
	/**
	 * 图片去色
	 * @param bmpOriginal
	 * @return
	 */
	public static Bitmap convertGreyImg(Bitmap bmpOriginal) {    
		int width, height;
        height = bmpOriginal.getHeight();
        width = bmpOriginal.getWidth();

        Bitmap bmpGrayscale = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        Canvas c = new Canvas(bmpGrayscale);
        Paint paint = new Paint();
        ColorMatrix cm = new ColorMatrix();
        cm.setSaturation(0);
        ColorMatrixColorFilter f = new ColorMatrixColorFilter(cm);
        paint.setColorFilter(f);
        c.drawBitmap(bmpOriginal, 0, 0, paint);
        return bmpGrayscale;
    }    

	/*
	 * 将RGB数组转化为像素数组
	 */
	private static int[] convertByteToColor(byte[] data) {
		int size = data.length;
		if (size == 0) {
			return null;
		}

		// 理论上data的长度应该是3的倍数，这里做个兼容
		int arg = 0;
		if (size % 3 != 0) {
			arg = 1;
		}

		int[] color = new int[size / 3 + arg];

		if (arg == 0) { // 正好是3的倍数
			for (int i = 0; i < color.length; ++i) {

				color[i] = (data[i * 3] << 16 & 0x00FF0000) | (data[i * 3 + 1] << 8 & 0x0000FF00) | (data[i * 3 + 2] & 0x000000FF) | 0xFF000000;
			}
		} else { // 不是3的倍数
			for (int i = 0; i < color.length - 1; ++i) {
				color[i] = (data[i * 3] << 16 & 0x00FF0000) | (data[i * 3 + 1] << 8 & 0x0000FF00) | (data[i * 3 + 2] & 0x000000FF) | 0xFF000000;
			}

			color[color.length - 1] = 0xFF000000; // 最后一个像素用黑色填充
		}

		return color;
	}

}
