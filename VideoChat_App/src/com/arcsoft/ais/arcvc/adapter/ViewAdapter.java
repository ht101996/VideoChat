package com.arcsoft.ais.arcvc.adapter;

import java.util.List;
import java.util.Map;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.arcsoft.ais.arcvc.R;
import com.arcsoft.ais.arcvc.utils.Global;

public class ViewAdapter extends BaseAdapter{
    
	private List<Map<String,Object>> mapList;
	
	LayoutInflater mInflater;
	
	public ViewAdapter(List<Map<String,Object>> mapList, Context context){
		this.mapList = mapList;
		this.mInflater = LayoutInflater.from(context);
	}
	
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mapList.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return mapList.get(position);
	}

	@Override
	public long getItemId(int arg0) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		 ViewHolder holder;
		 if (convertView == null) {
             convertView = mInflater.inflate(R.layout.friend_list_item,null);
             holder = new ViewHolder();
            /**得到各个控件的对象*/                    
            holder.textView = (TextView) convertView.findViewById(R.id.ItemText);
            holder.imageView = (ImageView) convertView.findViewById(R.id.ItemImage);
            convertView.setTag(holder);//绑定ViewHolder对象                   
		 }
		 else{
		    holder = (ViewHolder)convertView.getTag();//取出ViewHolder对象                  
		 }
		 /**设置TextView显示的内容，即我们存放在动态数组中的数据*/            
         holder.textView.setText(mapList.get(position).get("ItemText").toString());
         Bitmap bitmap = (Bitmap)mapList.get(position).get("ItemImage");
         holder.imageView.setImageBitmap(bitmap);
         return convertView;
	}
	
	public void changeImage(int position, Map<String, Object> map){
		Log.i(Global.TAG, "ViewAdapter changeImage==========map==" + map);
		mapList.set(position, map);
		notifyDataSetChanged();
	}
	
	public class ViewHolder{   
        TextView textView;  
        ImageView imageView;  
    }  

}
