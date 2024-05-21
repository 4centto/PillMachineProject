package com.michel.pillapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

public class Home extends AppCompatActivity {

    private BTConnection connection;
    private Button btn_conectar;
    private ProgressBar progress;
    private boolean isConnected = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);

        connection = new BTConnection("98:D3:41:F6:4B:C8");

        btn_conectar = (Button) findViewById(R.id.btn_conectar);
        progress = (ProgressBar) findViewById(R.id.progress);

        progress.setVisibility(View.INVISIBLE);

        btn_conectar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(isConnected == false){
                    Toast.makeText(Home.this, "Connecting...", Toast.LENGTH_SHORT).show();
                    btn_conectar.setVisibility(View.INVISIBLE);
                    progress.setVisibility(View.VISIBLE);

                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            if(connection.makeConnection()){
                                Toast.makeText(Home.this, "Connection successfuly", Toast.LENGTH_SHORT).show();
                                isConnected = true;

                                Intent i = new Intent(Home.this, Calendar.class);
                                startActivity(i);
                                finish();

                            } else {
                                btn_conectar.setVisibility(View.VISIBLE);
                                progress.setVisibility(View.INVISIBLE);
                                Toast.makeText(Home.this, "Connection failed", Toast.LENGTH_SHORT).show();
                            }
                        }
                    }, 500);

                }

            }
        });

    }
}