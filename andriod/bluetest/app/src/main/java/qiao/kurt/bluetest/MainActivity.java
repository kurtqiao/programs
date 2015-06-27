package qiao.kurt.bluetest;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;


public class MainActivity extends Activity implements View.OnClickListener {

    BluetoothAdapter btAdapter;
    BluetoothDevice  btDevice;
    BluetoothSocket btSocket;
    OutputStream outStream;

    Button mb_connect;
    Button mb_lighton;
    Button mb_lightoff;
    TextView m_txt_out;

    // Well known SPP UUID, after run intel SPP_loopback.py, you can see this in edison bluetooth
    private static final UUID SSP_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // intel edison bluetooth MAC address 98:4F:EE:04:0E:B5
    private static String EDISON_ADDRESS = "98:4F:EE:04:0E:B5";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initContent();
        initBluetooth();

        mb_connect.setOnClickListener(this);
        mb_lighton.setOnClickListener(this);
        mb_lightoff.setOnClickListener(this);

    }

    private void initContent() {
        mb_connect = (Button) findViewById(R.id.b_connect);
        mb_lightoff = (Button) findViewById(R.id.b_lightoff);
        mb_lighton = (Button) findViewById(R.id.b_lighton);
        m_txt_out = (TextView) findViewById(R.id.out_txt);
    }
    //You have to add bluetooth permission in AndriodManifest.xml
    private void initBluetooth() {

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        if (btAdapter == null) {
            Toast.makeText(getApplicationContext(), "No bluetooth!", 0).show();
            m_txt_out.append("onCreate, No bluetooth detect...\n");
            //    finish();
        }
        else {
            if (btAdapter.isEnabled()) {
                m_txt_out.append("check bluetooth enabled...\n");
            } else {
                Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intent, 1);
            }
        }
    }

    //To connect intel edison board through bluetooth SSP, you have to run DBUS APIs on edison board
    //please refer intel document: http://download.intel.com/support/edison/sb/edisonbluetooth_331704007.pdf
    //1. download SPP-loopback.py and run as background in edison by: python SPP-loopback.py &
    //2. you probably need to pair your phone, if so, do belows before step 1
    //2.1 connect edison with putty.exe
    //2.2 rfkill unblock bluetooth
    //    hciconfig hci0 up   [optional]
    //    bluetoothctl
    //2.3 when enter bluetooth environment,
    //    pair Yo:ur:Ad:dr:es:s0
    //    trust Yo:ur:Ad:dr:es:s0
    //    'paired-devices' to check if your phone paired success
    //    now use 'show' to check if content 'UUID: Serial Port'
    //    if yes, means your edison bluetooth can connect by SPP now

    @Override
    public void onClick(View view) {

        if(view.getId() == R.id.b_connect) {
            Toast.makeText(getApplicationContext(),"Connect...", 0).show();
            m_txt_out.append("try to connect intel edison by bluetooth...\n");
            bluetoothConnect();
        }

        //Light On Button process
        if(view.getId() == R.id.b_lighton) {
            Toast.makeText(getApplicationContext(), "light on", 0).show();
            bluetoothSendMsg("light on");
            m_txt_out.append("intel edison light on!\n");
        }

        //Light Off Button process
        if(view.getId() == R.id.b_lightoff) {
            Toast.makeText(getApplicationContext(), "light off", 0).show();
            bluetoothSendMsg("light off");
            m_txt_out.append("intel edison light off!\n");
        }
    }

    //bluetooth send serial message
    private void bluetoothSendMsg(String s) {
        byte [] msgOnBuf;
        msgOnBuf = s.getBytes();
        try {
            outStream.write(msgOnBuf);
        } catch (IOException e) {
            m_txt_out.append("send message fail!\n");
        }
    }

    //connect bluetooth and init write stream
    private void bluetoothConnect() {
        btDevice = btAdapter.getRemoteDevice(EDISON_ADDRESS);
        if (btDevice == null) {
            m_txt_out.append("get remote device fail!\n");
            finish();
        }
        //
        try {
            btSocket = btDevice.createRfcommSocketToServiceRecord(SSP_UUID);
        } catch (IOException e) {
            m_txt_out.append("bluetooth socket create fail.\n");
        }
        //save resource by cancel discovery
        btAdapter.cancelDiscovery();

        //connect
        try {
            btSocket.connect();
            m_txt_out.append("Connection established.\n");
        } catch ( IOException e) {
            try {
                btSocket.close();
            }catch (IOException e2) {
                m_txt_out.append("unable to close socket after connect fail.\n");
            }
        }

        //prepare outStream to send message
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            m_txt_out.append("output stream init fail!\n");
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}

/*
Tips:
**How to access edison mass storage device in the edison file-system?
   rmmod g_multi
   mkdir /update
   losetup -o 8192 /dev/loop0 /dev/disk/by-partlabel/update
   mount /dev/loop0/update

   To reverse this
   umount /update
   modprobe g_multi
refer: https://communities.intel.com/message/253856#253856

**How to control edison GPIOs ?
  refer to intel edison kit for arduino, chapter2.2, chapter11
  http://download.intel.com/support/edison/sb/edisonarduino_hg_331191007.pdf
*/
