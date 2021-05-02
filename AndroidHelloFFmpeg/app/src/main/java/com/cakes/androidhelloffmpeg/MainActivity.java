package com.cakes.androidhelloffmpeg;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.method.MovementMethod;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {

        /*
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");

        动态库没有load也可以运行
        */
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        TextView textInfo = findViewById(R.id.text_info);
        textInfo.setMovementMethod(ScrollingMovementMethod.getInstance());

        findViewById(R.id.btn_config).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textInfo.setText(getConfig());
            }
        });
        findViewById(R.id.bnt_protocol).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textInfo.setText(getProtocol());
            }
        });
        findViewById(R.id.btn_format).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textInfo.setText(getFormat());
            }
        });
        findViewById(R.id.btn_codec).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textInfo.setText(getCodec());
            }
        });
        findViewById(R.id.btn_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showToast("开始获取Filter");
                textInfo.setText(getFilter());
            }
        });
    }

    private void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native String getConfig();

    public native String getProtocol();

    public native String getFormat();

    public native String getCodec();

    public native String getFilter();
}