package com.emarolab.carfi.imustream;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.emarolab.carfi.helpers.MqttHelper;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONArray;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.text.DateFormat;
import java.util.Timer;

import edu.ucsd.seelab.HierarchicalLearning.FilterWatchData;
import edu.ucsd.seelab.HierarchicalLearning.NeuralNetwork;
import edu.ucsd.seelab.HierarchicalLearning.ShapeAndWeights;

public class PcComunicationActivity extends AppCompatActivity {
    public TextView Step, HRM, TS, TS2,TS3, ipOut, portOut, textConnection;
    private Button p1_button;
    private Button p2_button;
    private boolean pause_flag = true;
    private boolean pauseNN_flag = false;
    private BroadcastReceiver receiver;
    private Timer myTimer;
    private MqttHelper mqttHelper;
    private DateFormat dateFormat = new SimpleDateFormat("MM/dd HH:mm:ss:SSS");
    private long driftStart_phone = 0;
    private long driftStart_watch = 0;
    private final int intervalMilliseconds = 3000;
    private long lastMessageMilliseconds = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pc_comunication);

        p1_button = (Button)findViewById(R.id.buttonPause);
        p2_button = (Button)findViewById(R.id.buttonPauseNN);

        Intent intent = getIntent();
        String mqtt_ip = intent.getStringExtra(MqttSettingActivity.ip_message);
        String mqtt_port = intent.getStringExtra(MqttSettingActivity.port_message);
        String mqtt_user = intent.getStringExtra(MqttSettingActivity.user_message);
        String mqtt_password = intent.getStringExtra(MqttSettingActivity.password_message);

        ipOut = (TextView) findViewById(R.id.ipSending);
        portOut = (TextView) findViewById(R.id.portSending);
        ipOut.setText(mqtt_ip);
        portOut.setText(mqtt_port);
        textConnection = (TextView) findViewById(R.id.connectionS);

        Step = (TextView) findViewById(R.id.step);
        HRM = (TextView) findViewById(R.id.hrm);
        TS = (TextView) findViewById(R.id.ts);
        TS2 = (TextView) findViewById(R.id.ts2);
        TS3 = (TextView) findViewById(R.id.ts3);

        startMqtt(mqtt_ip,mqtt_port,mqtt_user,mqtt_password);

        IntentFilter filter = new IntentFilter();
        filter.addAction("com.example.Broadcast");

        FilterWatchData filterWatchData = new FilterWatchData(32);//needs to be power of 2


        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Bundle bundle = intent.getExtras();
                if (bundle != null) {
                    float[] step = bundle.getFloatArray("stepcount");
                    float[] hrm = bundle.getFloatArray("heartrate");
                    float[] acc = bundle.getFloatArray("accelerometer");
                    float[] gyro = bundle.getFloatArray("gyroscope");
                    long time = bundle.getLong("timestamp");
                    long tmp = bundle.getLong("driftStart");
                    if (tmp!=0){
                        driftStart_phone = System.currentTimeMillis();
                        driftStart_watch = tmp;
                    }
                    //TODO: Add NN and send results here
                    double[] in_raw = new double[] {
                            step[0],
                            hrm[0],
                            acc[0],
                            acc[1],
                            acc[2],
                            gyro[0],
                            gyro[1],
                            gyro[2]
                    };
                    filterWatchData.pushData(acc, gyro);
                    long timestamp = System.currentTimeMillis();
                    if (timestamp-lastMessageMilliseconds>=intervalMilliseconds) {
                        lastMessageMilliseconds = timestamp;
                        double[] inputs = filterWatchData.GenerateNNInput();
                        String json = NeuralNetwork.Loop(timestamp / 1000L,
                                ShapeAndWeights.N_IN, inputs);
                        mqttHelper.onDataReceived("watch/nnjson",json);
                    }
                    String string = "step;" + step[0] + ";hrm;" + hrm[0] +";acc;"+acc[0]+";"+acc[1]+";"+acc[2]+";gyro;"+gyro[0]+";"+gyro[1]+";"+gyro[2]+";ts;"+dateFormat.format(new Date(time));
                    imuVisualization(step,hrm,time,driftStart_watch,driftStart_phone);
                    mqttHelper.onDataReceived(string);
                    connectionCheck();
                }
            }
        };

        registerReceiver(receiver, filter);
    }

    private void connectionCheck(){
        boolean connection = mqttHelper.checkConnection();
        if(connection){
            textConnection.setText("connected");
            textConnection.setTextColor(Color.GREEN);
        }else{
            textConnection.setText("disconnected");
            textConnection.setTextColor(Color.RED);
        }
    }

    private void imuVisualization(float[] step, float[] hrm, long ts,long driftStart_watch, long driftStart_phone){
        Step.setText(String.valueOf(step[0]));
        HRM.setText(String.valueOf(hrm[0]));
        TS.setText(dateFormat.format(new Date(ts)));
        TS2.setText(Long.toString(driftStart_watch));
        TS3.setText(Long.toString(driftStart_phone));

    }

    @Override
    protected void onDestroy() {
        if (receiver != null) {
            unregisterReceiver(receiver);
            receiver = null;
        }
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        if (receiver != null) {
            unregisterReceiver(receiver);
            receiver = null;
        }
        mqttHelper.closeClient();
        super.onBackPressed();
    }

    public void pauseNN(View view){
        mqttHelper.setPublishNNPermission(pauseNN_flag);
        pauseNN_flag = !pauseNN_flag;

        if(pauseNN_flag){
            p1_button.setText("Resume NN");
        }else{
            p1_button.setText("Pause NN");
        }
    }

    public void pause(View view){
        mqttHelper.setPublishPermission(pause_flag);
        pause_flag = !pause_flag;

        if(pause_flag){
            p1_button.setText("Resume Raw");
        }else{
            p1_button.setText("Pause Raw");
        }
    }

    private void startMqtt(String ip, String port, String user, String password){
        mqttHelper = new MqttHelper(getApplicationContext(),ip,port,user,password);
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {

            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

            }
        });
    }
}
