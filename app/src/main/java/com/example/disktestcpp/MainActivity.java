package com.example.disktestcpp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.disktestcpp.databinding.ActivityMainBinding;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'disktestcpp' library on application startup.
    static {
        System.loadLibrary("disktestcpp");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        //File cacheDir = this.getCacheDir();
        File cacheDir = this.getExternalCacheDir();
        String cacheDirPath = cacheDir.getAbsolutePath();

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI(cacheDirPath));
    }

    /**
     * A native method that is implemented by the 'disktestcpp' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String dirPath);
}