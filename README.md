# smstempcontroller
This is firmware for simple network controller, witch read temperature from 1-Wire net and send via sms.

###have a several command:
* search - search all sensors on 1-wire line. Return list of sensors.
* status - return list of sensors with temperature
* setnode num,min,max,phone_or_nothing  num is sensor number in search/status list, min, max is temperature range, phone - destinations, witch want will bring notification about out of range temperature. Node is a rule for temperature range for a sensor and a phone.
*shownode all_num_or_phone - show selected or all nodes via sms
*deletenode all_num_or_phone - is similar


### Hardware
* Simcom SIM900 GSM modem
* Dallas DS4280 1-wire controller
* any dallas 1-wire temperature sensors

###Also
* logging data to SDcatd
* view log on HD44780 lcd screen

###Contacts
sd1995@ukr.net - all questions ask here
