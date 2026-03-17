/*
 File/Sketch Name: AudioFrequencyDetector

 Version No.: v1.0  Created 12 December, 2019
 
 Original Author: Clyde A. Lettsome, PhD, PE,  MEM
 
 Description:  This code/sketch makes displays the approximate frequency  of the loudest sound detected by a sound detection module. For this project, the  analog output from the 
 sound module detector sends the analog audio signal  detected to A0 of the Arduino Uno. The analog signal is sampled and quantized (digitized).  A Fast Fourier Transform (FFT) is
 then performed on the digitized data. The  FFT converts the digital data from the approximate discrete-time domain result.  The maximum frequency of the approximate discrete-time
 domain result is then  determined and displayed via the Arduino IDE Serial Monitor.

 Note: The arduinoFFT.h  library needs to be added to the Arduino IDE before compiling and uploading this  script/sketch to an Arduino.

 License: This program is free software; you  can redistribute it and/or modify it under the terms of the GNU General Public License  (GPL) version 3, or any later
 version of your choice, as published by the Free  Software Foundation.

 Notes: Copyright (c) 2019 by C. A. Lettsome Services,  LLC
 For more information visit https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/

*/

#include  "arduinoFFT.h"

#include <FastLED.h>

#define NUM_LEDS 256
#define LED_PIN 7

//int Led=LED_BUILTIN;//set up pin for LED output (any Digital pin can be used)
int micsensorpin=3; //setup pin for input from Sensor D0
int micState;//set up a variable to hold info from D0 (HIGH or LOW)
const int Mic_pin = A0;

CRGB led[NUM_LEDS];
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must  be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
double highestPeak = 900;
double lowestPeak = 400;
int highest_vol = 0;
int lowest_vol = 0; 
int normalized_vol = 0;
 
arduinoFFT  FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
unsigned long lastTimeChecked = 0;  // To track time for the 10-second window
unsigned long lastAllOff = 0;  // To track time for the 10-second window
  
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double  vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
 
