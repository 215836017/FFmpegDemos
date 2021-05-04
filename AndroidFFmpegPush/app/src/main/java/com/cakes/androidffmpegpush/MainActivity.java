package com.cakes.androidffmpegpush;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "MainActivity";

    /*** 要推送的目的地址 */
    private final String DEST_URL = "";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        findViewById(R.id.btn_push_video).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startPushVideo();
            }
        });
    }

    private void startPushVideo() {
        String dir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/testFFmpeg/";
        File dirFile = new File(dir);
        if (null != dirFile) {
            if (!dirFile.exists()) {
                dirFile.mkdirs();
            }
        }

        LogUtil.d(TAG, "path = " + dir + "srcVideo.mp4");
        File srcFile = new File(dir + "srcVideo.mp4");
        if (null == srcFile || !srcFile.exists()) {
            showToast("源文件不存在或文件错误");
            return;
        }

        LogUtil.d(TAG, "srcFile.getAbsolutePath() = " + srcFile.getAbsolutePath());
        showToast("开始推送了...");
        pushVideo(srcFile.getAbsolutePath(), DEST_URL);
    }

    private void showToast(String msg) {
        LogUtil.i(TAG, msg);
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    private native int pushVideo(String srcPath, String destUrl);
}