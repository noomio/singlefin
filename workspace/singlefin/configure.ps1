& "C:\Users\nikol\Anaconda2\shell\condabin\conda-hook.ps1"

Start-Process -FilePath "C:\Users\nikol\Anaconda2\python.exe" -NoNewWindow -ArgumentList "tools/configure.py",
"--source-directory","../apps/common/src-input", "--platform", "THREADX", "--architecture", "arm32", "--output-directory","out", "--verbose" -Wait 