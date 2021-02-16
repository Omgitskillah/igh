# IGH SYSTEM RELEASE SOFTWARE TEST STRATEGY
It is imperative that after each itteration of code, a standard system is subjected to the rigorus regression test routine to verify all the epected features.  

## SYSTEM FEATURES
1. IGH Shield
2. IGH Spear
3. MQTT Broker
## IGH SHIELD HARDWARE FEATURES
1. The shield cotroller board
2. Water contorl valve
3. Water Flow meter
4. RF Comms
5. GSM Comms
6. Button
7. SD Data logging
8. Systems Settings

## IGH SPEAR HARDWARE FEATURES
1. Spear control board
2. RF Comms
3. Soil Humidity and Temperature Sensor
4. Air Humidity and Temperature Sensor
5. CO2 Sesnor
6. NPK Sensor
7. Lux Meter
8. System Settings :heavy_check_mark:

Tests on the devices should be performed using the test suite at this [google sheets](https://docs.google.com/spreadsheets/d/1aRA9xHEJYj3mzZMMTfezHIbYy6dl5QJ9LwqFd2dbHtI/edit?usp=sharing)

Check the box in the `checkbox` column once a test has been confirmed to pass and write the name of the person testing in the `Tester` column.  
A failed test should be left unchecked with the name of the tester still included.  
When a test fails, create a new issue in this [illuminum github repo](https://github.com/Omgitskillah/igh) with the title as the failed test name. Add details of the issue as observed with any attachments where available and then add the Issue number to the `GitHub Issue Number` column in the test case sheet. 

## NOTE
- To add a new test case, add a row below the appropriate section then give the test case an appropriate name.
- To remove a test case, highlight the entire row and delete the row
- Each test suite is an entire sheet on its own and should not be modified after the test is complete. 
- To create a new test suite, duplicate the sheet and rename the new tab with the new date and reference the appropriate firmware version under test. 






