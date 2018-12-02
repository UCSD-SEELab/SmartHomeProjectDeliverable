/* This code is based on the example available at
    https://github.com/joelwass/Android-BLE-Scan-Example
    modified by
    Rishikanth Chandrasekaran
    SEE Lab
    Universify of California, San Diego
 */

package com.example.joelwasserman.androidbletutorial;

import android.Manifest;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Optional;
import java.util.OptionalDouble;

import edu.ucsd.seelab.HierarchicalLearning.NeuralNetwork;
import edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig;
import edu.ucsd.seelab.HierarchicalLearning.ShapeAndWeights;

import static android.bluetooth.le.ScanSettings.MATCH_NUM_ONE_ADVERTISEMENT;
import static android.bluetooth.le.ScanSettings.SCAN_MODE_LOW_POWER;
import static edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig.MQTTPort;
import static edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig.MQTTServer;

public class MainActivity extends AppCompatActivity {
    BluetoothManager btManager;
    BluetoothAdapter btAdapter;
    BluetoothLeScanner btScanner;
    Button startScanningButton;
    Button sendRawButton;
    boolean sendingRaw = false;
    Button stopScanningButton;
    boolean stat = true;
    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    int r4 = 0;
    int r5 = 0;
    int r6 = 0;
    int r1_count = 1;
    int r2_count = 1;
    int r3_count = 1;
    int r4_count = 1;
    int r5_count = 1;
    int r6_count = 1;
    TextView peripheralTextView;
    String topic = "crk";
    MqttAndroidClient client;
    private final static int REQUEST_ENABLE_BT = 1;
    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;
    Handler handler = new Handler();
    int delay = 5000; //milliseconds

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        peripheralTextView = (TextView) findViewById(R.id.PeripheralTextView);
        peripheralTextView.setMovementMethod(new ScrollingMovementMethod());

