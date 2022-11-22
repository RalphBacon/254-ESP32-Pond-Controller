# Video #254 ESP32 Pond Controller  
I wanted a web-based controller for my pond pumps. Here's a very quick walkthrough of the full project.  

![Thumbnail-00011 (Custom)](https://user-images.githubusercontent.com/20911308/202498410-702e664c-e673-41f2-8ada-202741559a4f.png)  
**YouTube Video Link: https://youtu.be/LFzoKGW2qHQ**  
<br>
[![PCBWayPrototype the easy way](https://user-images.githubusercontent.com/20911308/185422574-52a4e7db-c680-4dd2-87be-1f1dd1db6a65.gif "PCBWay - up to 20% Discount on 4 & 6-layer PCBs")](https://pcbway.com/)  
**► Use coupon code **PCBWay8th-RSB** to get $10 off a $40 order! Coupon lasts for THREE months. Limited number of coupons available.**

### DETAILS
I decided that a very simple, time-based pond-pump controller was enough for my tiny barrel pond. And three fishes.

An ESP32 microcontroller was used because I wanted to be able to control the relays (that control the two pumps) via the web, using my phone. Having designed the circuit, then the PCB (restricting myself to the maximum 100mm x 100mm that most PCB fabs have) I cut a wooden base plate for the enclosure.

This made it very easy to mount the PCB, relays module board and anything else I needed. With a transparent lid it was easy to see the 7-segment time and countdown display.

► I made use of the task scheduler library from arkhipenko:  
<br>
![Task Scheduler Library Screenshot (Custom)](https://user-images.githubusercontent.com/20911308/197220810-609ad936-ce50-4652-948c-1b4f6491e276.png)  
**arkhipenko/TaskScheduler**: Cooperative multitasking for Arduino, ESPx, STM32, nRF and other microcontrollers  
https://github.com/arkhipenko/TaskScheduler  

### INFORMATION

Random Nerds (Rui Santos) cribsheet for 'safe' ESP32 GPIO pins (but see my augmented version in this GitHub, ready for download)  
https://randomnerdtutorials.com/esp32-pinout-reference-gpios/ 

More information (including all links, sketches & PDFs in my GitHub):
https://github.com/RalphBacon/254-ESP32-Pond-Controller

► List of all my videos
(Special thanks to Michael Kurt Vogel for compiling this)  
http://bit.ly/YouTubeVideoList-RalphBacon

► If you like this video please give it a thumbs up, share it and if you're not already subscribed please consider doing so and joining me on my Arduinite (and other μControllers) journey

My channel, GitHub and blog are here:  
\------------------------------------------------------------------  
• https://www.youtube.com/RalphBacon  
• https://ralphbacon.blog  
• https://github.com/RalphBacon  
• https://buymeacoffee.com/ralphbacon  
\------------------------------------------------------------------

My ABOUT page with email address: https://www.youtube.com/c/RalphBacon/about
