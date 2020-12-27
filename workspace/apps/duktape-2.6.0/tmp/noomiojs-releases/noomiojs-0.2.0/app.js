
var count = 0;
var intervalId;


dout = DigitalOut(PIN4); 
var val = false;

setTimeout(function cb() {
   print('one shot timer, called after 10 seconds');
}, 10000);


intervalId = setInterval(function () {

    print('interval', ++count);
    
    val = !val;
    dout.write(val);

    if (count >= 25) {
    	clearInterval(intervalId);
    }

}, 1000);

print("Hello from Javascript!");



