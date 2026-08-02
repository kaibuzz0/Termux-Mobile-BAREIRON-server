// ServerConnectActivity.java — Connect to BAREIRON server via IP
package com.bareiron.game;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

import java.io.*;
import java.net.*;

public class ServerConnectActivity extends AppCompatActivity {
    private EditText etServerIP;
    private EditText etServerPort;
    private Button btnConnect;
    private Button btnPing;
    private TextView tvResult;
    private Handler handler = new Handler(Looper.getMainLooper());
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server_connect);
        
        etServerIP = findViewById(R.id.etServerIP);
        etServerPort = findViewById(R.id.etServerPort);
        btnConnect = findViewById(R.id.btnConnect);
        btnPing = findViewById(R.id.btnPing);
        tvResult = findViewById(R.id.tvResult);
        
        etServerIP.setText("127.0.0.1");
        etServerPort.setText("25565");
        
        btnPing.setOnClickListener(v -> pingServer());
        btnConnect.setOnClickListener(v -> connectToServer());
    }
    
    private void pingServer() {
        String ip = etServerIP.getText().toString().trim();
        int port = Integer.parseInt(etServerPort.getText().toString().trim());
        
        tvResult.setText("Pinging " + ip + ":" + port + "...");
        
        new Thread(() -> {
            try (Socket socket = new Socket()) {
                socket.connect(new InetSocketAddress(ip, port), 5000);
                
                // Send Minecraft handshake + status request (protocol 772)
                DataOutputStream out = new DataOutputStream(socket.getOutputStream());
                DataInputStream in = new DataInputStream(socket.getInputStream());
                
                // Handshake
                ByteArrayOutputStream handshake = new ByteArrayOutputStream();
                DataOutputStream hw = new DataOutputStream(handshake);
                hw.writeByte(0x00); // packet ID
                writeVarInt(hw, 772); // protocol version
                writeVarInt(hw, ip.length());
                hw.writeBytes(ip);
                hw.writeShort(port);
                hw.writeByte(0x01); // status
                
                byte[] hwBytes = handshake.toByteArray();
                writeVarInt(out, hwBytes.length);
                out.write(hwBytes);
                
                // Status request
                out.writeByte(0x01);
                out.writeByte(0x00);
                out.flush();
                
                // Read response
                int len = readVarInt(in);
                int packetId = readVarInt(in);
                int jsonLen = readVarInt(in);
                byte[] jsonBytes = new byte[jsonLen];
                in.readFully(jsonBytes);
                String json = new String(jsonBytes);
                
                handler.post(() -> tvResult.setText("✅ Server online!\n" + json));
                
            } catch (Exception e) {
                handler.post(() -> tvResult.setText("❌ Failed: " + e.getMessage()));
            }
        }).start();
    }
    
    private void connectToServer() {
        String ip = etServerIP.getText().toString().trim();
        int port = Integer.parseInt(etServerPort.getText().toString().trim());
        
        // Save connection
        getSharedPreferences("bareiron", MODE_PRIVATE)
            .edit()
            .putString("last_ip", ip)
            .putInt("last_port", port)
            .apply();
        
        tvResult.setText("✅ Saved connection: " + ip + ":" + port + "\nLaunch Minecraft to join!");
    }
    
    // Minecraft VarInt helpers
    private void writeVarInt(DataOutputStream out, int value) throws IOException {
        while ((value & 0xFFFFFF80) != 0) {
            out.writeByte(value & 0x7F | 0x80);
            value >>>= 7;
        }
        out.writeByte(value);
    }
    
    private int readVarInt(DataInputStream in) throws IOException {
        int value = 0;
        int position = 0;
        byte currentByte;
        while (true) {
            currentByte = in.readByte();
            value |= (currentByte & 0x7F) << position;
            if ((currentByte & 0x80) == 0) break;
            position += 7;
            if (position >= 32) throw new IOException("VarInt too big");
        }
        return value;
    }
}
