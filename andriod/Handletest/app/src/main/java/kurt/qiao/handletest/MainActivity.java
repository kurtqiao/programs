package kurt.qiao.handletest;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;


public class MainActivity extends Activity implements View.OnClickListener {

    TextView mTxTout;
    Button mNTButton;
    TestThread mTThread;

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            byte[] writeBuf = (byte[]) msg.obj;

            //Toast.makeText(getApplicationContext(), "test handler before switch", Toast.LENGTH_SHORT).show();
            mTxTout.append("run handle message.\n");
            switch(msg.what) {
                case 1:
                    String writeMessage = new String(writeBuf);
                    //Toast.makeText(getApplicationContext(),"test handler", Toast.LENGTH_SHORT).show();
                    mTxTout.append(" handle message switch 1 \n");
                    mTxTout.append(writeMessage);
                    break;
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTxTout = (TextView)findViewById(R.id.TH_TEXT);
        mNTButton = (Button)findViewById(R.id.b_NEW_THREAD);
        mNTButton.setOnClickListener(this);

        ((TextView)findViewById(R.id.textView)).setText("A test program that when you click button,\na new thread will be created and feedback message.");

    }

    @Override
    public void onClick(View view) {

        Toast.makeText(getApplicationContext(), "new thread", Toast.LENGTH_SHORT).show();
        if (view.getId() == R.id.b_NEW_THREAD) {
            //create new thread and send to target
            mTxTout.append("new thread created..\n");
            mTThread = new TestThread();
            mTThread.start();


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

    private class TestThread extends Thread {

        //public TestThread() {

        //}
        public void run() {
            //byte [] threadmsg
            mHandler.obtainMessage(1, ("feedback thread message!\n").getBytes()).sendToTarget();
            //Looper.loop();
        }

    }
}
