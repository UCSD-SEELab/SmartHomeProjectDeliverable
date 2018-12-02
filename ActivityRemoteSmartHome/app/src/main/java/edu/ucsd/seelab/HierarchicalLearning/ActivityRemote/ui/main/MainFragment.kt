/*
    BSD 3-Clause License

    Copyright (c) 2018, The Regents of the University of California.
    All rights reserved.

    Created by Massimiliano Menarini
    University of California, San Diego
    Department of Computer Science and Engineering

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package edu.ucsd.seelab.HierarchicalLearning.ActivityRemote.ui.main

import android.arch.lifecycle.ViewModelProviders
import android.os.Bundle
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ToggleButton
import edu.ucsd.seelab.HierarchicalLearning.ActivityRemote.R
import edu.ucsd.seelab.HierarchicalLearning.SamsungProjectConfig
import kotlinx.android.synthetic.main.main_fragment.*
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*
import org.json.JSONObject
import java.time.LocalDateTime
import java.util.logging.Logger

class MainFragment : Fragment(), MqttCallback {
    override fun messageArrived(topic: String?, message: MqttMessage?) {
        Logger.getLogger(MainFragment::class.java.name).warning("messageArrived: "+topic)
        //TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun connectionLost(cause: Throwable?) {
        Logger.getLogger(MainFragment::class.java.name).warning("connectionLost: "+cause.toString())
        //TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    override fun deliveryComplete(token: IMqttDeliveryToken?) {
        Logger.getLogger(MainFragment::class.java.name).warning("deliveryComplete: "+token.toString())
        //TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    companion object {
        fun newInstance() = MainFragment()
    }

    private var connected = false
    private lateinit var viewModel: MainViewModel
    private lateinit var mqtt : MqttAndroidClient

    private fun connect(URI: String) {
        if(!::mqtt.isInitialized) {
            val clientId = MqttClient.generateClientId()
            mqtt = MqttAndroidClient(
                context, URI,
                clientId)
            mqtt.setCallback(this)
        }
        if(mqtt.serverURI!=URI){
            val clientId = MqttClient.generateClientId()
            mqtt = MqttAndroidClient(
                context, URI,
                clientId)
            mqtt.setCallback(this)
        }
        val options = MqttConnectOptions()
        options.userName = SamsungProjectConfig.MQTTUser
        options.password = SamsungProjectConfig.MQTTPassword.toCharArray()
        val token = mqtt.connect(options)
        token.actionCallback = object : IMqttActionListener {
            override fun onSuccess(asyncActionToken: IMqttToken?) {
                connected=true
                toggleButtonConnect.isEnabled = true
            }
            override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                connected=false
                toggleButtonConnect.isEnabled = true
                toggleButtonConnect.isChecked = false
            }
        }
    }
    private fun disconnect() {
        if (!::mqtt.isInitialized) {
            mqtt.disconnect()
        }
        connected=false
    }

    private fun send(activity: String, isOn: Boolean) {
        if(connected) {
            val payload = mapOf<String, String>("timestamp" to LocalDateTime.now().toString(),
                "activity" to activity, "isActive" to isOn.toString())
            val json = JSONObject(payload)
            mqtt.publish("labels",json.toString().toByteArray(Charsets.UTF_8), 2, false)
        }
    }

    private fun send2(activity: String, isOn: Boolean, toggleButtonSelected: ToggleButton) {
        if(toggleButtonWalking!=toggleButtonSelected)
            toggleButtonWalking.isChecked=false
        if(toggleButtonSitting!=toggleButtonSelected)
            toggleButtonSitting.isChecked=false
        if(toggleButtonStanding!=toggleButtonSelected)
            toggleButtonStanding.isChecked=false
        if(toggleButtonLayingDown!=toggleButtonSelected)
            toggleButtonLayingDown.isChecked=false

        if(connected) {
            val payload = mapOf<String, String>("timestamp" to LocalDateTime.now().toString(),
                "activity" to activity, "isActive" to isOn.toString())
            val json = JSONObject(payload)
            mqtt.publish("labels2",json.toString().toByteArray(Charsets.UTF_8), 2, false)
        } else {
            toggleButtonSelected.isChecked = false
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        return inflater.inflate(R.layout.main_fragment, container, false)
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        viewModel = ViewModelProviders.of(this).get(MainViewModel::class.java)

        editTextServerURI.setText(viewModel.getURI())
        toggleButtonConnect.setOnClickListener { _ ->
            toggleButtonConnect.isEnabled = false
            if (toggleButtonConnect.isChecked) {
                connect(editTextServerURI.text.toString())
            } else {
                disconnect()
            }
        }

        toggleButtonPrepping.setOnCheckedChangeListener{ _, isChecked ->  send("Prepping Food", isChecked)}
        toggleButtonCooking.setOnCheckedChangeListener{ _, isChecked ->  send("Cooking", isChecked)}
        toggleButtonSettingTable.setOnCheckedChangeListener{ _, isChecked ->  send("Setting Table", isChecked)}
        toggleButtonEating.setOnCheckedChangeListener{ _, isChecked ->  send("Eating", isChecked)}
        toggleButtonClearingTable.setOnCheckedChangeListener{ _, isChecked ->  send("Clearing Table", isChecked)}
        toggleButtonMakingTea.setOnCheckedChangeListener{ _, isChecked ->  send("Making Tea", isChecked)}
        toggleButtonDrinkingTea.setOnCheckedChangeListener{ _, isChecked ->  send("Drinking Tea", isChecked)}
        toggleButtonWatchingTV.setOnCheckedChangeListener{ _, isChecked ->  send("Watching TV", isChecked)}

        toggleButtonWalking.setOnClickListener { button -> if(button is ToggleButton) send2(
            "Walking",
            button.isChecked,
            toggleButtonWalking)}
        toggleButtonSitting.setOnClickListener{ button -> if(button is ToggleButton) send2(
            "Sitting",
            button.isChecked,
            toggleButtonSitting
        )}
        toggleButtonStanding.setOnClickListener{ button -> if(button is ToggleButton) send2(
            "Standing",
            button.isChecked,
            toggleButtonStanding
        )}
        toggleButtonLayingDown.setOnClickListener{ button -> if(button is ToggleButton) send2(
            "Laying Down",
            button.isChecked,
            toggleButtonLayingDown
        )}


    }


}
