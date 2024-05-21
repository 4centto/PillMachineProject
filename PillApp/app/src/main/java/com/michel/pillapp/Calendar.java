package com.michel.pillapp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TimePicker;
import android.widget.Toast;

import java.nio.charset.Charset;
import java.util.Arrays;

public class Calendar extends AppCompatActivity {

    private Spinner spinner_dias, spinner_pastillas;
    private EditText et_cantidad;
    private TimePicker timePicker;
    private Button btn_guardar;

    private String dia = "", pastilla = "";
    private int cantidad = 0, hora = 0, minutos = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_calendar);

        spinner_dias = (Spinner) findViewById(R.id.spn_dias);
        spinner_pastillas = (Spinner) findViewById(R.id.spn_pastillas);
        et_cantidad = (EditText) findViewById(R.id.et_cant);
        btn_guardar = (Button) findViewById(R.id.btn_guardar);
        timePicker = (TimePicker) findViewById(R.id.timePicker);

        ArrayAdapter<String> adapter_dias = new ArrayAdapter<>(this, R.layout.custom_spinner, getResources().getStringArray(R.array.dias));
        adapter_dias.setDropDownViewResource(R.layout.custom_item_spinner);

        ArrayAdapter<String> adapter_pastillas = new ArrayAdapter<>(this, R.layout.custom_spinner, getResources().getStringArray(R.array.pastillas));
        adapter_pastillas.setDropDownViewResource(R.layout.custom_item_spinner);

        spinner_dias.setAdapter(adapter_dias);
        spinner_pastillas.setAdapter(adapter_pastillas);

        btn_guardar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String cant = et_cantidad.getText().toString();
                if (cant.compareToIgnoreCase("") != 0) {
                    cantidad = Integer.parseInt(et_cantidad.getText().toString());
                    if (cantidad >= 1) {
                        if (cantidad <= 5) {
                            dia = castDay(spinner_dias.getSelectedItem().toString());
                            pastilla = castPill(spinner_pastillas.getSelectedItem().toString());
                            cantidad = Integer.parseInt(et_cantidad.getText().toString());
                            hora = timePicker.getHour();
                            minutos = timePicker.getMinute();

                            String chain = dia + "#" + pastilla + "#" + cantidad + "#" + hora + "#" + minutos;
                            byte[] message = chain.getBytes(Charset.forName("UTF-8"));

                            if(BTConnection.sendMessage(message)){
                                Toast.makeText(Calendar.this, "Horario enviado!", Toast.LENGTH_SHORT).show();
                            } else {
                                Toast.makeText(Calendar.this, "Error", Toast.LENGTH_SHORT).show();
                            }

                        } else {
                            Toast.makeText(Calendar.this, "No se puede dar mas de 5 pastillas", Toast.LENGTH_SHORT).show();
                        }
                    } else {
                        Toast.makeText(Calendar.this, "Debe haber al menos 1 pastilla", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    Toast.makeText(Calendar.this, "Debe haber al menos 1 pastilla", Toast.LENGTH_SHORT).show();
                }
            }

        });

    }

    private String castDay(String day){
        switch(day){
            case "Lunes":
                return "1";
            case "Martes":
                return "2";
            case "Miercoles":
                return "3";
            case "Jueves":
                return "4";
            case "Viernes":
                return "5";
            case "Sabado":
                return "6";
            case "Domingo":
                return "7";
            default:
                return "";
        }
    }

    private String castPill(String pill){
        switch(pill){
            case "Pastilla 1":
                return "1";
            case "Pastilla 2":
                return "2";
            case "Pastilla 3":
                return "3";
            case "Pastilla 4":
                return "4";
            default:
                return "0";
        }
    }

}