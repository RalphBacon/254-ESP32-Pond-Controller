#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "serverH.hpp"

// WiFi Server stuff and WiFi logon Credentials
const char *SSID = "YOUR-SSID";
const char *PASSWORD = "YOUR PASSWORD";

// Try and get a static IP address
IPAddress local_IP(192, 168, 1, 92);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

// To get the local time we must include DNS servers
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

namespace WIFI
{
    const char *wl_status_to_string(wl_status_t status)
    {
        switch (status)
        {
        case WL_NO_SHIELD:
            return "WL_NO_SHIELD";
        case WL_IDLE_STATUS:
            return "WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL:
            return "WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED:
            return "WL_SCAN_COMPLETED";
        case WL_CONNECTED:
            return "WL_CONNECTED";
        case WL_CONNECT_FAILED:
            return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
            return "WL_CONNECTION_LOST";
        case WL_DISCONNECTED:
            return "WL_DISCONNECTED";
        default:
            return "UNKNOWN";
        }
    }

    // Configure the local network connection with specified IP address
    void begin()
    {
        WiFi.mode(WIFI_STA);
        WiFi.persistent(false);
        WiFi.setAutoReconnect(true);
        WiFi.setSleep(false);
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        if (WiFi.begin(SSID, PASSWORD))
        {
            log_i("WiFi begin success.");
        }
    }

    // Connect to the Wi-Fi as a client (station, not an access point)
    bool connect()
    {
        log_d("Connecting to WiFi (60 second timeout)");
        uint8_t wifiStatus = WiFi.waitForConnectResult();

        // Successful connection?
        if ((wl_status_t)wifiStatus != WL_CONNECTED)
        {
            log_e("WiFi Status: %s, exiting\n", wl_status_to_string((wl_status_t)wifiStatus));
            return false;
        }

        // Just some information about the Wifi channel & IP address
        log_i(
            "Running on SSID %s with IP %s, Channel %d, MAC address %sF",
            (char *)WiFi.SSID().c_str(),
            (char *)(WiFi.localIP()).toString().c_str(),
            WiFi.channel(),
            (char *)WiFi.macAddress().c_str());

        // Now register all the server pages and start the server
        serverH::registerServerPages();

        // All done
        return true;
    }
}