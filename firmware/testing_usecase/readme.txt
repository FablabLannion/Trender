README

TRENDER - use case: Testing

author: Julien JACQUES (jjacques@legtux.org)
        FabLab de Lannion

Usecase demonstrated at Lannion's Adonnantes#2016

1. Compile (complete the "........"'s fields before)
   and 
   Upload the sketch "testing_usecase.ino" to your esp8266 device.

2. With your ThingSpeak's account (previously created),
   a. create 2 channels
      - First one to grab the test results
      - the other one to get scores computed from the first channel's data
   
   b. create a new app by copying the script named "testing_thingspeak_app.m"
      (-> fill the blank fields with the appripriate channelIDs and API Keys)

3. Put the PHP Script named "index.php" (after fill the appropriate Write API Key)
   somewhere on your PHP server
   
   Run the PHP script to generate test results from the "Test Result Simulator"
   e.g.   http//<yourdomainname>/index.php?wait=30&maxi=2


  --> You are now ready to visualize your result trending ! 
