# Author: Nikolas Karakotas
param (
	#[Parameter(Mandatory=$true)][string]$FileName = "blinky.bin",
	[string]$BinFile = $(throw "BinFile parameter is required."),
	[string]$BinPath = $(throw "BinPath parameter is required."),
	[string]$IniFile = "oem_app_path.ini",
	[Int]$Timeout = 60,
	[Int]$Ack = 0,
	[string]$ComPort = "",
	[bool]$ResetTarget=$false
)


# .\push-app.ps1 -BinPath '..\workspace\apps\blinky\bin' -BinFile blinky.bin -ResetTarget $True

function Send {
	param($port, $command, $file, $binary=$false, $commandonly=$false)

	$port.WriteLine($command)
	start-sleep -m 250
	$resp = $port.ReadExisting()
	Write-Host $resp


	if($resp -match 'CONNECT' -and $commandonly -eq $false ){
		Write-Host 'Received CONNECT'
		Write-Host 'Uploading File...'
		if($binary){
			$Start = 0
			$Length = 0
		 	$resolvedPath = Resolve-Path -Path $file
            $fileStream = New-Object -TypeName System.IO.FileStream -ArgumentList ($resolvedPath, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read)
            $fileReader = New-Object -TypeName System.IO.BinaryReader -ArgumentList $fileStream
            if ($Length -eq 0) {
                $Length = $fileReader.BaseStream.Length
            }
        
            $fileReader.BaseStream.Position = $Start
            $bytes = $fileReader.ReadBytes($Length) 
            $fileReader.Close
            $fileStream.Close
            $port.Write($bytes, $Start, $Length)
		}
		else{
			$data = Get-Content -Path $file 
			$port.Write($data)
		}


		$resp = $port.ReadLine()
		Write-Host $resp
		$resp = $port.ReadLine()
		Write-Host $resp
		$resp = $port.ReadLine()
		Write-Host $resp

		if($resp -match 'OK'){
			Write-Host 'Upload completed!'
			return $true
		}else{
			return $false
		}

	}else{
		return $false,$resp
	}

}

if(-Not ($BinFile -match '.\.bin')){
	throw "Not a binary file, must be a *.bin!"
}

$filebin = ($BinPath + '\' + $BinFile)
$fileini = ($BinPath + '\' + $IniFile)
if(-Not (Test-Path $filebin )){
		throw "BinPath doesnt exist!"
}


$serial_port = Get-PnpDevice -Class 'Ports' -Status 'OK' | Select-Object -Property FriendlyName | Select-String -Pattern 'Quectel USB AT Port'


$datatxbinfile = 'EUFS:/datatx/' + $BinFile
$datatxinifile = 'EUFS:/datatx/' + $IniFile
$binfilesize = (Get-Item $filebin).Length
$inifilesize = (Get-Item $fileini).Length

$at_cmd_file_upload = 'AT+QFUPL=' + '"' +$datatxbinfile + '"' + ',' + $binfilesize + ',' + $Timeout + ',' + $Ack + "`r"
$at_cmd_file_delete = 'AT+QFDEL=' + '"' +$datatxbinfile + '"' + "`r"

$at_cmd_ini_upload = 'AT+QFUPL=' + '"' +$datatxinifile + '"' + ',' + $inifilesize + ',' + $Timeout + ',' + $Ack + "`r"
$at_cmd_ini_delete = 'AT+QFDEL=' + '"' +$datatxinifile + '"' + "`r"

$at_cmd_reset = "AT+CFUN=1,1`r"


if($serial_port.Count -eq 1){
	if($serial_port[0] -match '(COM\d+)'){
		
		$com_port = Write-Output $Matches.0
		$port = new-Object System.IO.Ports.SerialPort $com_port,921600,None,8,one

		try{

			$port.open();			
			$resp = Send -Port $port -Command $at_cmd_file_upload -File $filebin -Binary $true
			if($resp -match 'CME ERROR: 407'){
				$resp = Send -Port $port -Command $at_cmd_file_delete -File $filebin -CommandOnly $true
				$resp = Send -Port $port -Command $at_cmd_file_upload -File $filebin -Binary $true
			}

			$resp = Send -Port $port -Command $at_cmd_ini_upload -File $fileini
			if($resp -match 'CME ERROR: 407'){
				$resp = Send -Port $port -Command $at_cmd_ini_delete -File $fileini -CommandOnly $true
				$resp = Send -Port $port -Command $at_cmd_ini_upload -File $fileini
			}

			if($ResetTarget){
				Write-Host 'Reseting Target...'
				$port.WriteLine($at_cmd_reset)
				$port.ReadLine()
				$port.DiscardOutBuffer()
				$port.DiscardInBuffer()
				$port.Dispose() 
				$port.Close()
				exit 0
			}
			
		}catch [System.Exception]{
        	Write-Error ("Failed to connect : " + $_)
	        $error[0] | Format-List -Force
	        if ($port -ne $null) { $port.Close() }
	        	exit 1
    	}finally{

    		$port.Close()
    	}
    }

}else{
	Write-Output 'Quectel USB AT Port Not Found!'
}
