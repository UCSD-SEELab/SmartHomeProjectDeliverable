package com.emarolab.carfi.imustream;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig;

import static edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig.*;

public class MqttSettingActivity extends AppCompatActivity {
    public static final String ip_message = "com.example.androidweartest.ip";
    public static final String port_message = "com.example.androidweartest.port";
    public static final String user_message = "com.example.androidweartest.user";
    public static final String password_message = "com.example.androidweartest.password";

    public static final String port_default = String.valueOf(MQTTPort);
    public static final String user_default = MQTTUser;
    public static final String password_default = MQTTPassword;
    private EditText editTextIP;
    private EditText editTextPort;
    private EditText editTextUser;
    private EditText editTextPass;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mqtt_setting);
        editTextIP = (EditText) findViewById(R.id.IP);
        editTextPort = (EditText) findViewById(R.id.port);
        editTextUser = (EditText) findViewById(R.id.userID);
        editTextPass = (EditText) findViewById(R.id.userPSW);
        if(editTextIP.getText().toString().matches("")) editTextIP.setText(MQTTServer);
        if(editTextPort.getText().toString().matches("")) editTextPort.setText(String.valueOf(MQTTPort));
        if(editTextUser.getText().toString().matches("")) editTextUser.setText(MQTTUser);
        if(editTextPass.getText().toString().matches("")) editTextPass.setText(MQTTPassword);
    }

    public void openPcComunication(View view) {
        Intent intent = new Intent(this, PcComunicationActivity.class);

        String mqtt_ip = editTextIP.getText().toString();
        if (mqtt_ip.matches("")){
            Toast.makeText(this, "You did not enter an IP address", Toast.LENGTH_SHORT).show();
            return;
        }

        String mqtt_port = editTextPort.getText().toString();
        if (mqtt_port.matches("")){
            mqtt_port = port_default;
        }

        String mqtt_user = editTextUser.getText().toString();
        if (mqtt_user.matches("")){
            mqtt_user = user_default;
        }

        String mqtt_password = editTextPass.getText().toString();
        if (mqtt_password.matches("")){
            mqtt_password = password_default;
        }

        intent.putExtra(ip_message, mqtt_ip);
        intent.putExtra(port_message, mqtt_port);
        intent.putExtra(user_message, mqtt_user);
        intent.putExtra(password_message, mqtt_password);

        startActivity(intent);
    }
}