        sendRawButton = (Button) findViewById(R.id.SendRawButton);
        sendRawButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                sendingRaw=!sendingRaw;
                if (sendingRaw)
                    sendRawButton.setText("Stop Send Raw");
                else
                    sendRawButton.setText("Send Raw");
            }
        });


        startScanningButton = (Button) findViewById(R.id.StartScanButton);
        startScanningButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                startScanning();
            }
        });

        stopScanningButton = (Button) findViewById(R.id.StopScanButton);
        stopScanningButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                stopScanning();
            }
        });
        stopScanningButton.setVisibility(View.INVISIBLE);

        btManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
        btAdapter = btManager.getAdapter();
        btScanner = btAdapter.getBluetoothLeScanner();


        if (btAdapter != null && !btAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent,REQUEST_ENABLE_BT);
        }

        // Make sure we have access coarse location enabled, if not, prompt the user to enable it
        if (this.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            final AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("This app needs location access");
            builder.setMessage("Please grant location access so this app can detect peripherals.");
            builder.setPositiveButton(android.R.string.ok, null);
            builder.setOnDismissListener(new DialogInterface.OnDismissListener() {
                @Override
                public void onDismiss(DialogInterface dialog) {
                    requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
                }
            });
            builder.show();
        }

        //MQTT - connect to broker
        String clientId = MqttClient.generateClientId();
        client =  new MqttAndroidClient(this.getApplicationContext(), "tcp://"+MQTTServer+":"+MQTTPort, clientId);
        MqttConnectOptions options = new MqttConnectOptions();
        options.setUserName("admin");
        options.setPassword(SamsungProjectConfig.MQTTPassword.toCharArray());
        try {
            IMqttToken token = client.connect(options);
//            IMqttToken token = client.connect();
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    // We are connected
                    peripheralTextView.append("connected !!");
                    Log.d("debugs", "onSuccess");
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    // Something went wrong e.g. connection timeout or firewall problems
                    peripheralTextView.append("Failure !!");
                    Log.d("debugs", "onFailure");

                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void sendmqtt(String topic, String msg) {
        String payload = msg;
        byte[] encodedPayload = new byte[0];

        try {
            encodedPayload = payload.getBytes("UTF-8");
            MqttMessage message = new MqttMessage(encodedPayload);
            client.publish(topic, message);
        } catch (UnsupportedEncodingException | MqttException e) {
            e.printStackTrace();
        }
    }

    public void sendmqtt(String rssi_msg) {
        String payload = rssi_msg;
        String topic = "crk";
        byte[] encodedPayload = new byte[0];

                try {
                    encodedPayload = payload.getBytes("UTF-8");
                    MqttMessage message = new MqttMessage(encodedPayload);
                    client.publish(topic, message);
                } catch (UnsupportedEncodingException | MqttException e) {
                    e.printStackTrace();
                }
    }
    private OptionalDouble NormalizedRSSI(double ... rssi) {
        return  Arrays.stream(rssi).filter(d -> d!=0).max();
    }
    private double[] ExtractFeature(OptionalDouble[] in) {
        Optional<OptionalDouble> max = Arrays.stream(in).max(new Comparator<OptionalDouble>() {
            @Override
            public int compare(OptionalDouble o1, OptionalDouble o2) {
                if(!o1.isPresent() && !o2.isPresent()) return 0;
                if(!o1.isPresent()) return -1;
                if(!o2.isPresent()) return 1;
                return Double.compare(o1.getAsDouble(),o2.getAsDouble());
            }
        });
        return Arrays.stream(in).mapToDouble(val -> val.isPresent() && max.isPresent() && val.equals(max.get())? 1.0 : 0.0).toArray();
    }
    // Device scan callback.
    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {

            Log.d("debugs", "enteringgggg");
            if (result.getDevice().getAddress().equals("C9:C0:78:1C:21:D4")) {
                r1 += result.getRssi();
                r1_count += 1;
//                peripheralTextView.append("kitchen Yellow: " + r1 + "\n");
//                sendmqtt("" + r1, 0);
            }
            if (result.getDevice().getAddress().equals("E3:6C:2B:DB:7B:ED")) {
                r2 += result.getRssi();
                r2_count +=1;
//                peripheralTextView.append("kitchen Pink: " + r2 + "\n");
            }
            if (result.getDevice().getAddress().equals("FC:2D:91:D2:06:65")) {
                r3 = result.getRssi();
                r3_count += 1;
//                peripheralTextView.append("Dining red: " + r3 + "\n");
            }

            if (result.getDevice().getAddress().equals("FB:EC:0E:C5:58:14")) {
                r4 = result.getRssi();
                r4_count += 1;
//                peripheralTextView.append("Living red: " + r4 + "\n");
            }

            if (result.getDevice().getAddress().equals("E7:CC:C7:6C:91:76")) {
                r5 = result.getRssi();
                r5_count += 1;
//                peripheralTextView.append("Living pink: " + r5 + "\n");
            }
            //Avoid 0s
            OptionalDouble[] input = new OptionalDouble[] {
                    NormalizedRSSI(r1,r2), //kitchen
                    NormalizedRSSI(r4,r5), //living room
                    NormalizedRSSI(r3) //dining room
            };
            //Extract feature
            String json = NeuralNetwork.Loop(System.currentTimeMillis()/1000L,
                    ShapeAndWeights.N_IN,
                    ExtractFeature(input));
            sendmqtt("location/nnjson", json);
            if (sendingRaw) {
                String mqtt_rssi = "{rssi1: " + r1 / r1_count + ", rssi2: " + r2 / r2_count + ", rssi3: " + r3 / r3_count + ", rssi4: " + r4 / r4_count + ", rssi5: " + r5 / r5_count + "}";
                sendmqtt(mqtt_rssi);
            }
//            if (result.getDevice().getAddress() == "C9:C0:78:1C:21:D4" || result.getDevice().getAddress() == "FB:EC:0E:C5:58:14" ||
//                    result.getDevice().getAddress() == "E7:CC:C7:6C:91:76" || result.getDevice().getAddress() == "D0:98:64:F6:2D:D5") {
//                String payload = "{rssi1:" + result.getRssi() + "}";
//                byte[] encodedPayload = new byte[0];
//
//                try {
//                    encodedPayload = payload.getBytes("UTF-8");
//                    MqttMessage message = new MqttMessage(encodedPayload);
//                    client.publish(topic, message);
//                } catch (UnsupportedEncodingException | MqttException e) {
//                    e.printStackTrace();
//                }
//            }

/*
            peripheralTextView.append("Device Name: " + result.getDevice().getName() +
                    " MAC: " + result.getDevice().getAddress() +
                    " rssi: " + result.getRssi() + "\n");
*/

            // auto scroll for text view
            final int scrollAmount = peripheralTextView.getLayout().getLineTop(peripheralTextView.getLineCount()) - peripheralTextView.getHeight();
            // if there is no need to scroll, scrollAmount will be <=0
            if (scrollAmount > 0)
                peripheralTextView.scrollTo(0, scrollAmount);
        }
    };

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_COARSE_LOCATION: {
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    System.out.println("coarse location permission granted");
                } else {
                    final AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setTitle("Functionality limited");
                    builder.setMessage("Since location access has not been granted, this app will not be able to discover beacons when in the background.");
                    builder.setPositiveButton(android.R.string.ok, null);
                    builder.setOnDismissListener(new DialogInterface.OnDismissListener() {

                        @Override
                        public void onDismiss(DialogInterface dialog) {
                        }

                    });
                    builder.show();
                }
                return;
            }
        }
    }

    public void startScanning() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        System.out.println("start scanning");
        peripheralTextView.setText("");
        startScanningButton.setVisibility(View.INVISIBLE);
        stopScanningButton.setVisibility(View.VISIBLE);

        ScanSettings.Builder settingBuilder = new ScanSettings.Builder();
        settingBuilder.setScanMode(SCAN_MODE_LOW_POWER | MATCH_NUM_ONE_ADVERTISEMENT);
        final ScanSettings settings = settingBuilder.build();

        ScanFilter filter1 = new ScanFilter.Builder().setDeviceAddress("C9:C0:78:1C:21:D4").build();
        ScanFilter filter2 = new ScanFilter.Builder().setDeviceAddress("E3:6C:2B:DB:7B:ED").build();
        ScanFilter filter3 = new ScanFilter.Builder().setDeviceAddress("FC:2D:91:D2:06:65").build();
        ScanFilter filter4 = new ScanFilter.Builder().setDeviceAddress("FB:EC:0E:C5:58:14").build();
        ScanFilter filter5 = new ScanFilter.Builder().setDeviceAddress("E7:CC:C7:6C:91:76").build();
        ScanFilter filter6 = new ScanFilter.Builder().setDeviceAddress("D0:98:64:F6:2D:D5").build();
        final List<ScanFilter> filters =  new ArrayList<>();
        filters.add(filter1);
        filters.add(filter2);
        filters.add(filter3);
        filters.add(filter4);
        filters.add(filter5);
