/* Copyright (C) 2010 MoSync AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

package com.mosync.java.android;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * Service used to give MoSync applications higher priority and display
 * a notification icon that can be used to launch the application.
 * @author Mikael Kindborg
 */
public class MoSyncService extends Service
{
	// TODO: Remove commented out code.
	//static boolean sIsRunning = true;
	/*public static void stopService()
	{
		sIsRunning = false;
	}*/
	
	// Since we just have one service, we can cheat and 
	// make these global.
	public static int sNotificationId;
	static String sNotificationTitle;
	static String sNotificationText;
	
	// Instance of this service.
	static MoSyncService sMe;
	
	public static void startService(
		Context context, 
		int notificationId,
		String notificationTitle, 
		String notificationText)
	{
		Log.i("@@@MoSync", "MoSyncService.startService");
		
		// There should be no running service.
		if (null != sMe)
		{
			Log.i("@@@MoSync", "MoSyncService.startService - service is already running, returning");
			return;
		}
		
		sNotificationId = notificationId;
		sNotificationTitle = notificationTitle;
		sNotificationText = notificationText;
		
		Intent serviceIntent = new Intent(context, MoSyncService.class);
		context.startService(serviceIntent);
	}
	
	public static void stopService()
	{
		Log.i("@@@MoSync", "MoSyncService.stopService");
		
		if (null != sMe)
		{
			Log.i("@@@MoSync", "MoSyncService.stopService - stopSelf");
			sMe.stopSelf();
			sMe = null;
		}
	}
	
	public static void removeServceNotification(
		int notificationId, Activity activity)
	{
		// We use a wrapper class to be backwards compatible.
		// Loading the wrapper class will throw an error on
		// platforms that does not support it.
		try
		{
			new StartForegroundWrapper();
			// Nothing special needs to be done here, the notification
			// will be removed when the service is stopped.
		}
		catch (java.lang.VerifyError error)
		{
			// We are below API level 5, and need to remove the 
			// notification manually.
			NotificationManager mNotificationManager = (NotificationManager) 
				activity.getSystemService(Context.NOTIFICATION_SERVICE);
			mNotificationManager.cancel(notificationId);
		}
	}
	
	@Override
	public IBinder onBind(Intent intent) 
	{
		return null;
	}
	
	/**
	 * Start service in foreground with notification icon.
	 * Start foreground is only available on Android API level 5 and above.
	 */
	@Override
	public void onCreate()
	{
		super.onCreate();
		
		Log.i("@@@MoSync", "MoSyncService.onCreate sMe: " + sMe);
		
		sMe = this;
		
		// Get icon.
		int icon = getResources().getIdentifier(
			"icon", 
			"drawable", 
			getPackageName());
		Log.i("@@@MoSync", "MoSyncService.onCreate icon: " + icon);
		
		// TODO: Replace with app name.
		CharSequence tickerText = sNotificationTitle;
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);
		
		Context context = getApplicationContext();
		CharSequence contentTitle = sNotificationTitle;
		CharSequence contentText = sNotificationText;
		//Intent intent = new Intent("mosync.dynamicwebview.DISPLAY");
		Intent intent = new Intent(context, MoSync.class);
		intent.addFlags(
			Intent.FLAG_ACTIVITY_NEW_TASK | 
			Intent.FLAG_ACTIVITY_REORDER_TO_FRONT |
			Intent.FLAG_ACTIVITY_SINGLE_TOP |
			Intent.FLAG_DEBUG_LOG_RESOLUTION |
			0);
		PendingIntent contentIntent = PendingIntent.getActivity(
			context, 
			0, 
			intent,
			0
		    );
		notification.setLatestEventInfo(
			context, 
			contentTitle,
			contentText, 
			contentIntent);
		// TODO: Is this needed?
		notification.flags |= Notification.FLAG_ONGOING_EVENT;

		// We use a wrapper class to be backwards compatible.
		// Loading the wrapper class will throw an error on
		// platforms that does not support it.
		try
		{
			// Start as foreground service on Android >= 5. 
			// This displays the notification.
			new StartForegroundWrapper().startForeground(
				this, sNotificationId, notification);
		}
		catch (java.lang.VerifyError error)
		{
			// Just add the notification on Android < 5.
			NotificationManager notificationManager = (NotificationManager)
				getSystemService(Context.NOTIFICATION_SERVICE);
			notificationManager.notify(sNotificationId, notification);
		}
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		
		Log.i("@@@MoSync", "MoSyncService.onDestory");
		
		stopService();
	}
}

/**
 * Wrapper class for startForeground, which is not available on
 * Android versions below level 5.
 * @author Mikael Kindborg
 */
class StartForegroundWrapper
{
	public void startForeground(
		Service service, int id, Notification notification)
	{
		service.startForeground(id, notification);
	}
}
