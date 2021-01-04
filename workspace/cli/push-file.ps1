param (
	[string]$Dir = '/ufs/',
	[Int]$Timeout = 60,
	[string]$File = $(throw "A file is required.")
)

if($File -is [System.IO.DirectoryInfo]){
	throw "Please select a file!"
}

$baudrate = 115200

$serial_ports = Get-PnpDevice -Class 'Ports' -Status 'OK' | Select-Object -Property FriendlyName | Select-String -Pattern 'USB Serial Port'


if($serial_ports.Count -ge 1){

	foreach($serial in $serial_ports){

		if($serial -match '(COM\d+)'){
			
			Write-Output "Trying USB Serial Port:" $Matches.0
			$com_port = Write-Output $Matches.0

			$port = new-Object System.IO.Ports.SerialPort $com_port,$baudrate,None,8,one
			# tera term use number
			$tera_port = $com_port -match '(\d+)'
			$tera_port = $Matches.0

			try{

				$port.open();
				Write-Host "Probing target..."
				$port.WriteLine("");	
				start-sleep -m 150
				$resp = $port.ReadExisting()
				
				if($resp -match '>>'){
					Write-Host "Target probed sucessfully."
					Write-Host "Trying to start E-Kermit on target..."
					$port.WriteLine("ek -r")
					start-sleep -m 150
					$resp = $port.ReadExisting()

					if($resp -match 'ek -r'){

						start-sleep -m 150
						$resp += $port.ReadExisting()

						start-sleep -m 150
						$resp += $port.ReadExisting()

						if($resp -match 'E-Kermit: 1.6'){

							Write-Output "E-Kermit 1.6 started on target!"
							Write-Output "Sending file..."
							$port.Close() # close as E-Kermit will need to use it
							start-sleep -m 250
							# Open kermit and send file
							Start-Process -NoNewWindow -Wait -FilePath "./ttermpro.exe" -ArgumentList "/BAUD=115200","/C=$tera_port","/CPARITY=none","/CSTOPBIT=1","/CFLOWCTRL=none","/AUTOWINCLOSE=on","/M=./kermit.ttl"

							#Start-Process -FilePath "./cknker.exe" -ArgumentList
							#"-B","-b","$baudrate","-l","$com_port","-m","none","-p","none","-c","-C",
							#"`"set modem type none, set line COM14, set carrier-watch off, set speed 115200, set flow none, set parity none, set stop-bits 1`"","-s","$File"
							##cknker.exe -C "set modem type none, set line COM14, set carrier-watch off, set speed 115200, set flow none, set parity none, set stop-bits 1, connect" -s $File -d

						}
					}else{
				        $error[0] | Format-List -Force
				        if ($port -ne $null) { $port.Close() }
				        Write-Error ("Failed to start E-Kermit.")
				        exit 1
					}
				}else{
					Write-Output ("Failed to connect : " + $_)
					if ($port -ne $null) { $port.Close() }
				}
				
			}catch [System.Exception]{
		        $error[0] | Format-List -Force
		        if ($port -ne $null) { $port.Close() }
		        Write-Error ("Failed to connect : " + $_)
		        exit 1
	    	}finally{

	    		if ($port -ne $null) { 
	    			$port.Close() 
	    		}

	    		exit 1
	    	}
	    }
    }

}else{
	Write-Error 'USB Serial Port not detected!'
}