//        filters.add(filter6);


//        btScanner.startScan(leScanCallback);
        handler.postDelayed(new Runnable(){
            public void run(){
                peripheralTextView.append("Scanning \n");
                //do something
                if (stat) {
                    stat = false;
                    AsyncTask.execute(new Runnable() {
                        @Override
                        public void run() {
                            btScanner.startScan(filters,settings,leScanCallback);
                        }
                    });
//                    btScanner.startScan(leScanCallback);

                } else {
                    stat = true;
                    AsyncTask.execute(new Runnable() {
                        @Override
                        public void run() {
                            btScanner.stopScan(leScanCallback);
                        }
                    });
                    r1 = r2 = r3 = r4 = r5 = r6 = 0;
                    r1_count = r2_count = r3_count = r4_count = r5_count = r6_count = 1;
                }

                handler.postDelayed(this, delay);
            }
        }, delay);


//        AsyncTask.execute(new Runnable() {
//            @Override
//            public void run() {
//                btScanner.startScan(filters,settings,leScanCallback);
//            }
//        });
    }

    public void stopScanning() {
        System.out.println("stopping scanning");
        peripheralTextView.append("Stopped Scanning");
        startScanningButton.setVisibility(View.VISIBLE);
        stopScanningButton.setVisibility(View.INVISIBLE);
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                btScanner.stopScan(leScanCallback);
            }
        });

        handler.removeCallbacksAndMessages(null);
    }
}
