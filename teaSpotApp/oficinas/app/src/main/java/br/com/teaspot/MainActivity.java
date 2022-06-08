package br.com.teaspot;

import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.RadioGroup;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;

public class MainActivity extends AppCompatActivity {

    private Button buttonEnviar;
    private RadioGroup radioSabor, radioSugar, radioTemp;

    private String pedido = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonEnviar = findViewById(R.id.buttonEnviar);

        radioSabor = findViewById(R.id.radioSabor);
        radioSugar = findViewById(R.id.radioSugar);
        radioTemp = findViewById(R.id.radioTemp);

        buttonEnviar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                int btn = radioSabor.getCheckedRadioButtonId();
                switch (btn) {
                    case R.id.radioButtonSabor1:
                        pedido = "1";
                        break;
                    case R.id.radioButtonSabor2:
                        pedido = "2";
                        break;
                    case R.id.radioButtonSabor3:
                        pedido = "3";
                        break;
                }

                int btn2 = radioSugar.getCheckedRadioButtonId();
                switch (btn2) {
                    case R.id.radioButtonAcucar:
                        pedido = pedido + "-1";
                        break;
                    case R.id.radioButtonAdocante:
                        pedido = pedido + "-2";
                        break;
                    case R.id.radioButtonNenhum:
                        pedido = pedido + "-3";
                        break;
                }

                int btn3 = radioTemp.getCheckedRadioButtonId();
                switch (btn3) {
                    case R.id.radioButtonSim:
                        pedido = pedido + "-1";
                        break;
                    case R.id.radioButtonNao:
                        pedido = pedido + "-2";
                        break;
                }

                new startSocket().run();
            }
        });
    }

    private void showDialog(String title, String body) {
        AlertDialog alertDialog = new AlertDialog.Builder(MainActivity.this).create();
        alertDialog.setTitle(title);
        alertDialog.setMessage(body);
        alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        alertDialog.show();
    }

    public class startSocket extends Thread{

        public void run() {
            try (Socket configSocket = new Socket()) {
                configSocket.connect(new InetSocketAddress("192.168.1.1", 8848), 1000);

                try (
                        PrintWriter writer = new PrintWriter(configSocket.getOutputStream(), true);
                        BufferedReader reader = new BufferedReader(new InputStreamReader(configSocket.getInputStream()));) {

                    writer.write(pedido);
                    writer.flush();

                    String resposta = reader.readLine();

                    if("PEDIDO ACEITO".equals(resposta)) {
                        showDialog("Sucesso", "Pedido enviado");
                        resposta = reader.readLine();
                        showDialog("Pedido finalizado", "Seu chá está pronto");
                    }
                    else {
                        showDialog("Aguarde", "Fila de pedidos cheia");
                    }
                } catch (IOException ex) {
                    Logger.getLogger(MainActivity.class.getName()).log(Level.SEVERE, null, ex);
                }
            } catch (SocketTimeoutException e){
                Logger.getLogger(MainActivity.class.getName()).log(Level.SEVERE, null, e);
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
            }
        }
    }
}