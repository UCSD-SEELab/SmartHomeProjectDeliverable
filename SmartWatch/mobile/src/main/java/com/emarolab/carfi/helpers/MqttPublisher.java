package com.emarolab.carfi.helpers;

import android.content.Context;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MqttPublisher implements Runnable {

    private MqttAndroidClient mqttAndroidClient;
    private String msg = "casa";
    private String topic = "";
    private String topicMsg = "";
    private boolean published = false;
    private boolean publishedTopic = false;

    public void setTopicAndMsg(final String topic, final String msg) {
        this.topic = topic;
        this.topicMsg = msg;
        publishedTopic = true;
    }

    public void setMsg(final String msg) {
        this.msg = msg;
        published = true;
    }

    private Context context;

    public MqttPublisher(MqttAndroidClient mqttAndroidClient, Context context) {
        this.mqttAndroidClient = mqttAndroidClient;
    }
    public MqttPublisher(MqttAndroidClient mqttAndroidClient, Context context, String msg) {
        this.mqttAndroidClient = mqttAndroidClient;
        this.msg = msg;
    }

    @Override
    public synchronized void run() {

        while (true) {
            if(published) {
                MqttMessage mqttMessage = new MqttMessage();
                String m = msg;
                mqttMessage.setPayload(m.getBytes());

                try {
                    mqttAndroidClient.publish("watch", mqttMessage);
                } catch (MqttException e) {
                    e.printStackTrace();
                }

                published = false;
            }
            if(publishedTopic) {
                MqttMessage mqttMessage = new MqttMessage();
                String m = topicMsg;
                mqttMessage.setPayload(m.getBytes());

                try {
                    mqttAndroidClient.publish(topic, mqttMessage);
                } catch (MqttException e) {
                    e.printStackTrace();
                }

                publishedTopic = false;
            }
        }
    }

}