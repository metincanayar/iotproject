In this project, I will make an IoT Based Weighing Scale with HX711 Module Load Cell and Arduino Uno. The load cell shall be capable of measuring any weight up to 40 kg. We will display the measured weight on the Nextion HMI and also remotely monitor the weight data on the Bylnk Server.

In this IoT Project we are interfacing 40Kg load cell to the Arduino Uno using the HX711 Load cell amplifier module. HX711 is a precision 24-bit analog to digital converter (ADC) designed for weighing scales and industrial control applications to interface directly with a bridge sensor. The HX711 load cell amplifier is used to get measurable data out from a load cell and strain gauge.

We will make a Weighing Scale Machine which can measure weights up to higher-value like 40KG. We need to calibrate the load cell and find the calibration factor. Once the calibration is done, we can include that factor in our code. Thus this will make the scale precise and accurate. The greater is the mass the greater the error. So we will try to remove the error from the weighing scale. We will finally display the measured weight in the Nextion HMI Display. We will send the so obtained weight value on the IoT Cloud platform called Blynk Application. Thus, weight can be monitored from any part of the world simply by observation on the Blynk app dashboard.

We set up our circuit as shown in circuit diagram file and after uploading the necessary code to Arduino Uno, we upload the interface we designed in Nextion Editor to the SD card in .tft format and attach it to the screen.


