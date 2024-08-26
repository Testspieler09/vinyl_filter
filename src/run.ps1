# run executable from source directory
param()
$OUTPUT_PATHS = "..\build\output.exe", "..\build\output.out", "..\build\output.app"

foreach ($path in $OUTPUT_PATHS) {
	if (Test-Path $path) {
		Start-Process $path -NoNewWindow $args
		exit
	}
}

Write-Host "Could not find executable" -f Red
