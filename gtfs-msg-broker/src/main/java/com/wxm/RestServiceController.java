package com.wxm;

import com.mongodb.BasicDBObject;
import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.HttpClients;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationListener;
import org.springframework.context.annotation.Bean;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageHeaders;
import org.springframework.messaging.core.MessageSendingOperations;
import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.messaging.simp.SimpMessageSendingOperations;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.messaging.simp.broker.BrokerAvailabilityEvent;
import org.springframework.stereotype.Component;
import org.springframework.stereotype.Controller;
import org.springframework.stereotype.Service;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketHandler;

import javax.net.ssl.HttpsURLConnection;
import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.io.StringWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.*;

/**
 * Created by johngouf on 17/07/15.
 */
@RestController
public class RestServiceController implements ApplicationListener<BrokerAvailabilityEvent>{

    MessageSendingOperations<String> messageSendingOperations;

    @Autowired
    public RestServiceController(final MessageSendingOperations<String> messageSendingOperations){
        this.messageSendingOperations = messageSendingOperations;
    }

    @RequestMapping("/update")
    public String hello(HttpServletRequest servletRequest) throws IOException {
        Map<String,String[]> values = servletRequest.getParameterMap();
        HashMap toSend = new HashMap();
        Set<String> keySet = values.keySet();
        for(String key : keySet)
        {
            toSend.put(key,values.get(key)[0]);
        }
        messageSendingOperations.convertAndSend("/topic/update",toSend);

        HttpClient httpclient = HttpClients.createDefault();
        HttpPost httpost = new HttpPost("https://api.parse.com/1/push");
        List channels = new ArrayList<>();
        channels.add("");

        HashMap data = new HashMap();
        data.put("icon",toSend.get("icon"));
        data.put("title",toSend.get("title"));
        data.put("snippet",toSend.get("snippet"));
        data.put("lat",Double.parseDouble(toSend.get("lat").toString()));
        data.put("lon",Double.parseDouble(toSend.get("lon").toString()));

        BasicDBObject obj = new BasicDBObject();
        obj.append("channels",channels);
        obj.append("data",data);

        System.out.println(obj.toString());

        httpost.setEntity(new StringEntity(obj.toString()));
        httpost.setHeader("X-Parse-Application-Id", "1fiaUIGamXFm3SeTPIP9uIznZpFjWwGi8PrBlBwo");
        httpost.setHeader("X-Parse-REST-API-Key", "g0fW4HXHaqpjeL5rCyDlRZ2mHdif9ChA59Lij2TS");
        httpost.setHeader("Content-Type", "application/json");

        HttpResponse response = httpclient.execute(httpost);
        HttpEntity entity = response.getEntity();

        StringWriter writer = new StringWriter();
        IOUtils.copy(entity.getContent(), writer);
        String theString = writer.toString();
        System.out.println(theString);

        return "hello!!";
    }

    @Override
    public void onApplicationEvent(BrokerAvailabilityEvent brokerAvailabilityEvent) {

    }
}
