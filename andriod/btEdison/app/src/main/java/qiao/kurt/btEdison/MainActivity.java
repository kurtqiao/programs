package qiao.kurt.btEdison;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import static android.widget.Toast.LENGTH_SHORT;


public class MainActivity extends Activity implements View.OnClickListener {

    BluetoothAdapter btAdapter;
    BluetoothDevice  btDevice;

    Button mb_connect;
    Button mb_lighton;
    Button mb_lightoff;
    TextView m_txt_out;

    Boolean Connect_Status;
    Boolean Light_Status;


    ConnectThread mConThread;
    ConnectedThread mConnectedThread;

    // Well known SPP UUID, after run intel SPP_loopback.py, you can see this in edison bluetooth
    private static final UUID SPP_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // intel edison bluetooth MAC address 98:4F:EE:04:0E:B5
    private static String EDISON_ADDRESS = "98:4F:EE:04:0E:B5";

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            byte[] writeBuf = (byte[]) msg.obj;
            //int begin = (int)msg.arg1;
            //int end = (int)msg.arg2;

            //Toast.makeText(getApplicationContext(),"test handler before switch", Toast.LENGTH_SHORT).show();
            switch(msg.what) {
                case 1:
                    String writeMessage = new String(writeBuf);
                    //writeMessage = writeMessage.substring(begin, end);
                    m_txt_out.append(writeMessage);
                    //Toast.makeText(getApplicationContext(),"test handler", Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initContent();
        initBluetooth();

        Connect_Status = false;
        Light_Status = false;

    }

    private void initContent() {
        mb_connect = (Button) findViewById(R.id.b_connect);
        mb_lightoff = (Button) findViewById(R.id.b_lightoff);
        mb_lighton = (Button) findViewById(R.id.b_lighton);
        m_txt_out = (TextView) findViewById(R.id.out_txt);

        mb_connect.setOnClickListener(this);
        mb_lighton.setOnClickListener(this);
        mb_lightoff.setOnClickListener(this);

        mb_lighton.setEnabled(false);
        mb_lightoff.setEnabled(false);
    }
    //You have to add bluetooth permission in AndriodManifest.xml
    private void initBluetooth() {

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        if (btAdapter == null) {
            Toast.makeText(getApplicationContext(), "No bluetooth!", LENGTH_SHORT).show();
            m_txt_out.append("onCreate, No bluetooth detect...\n");
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

            if (!Connect_Status) {

                btDevice = btAdapter.getRemoteDevice(EDISON_ADDRESS);
                mConThread = new ConnectThread(btDevice);
                mConThread.start();

                Toast.makeText(getApplicationContext(), "Connect...", LENGTH_SHORT).show();
                m_txt_out.append("try to connect intel edison by bluetooth...\n");

                Connect_Status = true;
                mb_connect.setText("Disconnect");
                mb_lighton.setEnabled(true);

            } else {
                    mConnectedThread.cancel();
                    mConThread.cancel();

                    Toast.makeText(getApplicationContext(), "Disconnect...", LENGTH_SHORT).show();
                    mb_connect.setText("  Connect  ");
            }
        }

        //Light On Button process
        if(view.getId() == R.id.b_lighton) {
            if (!Light_Status) {

                Toast.makeText(getApplicationContext(), "light on", LENGTH_SHORT).show();

                mConnectedThread.write(("light on").getBytes());
                m_txt_out.append("intel edison light on!\n");
                mb_lighton.setText("  Light Off  ");
                Light_Status = true;

            } else {
                mConnectedThread.write(("light off").getBytes());
                mb_lighton.setText("  Light On  ");
                Light_Status = false;
            }

        }

        //Light Off Button process
        if(view.getId() == R.id.b_lightoff) {
            Toast.makeText(getApplicationContext(), "light off", LENGTH_SHORT).show();
            //bluetoothSendMsg("light off");
            m_txt_out.append("intel edison light off!\n");
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

    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device) {
            BluetoothSocket tmp = null;
            mmDevice = device;
            try {
                tmp = mmDevice.createRfcommSocketToServiceRecord(SPP_UUID);
            } catch (IOException e) { }
            mmSocket = tmp;
            //btSocket = mmSocket;
        }
        public void run() {
            btAdapter.cancelDiscovery();
            try {
                mmSocket.connect();
            } catch (IOException connectException) {
                try {
                    mmSocket.close();
                } catch (IOException closeException) { }
                return;
            }

            mConnectedThread = new ConnectedThread(mmSocket);
            mConnectedThread.start();
        }
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
    }


    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;
        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }
            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }
        public void run() {
            byte[] buffer = new byte[1024];
            int begin = 0;
            int bytes = 0;
            while (true) {
                try {
                    bytes += mmInStream.read(buffer);

                    if (bytes != 0){
                        mHandler.obtainMessage(1, buffer).sendToTarget();
                        bytes = 0;
                    }
                } catch (IOException e) {
                    break;
                }
            }
        }
        public void write(byte[] bytes) {
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) { }
        }
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) { }
        }
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
