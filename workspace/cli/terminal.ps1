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

