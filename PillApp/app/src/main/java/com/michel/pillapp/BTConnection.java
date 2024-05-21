package com.michel.pillapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

public class BTConnection {

    private static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private static BluetoothSocket BTSocket = null;
    private BluetoothDevice hc05 = null;
    private String deviceAddress;

    public BTConnection (String deviceAddress){
        this.deviceAddress = deviceAddress;
    }

    //Método que envia mensajes al dispositivo
    public static boolean sendMessage(byte[] value){
        try {
            OutputStream os = BTSocket.getOutputStream();
            os.write(value);
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    //Metodo para desconectar al usuario
    public boolean disconnect(){
        try{
            BTSocket.close();
            return true;
        } catch(Exception e){
            e.printStackTrace();
        }
        return false;
    }

    //Metodo que se realiza ccuando el usuario clicka en Connect
    public boolean makeConnection(){
        try {
            BluetoothAdapter BTAdapter = BluetoothAdapter.getDefaultAdapter(); //Creamos nuestro adaptador bluetooth
            hc05 = BTAdapter.getRemoteDevice(this.deviceAddress); //Nos conectamos directamente con SPIDER, a traves de su address

            if(connect() == true){
                return true;
            } else {
                return false;
            }

        } catch(Exception e){
            e.printStackTrace();
        }
        return false;
    }

    //Método para conectar con el dispositivo
    private boolean connect(){
        int counter = 0;
        do { //Tratamos 3 veces de establecer conexion con nuestro dispositivo
            try {
                BTSocket = hc05.createRfcommSocketToServiceRecord(mUUID); //Establecemos conexión
                BTSocket.connect();
                return true;

            } catch (IOException e) {
                e.printStackTrace();
            }
            counter ++;
        } while(!BTSocket.isConnected() && counter < 3);
        return false;
    }

}
