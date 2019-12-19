Release Version: QFLOGv2.0

Please find the steps to set up the CLI as follows:

1. Install Python 2.7.5

2. Install pyserial 3.4 (http://pyserial.readthedocs.io/en/latest/pyserial.html) [pip install pyserial]

3. Add the following paths to the PYTHONPATH environment variable:
	3.1. Path to the project directory(~\QFLOGv2.0)
	3.2. Path to the project source directory (~\QFLOGv2.0\src)	
	3.3. Please find the steps to configure the said environment variable as follows:
		3.3.1.	Go to Computer -> Properties -> Advanced System Settings
		3.3.2.	Click on Environment Variables
		3.3.3.	Create the PYTHONPATH system environment variable if it doesn’t already exist
		3.3.4.	Add the above mentioned paths to the PYTHONPATH environment variable

Please find a brief of the supported commands as follows:

1.	HELLO: Send a HELLO packet to the device
	Usage:
	python QFLOG.py -p <COMPORT> HELLO

2.	PUSH: Push the specified file to the device
	Usage:
	python QFLOG.py -p <COMPORT> PUSH -f <AbsoluteFilepath>

3.	DELETE: Delete the specified file (present at the predefined location) from the device 
	Usage:
	python QFLOG.py -p <COMPORT> DELETE -f <FilenameOnly>
	
4.	VIEW_LOGS: View logs pertaining to the application executing on the device on the console 
	Usage:
	python QFLOG.py -p <COMPORT> VIEW_LOGS
	
	Note that CTRL+C terminates the session	

Release Notes :

1.	QFLOGv2.0 provides backward compatibility with Python 2.7.5