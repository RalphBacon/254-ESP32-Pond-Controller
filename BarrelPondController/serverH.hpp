#pragma once
#include "Arduino.h"
#include "defsHelper.h"
#include "relayH.hpp"
#include "buttonCss.h"

// Server pages go here
//#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Allow OTA software updates
#include <AsyncElegantOTA.h>

#include <LittleFS.h>

// This is the standard web server for the ESP32 (on port 80)
AsyncWebServer server(80);

namespace serverH
{
    const char *headerHTML1 = R"(
        <head>
            <title>Barrel Pond Controller</title>
            <meta name='viewport' content='width=device-width, initial-scale=1'>
            <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate" />
            <meta http-equiv="Pragma" content="no-cache" />
            <meta http-equiv="Expires" content="0" />
            <meta http-equiv="refresh" content="300">
            <style>
                @media screen and (min-width: 601px) {
                    div.temperature {
                        font-size: 1em;
                    }
                }

                /* if the max width of the device is 600px or less (eg portrait phone)
                get bigger fonts to make it readable */
                @media screen and (max-width: 600px) {
                    div.temperature {
                        font-size: 1.8em;
                    }
                })";
    /*
      We will inject the buttonCSS const here if required for the [landing] page
    */
    const char *headerHTML2 = R"(
        </style>
        </head>)";

    // Default 404 response
    void handle_NotFound(AsyncWebServerRequest *request)
    {
        request->send(404, "text/plain", "That page is missing, so sorry!");
    }

    // Main Landing Page
    void handle_landing(AsyncWebServerRequest *request)
    {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->printf(
            R"(
            <!DOCTYPE HTML>
            <html>
            )");
        response->printf(headerHTML1);
        response->printf(buttonCSS);
        response->printf(headerHTML2);
        response->printf(
            R"(                
            <body style='font-size:1em;font-family:arial;text-align:center'>
            <h3>BARREL POND CONTROL</h3>
            <div>
                <button type="button" class="button 
            )");

        // If the pump is ON now, allow to turn OFF and vice versa
        const char *btnText;
        if (relays::isWaterOn())
        {
            response->printf(
                R"(
                turnOFF"  
                onclick="location.href='http://192.168.1.92/off'">)");
            btnText = "Turn OFF pond pump";
        }
        else
        {
            response->printf(R"(
                turnON"  
                onclick="location.href='http://192.168.1.92/on'">)");
            btnText = "Turn ON pond pump";
        }

        // Text displayed on the button
        response->printf(btnText);

        // Anchor target for on/off
        response->printf(
            R"(
	            </button>
            )");

        // If in a feeding period, notify the user
        if (feedingTime)
        {
            response->printf(
                R"(
                <p style="color:red;font-weight:bold;">
                    WARNING: It's fishy feeding time!
                </p>
            )");
        }

        // Finish off the div and start the table
        response->printf(
            R"(
	        </div>)");

        // Images are served up separately, see the "server.on" section
        response->printf(R"(<img src='goldfish'>)");

        // Add in the admin footer if required
        if (request->hasParam("admin"))
        {
            // Ensure the format is: /?admin=true
            String overrideParm = request->getParam("admin")->value();
            if (overrideParm == "true")
            {
                response->printf(
                    R"(
                    <div class="footer">
                    <table>
                        <tr>
                            <th>Variable
                            <th>Status
                        </tr>

                    )");

                response->printf("<tr><td>Date<td>%02d/%02d/%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
                response->printf("<tr><td>Time<td>%02d:%02d:%02d", hoursTime, minsTime, secsTime);
                response->printf("<tr><td>Pump<td>%s\n", relays::isWaterOn() ? "ON" : "OFF");
                response->printf("<tr><td>Air<td>%s\n", relays::isAirOn() ? "ON" : "OFF");
                response->printf("<tr><td>Countdown<td>%02d:%02d\n", minsCountDown, secsCountDown);
                response->printf("<tr><td>Feeding Time?<td>%s", feedingTime ? "YES" : "NO");

                // Complete the table (admin only)
                response->printf(R"(
                    </table>
                    </div> <!-- footer ->)");
            }
        }

        // Complete the page
        response->printf(R"(
            </body>
            </html>)");

        // Send the page
        request->send(response);
    }

    // Turn everything off, time to feed the fish
    void handle_allOff(AsyncWebServerRequest *request)
    {
        // Turn everything off...
        log_i("Manual override to turn everything OFF");
        relays::airOff();
        relays::waterOff();

        // ... And back on again in a few minutes
        allBackOnTask.setIterations(1);
        allBackOnTask.enableDelayed(60000 * 10);
        log_v("Task allBackOn scheduled for 10 minutes");

        // Set the countdown timer so it starts getting shown
        minsCountDown = 10;
        secsCountDown = 0;
        countDownTimerActive = true;

        // request->send(200, "text/html", "All OFF");
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->printf(
            R"(
            <!DOCTYPE HTML>
            <html>
            )");
        response->printf("<meta http-equiv='refresh' content='5;URL=\"http://192.168.1.92/\"' />");
        response->printf(headerHTML1);
        response->printf(headerHTML2);
        response->printf(
            R"(                
            <body style='font-size=1.1em'>
            <h3>POND PUMPS NOW OFF</h3>
            <p>Will turn back on in )");
        std::string turnbackOnAfter = std::to_string(AUTO_ON_MINS);
        response->printf(turnbackOnAfter.c_str());
        response->printf(
            R"( minutes</p>
            <p style='font-size:smaller;'>(Unless in the 15 minutes 'quiet' period)</p>
            </body>
            </html>
        )");

        request->send(response);
    }

    // Web request to turn everything (back) ON
    void handle_allOn(AsyncWebServerRequest *request)
    {
        // Turn everything on
        log_i("Manual override to turn everything ON");
        relays::airOn();
        relays::waterOn();

        // Cancel any pending task to turn everything ON (set as result of previous ALL OFF)
        allBackOnTask.cancel();

        // Countdown timer prematurely completes (still will show until zero then blanks)
        minsCountDown = 0;
        secsCountDown = 0;

        // request->send(200, "text/plain", "All ON");
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->printf(
            R"(
            <!DOCTYPE HTML>
            <html>
            )");
        response->printf("<meta http-equiv='refresh' content='5;URL=\"http://192.168.1.92/\"' />");
        response->printf(headerHTML1);
        response->printf(headerHTML2);
        response->printf(
            R"(                
            <body style='font-size=1.1em'>
            <h3>POND PUMPS NOW ON</h3>
            <p>Will stay on until next hour</p>
            </body>
            </html>
        )");

        request->send(response);
    }

    // When this server gets a request for "<img src=goldfish>" it runs this handle function
    void handleFishName(AsyncWebServerRequest *request)
    {
        // These are the random fish images in /data folder (300px wide)
        // (We could read the files in the data folder to get this list)
        static std::vector<std::string> fishes =
            {"/bigeyes.png", "/blackie.png", "/fancy.png", 
            "/goldfish.png", "/hairy.png", "/hugeeyes.png", 
            "/millie.png", "/multicolor.png"};

        // Get a random number between 0 and the # of fish images
        // Note that the random seed was carried out as part of the server pages registration
        int randomFish = random(0, fishes.size());
        log_v("Random fish #%d: %s", randomFish, fishes[randomFish].c_str());
        
        // Send that image filename back to browser
        request->send(LittleFS, fishes[randomFish].c_str(), "image/png");
    }

    // Register all pages we want the user to access
    void registerServerPages()
    {
        // Just to be sure we have a 404 response on unknown requests
        server.onNotFound(
            [](AsyncWebServerRequest *request)
            {
                // request->send(404, "text/plain", "404 - Page Not Found, oops!");
                serverH::handle_NotFound(request);
            });

        // Main landing page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { serverH::handle_landing(request); });

        // Turn everything OFF
        server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
                  { serverH::handle_allOff(request); });

        // Turn everything ON
        server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
                  { serverH::handle_allOn(request); });

        // Goldfish images - we'll pick one from the list
        server.on("/goldfish", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      // Send a random fish image from the /data folder
                      serverH::handleFishName(request); });

        // Starting Async OTA web server AFTER all the server.on requests registered
        AsyncElegantOTA.begin(&server);
        server.begin();

        // Seed the random number generator for the fish image selection (see handleFishName)
        randomSeed(analogRead(34));
    }
}