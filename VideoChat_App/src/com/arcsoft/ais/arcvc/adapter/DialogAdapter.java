package com.arcsoft.ais.arcvc.adapter;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.arcsoft.ais.arcvc.R;

public class DialogAdapter extends BaseAdapter{
	
	List<String> contents = new ArrayList<String>();
	LayoutInflater mInflater;
	private Context context; 
	
	public DialogAdapter(Context context){
		this.context = context;
		this.mInflater = LayoutInflater.from(context);
	}

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return contents.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return contents.get(position);
	}

	@Override
	public long getItemId(int arg0) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup arg2) {
		PeopleView s = null;
		String content = contents.get(position);
		String[] array = content.split(",");

		if (null == convertView) {
			s = new PeopleView();

			switch (Integer.parseInt(array[0])) {
			case 0:
				convertView = mInflater.inflate(R.layout.listview_item_received, null);
				break;
			case 1:
				convertView = mInflater.inflate(R.layout.listview_item_send, null);
				break;
			}

			s.time = (TextView) convertView.findViewById(R.id.Time);
			s.message = (TextView) convertView.findViewById(R.id.Msg);
			s.portrait = (ImageView) convertView.findViewById(R.id.Img);
			convertView.setTag(s);
		} else{
			s = new PeopleView();

			switch (Integer.parseInt(array[0])) {
			case 0:
				convertView = mInflater.inflate(R.layout.listview_item_received, null);
				break;
			case 1:
				convertView = mInflater.inflate(R.layout.listview_item_send, null);
				break;
			}

			s.time = (TextView) convertView.findViewById(R.id.Time);
			s.message = (TextView) convertView.findViewById(R.id.Msg);
			s.portrait = (ImageView) convertView.findViewById(R.id.Img);
			convertView.setTag(s);
		}
			//s = (PeopleView) convertView.getTag();
		s.time.setText(array[2]+" "+new SimpleDateFormat("MM-dd"+" HH:"+"mm").format((new Date().getTime())));
		s.message.setText(array[1]);
		s.portrait.setImageResource(R.drawable.ic_launcher);
		//setListViewHeightBasedOnChildren(convertView);
		return convertView;

	}
	
	public static void setListViewHeightBasedOnChildren(ListView listView) { 
        ListAdapter listAdapter = listView.getAdapter();  
        if (listAdapter == null) { 
            // pre-condition 
            return; 
        } 
 
        int totalHeight = 0; 
        for (int i = 0; i < listAdapter.getCount(); i++) { 
            View listItem = listAdapter.getView(i, null, listView); 
            listItem.measure(0, 0); 
            totalHeight += listItem.getMeasuredHeight(); 
        } 
 
        ViewGroup.LayoutParams params = listView.getLayoutParams(); 
        params.height = totalHeight + (listView.getDividerHeight() * (listAdapter.getCount() - 1)); 
        listView.setLayoutParams(params); 
    } 
	
	public void addList(String content){
		contents.add(content);
		notifyDataSetChanged();
	}
	
	class PeopleView {
		TextView time;
		TextView message;
		ImageView portrait;
	}

}
