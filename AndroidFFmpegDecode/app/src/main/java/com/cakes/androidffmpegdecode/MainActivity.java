package com.cakes.androidffmpegdecode;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "MainActivity";

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

        findViewById(R.id.btn_decode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                decodeVideo();
            }
        });
    }

    private void decodeVideo() {
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

        File destFile = new File(dir + "destVideo.yuv");
        if (null != destFile) {
            if (destFile.exists()) {
                destFile.delete();
            }

            try {
                destFile.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        LogUtil.d(TAG, "decodeVideo() -- srcPath = " + srcFile.getAbsolutePath()
                + ", destPath = " + destFile.getAbsolutePath());
        showToast("开始解码...");
        startDecode(srcFile.getAbsolutePath(), destFile.getAbsolutePath());
        showToast("解码完成了！");
    }

    private void showToast(String msg) {
        LogUtil.i(TAG, msg);
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    private native String stringFromJNI();

    private native int startDecode(String srcPath, String destPath);

}