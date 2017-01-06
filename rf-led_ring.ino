    // Displays different effects on a neopixel ring.
    // Controlled by a NEXA/tellstick transmitter
    //
    // Use the "on" button to loop thru the different effects
    // Turn it off with the "off" button
    //
    //  Program with 
    //       Board = "Adafruit Trinket 8Mhz"
    //       Programmer = "USBtinyISP"
    //
    //  Hardware 
    //    Trinket 5v
    //       Neopixel   --> pin0
    //       Radiomodul --> pin2
    //
    //  Libs
    //    NewRemoteReceiver -->  https://bitbucket.org/fuzzillogic/433mhzforarduino
    //    Adafruit_NeoPixel --> 

 

    #include <NewRemoteReceiver.h> 
    #include <Adafruit_NeoPixel.h>
    
    #ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
     #include <avr/power.h>
    #endif
     
    #define PIN 0           //Output to the led ring pin
    #define iLED 1          //Onboard LED
    #define LEDs 12         //Number of LEDs
    #define ADDR 1000010    //Tellstick addr
    #define MODES 5         //Nax mode
    #define BRIGHTNESS 25   //Brightness

     
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDs, PIN);
     
    uint8_t  mode   = 0,        // Current animation effect
             offset = 0;        // Position of spinny eyes


    //------------------------------------------------------------- 
    void setup() {
      #ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
        if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
      #endif
      
      pixels.begin();
      pixels.setBrightness(BRIGHTNESS);

      // Initialize receiver on interrupt 0 (= digital pin 2), calls the callback "showCode"
      // after 2 identical codes have been received in a row. (thus, keep the button pressed for a moment)
      NewRemoteReceiver::init(0, 2, showCode);
      pinMode(iLED, OUTPUT);   //DEBUG
    }


    //-------------------------------------------------------------  
    void loop() {

      uint8_t i = 0;
      
      switch(mode) {

       case 0: // Turned it off 
         for(i=0; i<LEDs; i++) pixels.setPixelColor(i, 0x000000);  //Off
         pixels.show();
         delay(500);
         break;

         
       case 1: // Random sparks - just one LED on at a time!
         i = random(LEDs);
         pixels.setPixelColor(i, random(0xFFFFFF));
         pixels.show();
         delay(500);
         pixels.setPixelColor(i, 0x000000);
         break;

     
       case 2:  // Spinny wheels (3 LEDs on at a time)
         for(i=0; i<LEDs; i++) {
           if(((offset + i) & 3) == 0) 
               pixels.setPixelColor(i, 0xFF0000);
           else
               pixels.setPixelColor(i, 0x000000);
         }
         pixels.show();
         offset++;
         delay(500);
         break;


       case 3:  // Rainbow
         for(i=0; i<LEDs; i++) {
           if(((offset + i) & 3) == 0)  pixels.setPixelColor(i, 0xFF0000);
           if(((offset + i) & 3) == 1)  pixels.setPixelColor(i, 0x00FF00);
           if(((offset + i) & 3) == 2)  pixels.setPixelColor(i, 0x0000FF);
           if(((offset + i) & 3) == 3)  pixels.setPixelColor(i, 0x000000);
         }
         pixels.show();
         offset++;
         delay(500);
         break;   


       case 4:  // Pulsar  
         if (offset < 127)
            for(i=0; i<LEDs; i++) pixels.setPixelColor(i, offset, 0 ,0);
         else
            for(i=0; i<LEDs; i++) pixels.setPixelColor(i, 255-offset, 0 ,0);
         pixels.show();
         offset++;
         delay(10);
         break;  


       case 5:   // All set to white
         for(i=0; i<LEDs; i++) pixels.setPixelColor(i, 0xFFFFFF);   //White
         pixels.show();
         delay(500);
         break;   

          
      }
    }


   //------------------------------------------------------------- 
   // Callback function is called only when a valid code is received.
   // Note: interrupts are disabled. You can re-enable them if needed.
   void showCode(NewRemoteCode receivedCode) {

    if (receivedCode.address != ADDR) {
      switch (receivedCode.switchType) {
        case NewRemoteCode::off:
          digitalWrite(iLED, LOW);    //DEBUG
          mode = 0;                   //Set to mode 0 (off)
          break;
        
        case NewRemoteCode::on:
          digitalWrite(iLED, HIGH);   //DEBUG
          mode++;                     //Loop thru the modes..
          if (mode > MODES) mode = 0;
          break;
          
       }
     }
   }
