// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include <ESP8266WiFi.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"

#define WIFI_SSID "testesp"
#define WIFI_PASSWORD "test1234"

const char* SCOPE_ID = "0ne009DBDFB";
const char* DEVICE_ID = "1iaapabmtpi";
const char* DEVICE_KEY = "teW5yzBRRiyWqVcsTkMGQPp/MLhevr9z3z4ujRihAvk=";

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // ConnectionStatus
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

  // payload buffer doesn't have a null ending.
  // add null ending in another buffer before print
  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
}

void setup() {
  Serial.begin(9600);

  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
  }
}

void loop() {
  if (isConnected) {
    unsigned long ms = millis();
    if (ms - lastTick > 300000) {  // send telemetry every 5 minutes
      char msg[64] = { 0 };
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // send telemetry
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Temperatura\": %d, \"Humedad\": %d, \"PresionATM\": %d}",
               random(15,25), random(30,50), random(1,5));
        errorCode = iotc_send_telemetry(context, msg, pos);
      /*  errorCode = iotc_send_telemetry(context, msg, pos);

        pos = snprintf(msg, sizeof(msg) - 1, "{\"Humedad\": %d}",
                       20 + (rand() % 20));
        errorCode = iotc_send_telemetry(context, msg, pos);

        pos = snprintf(msg, sizeof(msg) - 1, "{\"PresionATM\":%d}",
                       1 + (rand() % 5));
        errorCode = iotc_send_telemetry(context, msg, pos);*/
      } else {  // send property
                // pos = snprintf(msg, sizeof(msg) - 1, "{\"PresionATM\":%d}",
        //               1 + (rand() % 5));
        //errorCode = iotc_send_property(context, msg, pos);
      }
      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Sending message has failed with error code %d", errorCode);
      }
    }

    iotc_do_work(context);  // do background work for iotc
  } else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  }
}