void  setup() 
{
    Serial.begin(9600); //Baud rate for the Serial Monitor
    samplingPeriod = round(100*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds  
    FastLED.addLeds<NEOPIXEL, LED_PIN>(led, NUM_LEDS);
    pinMode(Mic_pin, INPUT);
    pinMode(micsensorpin,INPUT);//set up the sensor pin to be an input
    digitalWrite(micsensorpin,LOW); //start with the sensor pin off
   // DisplayGaussianOFF(0, 5);
   // DisplayLightON(10);

}

void DisplayGlimmering(int position, int brightness) {
  int row = position / 20;  // Determine which row it belongs to (0, 1, or 2)
  int ledIndex = position % 20; // Find the LED index in the row

  // Map brightness based on volume (e.g., 0 to 255 range)
  int finalBrightness = constrain(brightness, 0, 255);

  // Serial.println("ledIndex");
  // Serial.println(ledIndex); 
  // Serial.println("finalBrightness");
  // Serial.println(finalBrightness); 
  // Set LED color based on brightness
  // led[ledIndex + row * 20] = CRGB(finalBrightness, finalBrightness, finalBrightness);
  led[position] = CRGB(finalBrightness, finalBrightness, finalBrightness*0.75);
  FastLED.show();
  delay(10);
  // led[ledIndex + row * 20] = CRGB(0, 0, 0);
  // led[position] = CRGB(0, 0, 0);
}

void DisplayAllOff(int position){
  led[position] = CRGB(0, 0, 0);
  for (int i = 0; i < NUM_LEDS; i++){
    led[i] = CRGB(0, 0, 0);
    // delay(1);
  }
  // delay(1);
}

void DisplayGaussian(int v1, int width){
  int p;
  if (v1 <= 0){
    p = 0;
  }
  else if (v1 >= NUM_LEDS -1){
    //Serial.println("v1 > 59");
    p = NUM_LEDS-1;
  }
  else{
    p = v1;
  }

  //p = 61;
  Serial.println("p = ");
  Serial.println(p);

  if ((0 < p) && (p < 1*(NUM_LEDS/5))){
    Serial.println("p<10");
    Serial.println(p<10);
    for (int i = 0; i < int(width/2); i++){
      if ((p-i) >=0){
        led [p - i] = CRGB (100/((i+1)), 0, 0);
      }
      if ((p+i) < 60){
        led [p + i] = CRGB (100/((i+1)), 0, 0);
      }
      
    }
  }

  else if (1*(NUM_LEDS/5 <= p) && (p < 2*(NUM_LEDS/5))){
    Serial.println("blue");
    for (int i = 0; i < int(width); i++){
      if ((p-i) >=0){
        led [p - i] = CRGB ( 50/((i+1)), 50/((i+1)), 0 );
      }
      if ((p+i) <60){
        led [p + i] = CRGB ( 50/((i+1)), 50/((i+1)), 0);
      }
    }
  }

  else if (2*(NUM_LEDS/5<= p) && (p < 3*(NUM_LEDS/5))){
    Serial.println("blue");
    for (int i = 0; i < int(width); i++){
      if ((p-i) >=0){
        led [p - i] = CRGB ( 0, 100/((i+1)), 0 );
      }
      if ((p+i) <60){
        led [p + i] = CRGB ( 0, 100/((i+1)), 0);
      }
    }
  }

  else if (3*(NUM_LEDS/5 <= p) && (p < 4*(NUM_LEDS/5))){
    Serial.println("blue");
    for (int i = 0; i < int(width); i++){
      if ((p-i) >=0){
        led [p - i] = CRGB ( 0, 50/((i+1)), 50/((i+1)));
      }
      if ((p+i) <60){
        led [p + i] = CRGB ( 0, 50/((i+1)), 50/((i+1)));
      }
    }
  }

  else if (4*(NUM_LEDS/5< p) && (p <5*(NUM_LEDS/5 ))){
    Serial.println("green");
    for (int i = 0; i < int(width); i++){
      if ((p-i) >=0){
        led [p - i] = CRGB ( 0, 0, 100/(i+1));
      }
      if ((p+i) <60){
        led [p + i] = CRGB ( 0, 0, 100/(i+1));
      }
    }
  }

  FastLED.show();
}

void DisplayGaussianOFF(int v1, int width){

  for (int i = 0; i < int(width); i++){
    if ((v1-i) >=0){
      led [v1 - i] = CRGB (0, 0, 0);
    }
    if ((v1+i) <60){
      led [v1 + i] = CRGB (0, 0, 0);
    }
  }
  FastLED.show();

}

void loop() 
{  
  float a;
  int b;
  float vol;
  int width;
     /*Sample SAMPLES times*/
     for(int  i=0; i<SAMPLES; i++)
     {
         microSeconds = micros();    //Returns the  number of microseconds since the Arduino board began running the current script.  
     
         vReal[i] = analogRead(Mic_pin); //Reads the value from analog pin  0 (A0), quantize it and save it as a real term.
         //Serial.println(vReal[i]); 
         vImag[i] = 0; //Makes  imaginary term 0 always
       /*remaining wait time between samples if  necessary*/
         while(micros() < (microSeconds + samplingPeriod))
         {
           //do nothing
         }
     }
 
     /*Perform FFT on samples*/
     FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
     FFT.Compute(vReal,  vImag, SAMPLES, FFT_FORWARD);
     FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency and print peak*/
     double peak = FFT.MajorPeak(vReal,  SAMPLES, SAMPLING_FREQUENCY);
     vol = analogRead(Mic_pin);
    //  Serial.println("peak");
    //  Serial.println(peak);     //Print out the most  dominant frequency.
       // Track the highest peak over the last 100 seconds
      if (millis() - lastTimeChecked >= 100000) {  // 100 seconds have passed
        highestPeak = 1000;  // Reset the highest peak value after 10 seconds
        lowestPeak = 100;
        lastTimeChecked = millis();  // Reset the timestamp
      }
      // Update highestPeak if the current peak is greater than the previous highest
      if (peak > highestPeak) {
        highestPeak = peak;
      }
      if (peak < lowestPeak) {
        lowestPeak = peak;
      }


      if (millis() - lastTimeChecked >= 10000) {  // 100 seconds have passed
        highest_vol = vol;
        lowest_vol = vol;
        lastTimeChecked = millis();  // Reset the timestamp
      }
      // Update highestPeak if the current peak is greater than the previous highest
      if (vol > highest_vol) {
        highest_vol = vol;
      }
      if (vol < lowest_vol) {
        lowest_vol = vol;
      }

      normalized_vol = int(map(vol, lowest_vol, highest_vol, 0, 255));
      Serial.println(normalized_vol);


      // Normalize "a" by the highest peak seen in the last 100 seconds
      // a = (peak * NUM_LEDS) / (highestPeak);  // Normalize based on the highest peak
      a = map(peak, lowestPeak, highestPeak, 0, 263);
      b = int(a);
     if (b > NUM_LEDS -1){
      b = NUM_LEDS;
     } 

    int sound_on = digitalRead(micsensorpin);
    if (sound_on){
      DisplayGlimmering(b, 255);
      // Serial.println(b);
    }
    else{
      DisplayAllOff(b);
      Serial.println("all off");
      lastAllOff = millis();
    }
    if (millis() - lastAllOff >= 7000) {  // 10 seconds have passed and they have not all turned off
        DisplayAllOff(b);
        Serial.println("used fail safe");
        lastAllOff = millis();
      }
}