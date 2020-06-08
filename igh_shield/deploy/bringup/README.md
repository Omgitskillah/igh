# HARDWARE BRINGUP

## Boron Pinout
The following pin configurations are assumed for the bringup test.
|Pin Name  | Pin Num |
|:---      | :---    |
|RFM69_CS  |   A2    |
|RFM69_G0  |   D8    |
|RFM69_RST |   D6    |
|RFM69_EN  |   N/C   |
|SD_CARD_CS|   A5    |
|VALVE_CTRL|   D7    |
|IRR_BUTTON|   A4    |
|SD_MISO   |   D11   |
|SD_MOSI   |   D12   |
|SD_SCK    |   D13   |
|RFM69_MISO|   D4    |
|RFM69_MOSI|   D3    |
|RFM69_SCK |   D2    |


The bringup firmware considers all the peripherals and resources of the device under test and lets the development/engineering team test the hardware so that production may be done parallel to the firmware development.  

## Programming the Device
1. prepare a computer running Windows
2. Install the lates version of Particle CLI for windows by following the instruction at this [link](https://docs.particle.io/tutorials/developer-tools/cli/)
3. Connect a fully built Shield to the computer over USB
4. Press both the ```RESET``` and ```MODE``` buttons on the Boron, release only the ```RESET``` button then and wait for the Boron to start blinking yellow after it blinks magenta then release the ```MODE``` button immediately the device starts blinking yellow
5. Open a ```CMD terminal/PowerShell``` at from the folder containing ```igh_shield_bringup.bin```
6. Enter the following command ```particle flash --usb igh_shield_bringup.bin``` then press enter
7. After successfully programming the device, the terminal will display a ```Flash success!``` message and will reset the board automatically.

## Running Device BringUp
8. Install CoolTerm, a terminal software from the following [link](https://freeware.the-meiers.org/CoolTermWin.zip) to the production computer.
9. Open CoolTerm
10. Click on ```Options``` on the top menu bar and select the ```COM``` port number for the Boron
11. Set the Baudrate to ```115200```
12. Click ``OK```
13. Click on ```Connect``` to start a bringup session
14. Enter `?` into the terminal and wait for a resposnes with all possible tests.
15. To run a specific test, enter into the terminal the index value, you will not need to press enter. The device will start the test for the specified number entered. The test available include:  
```
- ?. Print Options
- 1. Get Device ID
- 2. Test Device API
- 3. Test EEPROM
- 4. Test Irrigation Button
- 5. Test Valve Control
- 6. Test SD Card
- 7. Test Radio
``` 

### **4. Test Irrigation Button**
When testing the irrigation button, the device will give you instructions on how long to press the button. For the test to pass, you must hold the button for at least the number of seconds required.

### **5. Test Valve Control**
In order to test the valve, the device will toggle the valve to the open state, wait for the valve to fully open, the closes the valve and waits for it to full close before passing or failing the test.

### **6. Test SD Card**
In order to test the SD card, install an SD card into the device. It is assumed that once the SD card is installed, it will never be removed for the device's lifetime. Make sure the SD card is formated to ```Fat16``` or ```Fat32``` using a windows machine before installing it into the device. During this test, a file is written to the sd card then deleted. if this sequence is successful, then the result is true

### **7. Test Radio**
To test the Radio, you will need a second device with a radio running the ```igh-rfm-test-device.ino``` from the Particle console. The second radio may beused for all the devices under test and does not need any further interruction other than keeping it on. You may, however, choose to connect the second device to c terminal app to read serial data. In this case, the device will always print out the radio ID of the device under test and confirm to you that the device is infact sending radio data or not. You may also use this to do a range test if necessary. For this test, the device under test sends a message to the radio device and expects a response back. The test passes if a valid response is sent back to the device under test.

## _Notes_
* Using more advanced terminals like DockLight in place of CoolTerm, it is possible to automate the test and generate test results for quality control purposes of the devices.  
* A similar solution shall be presented for the Spear bringu