& "C:\Users\nikol\Anaconda2\shell\condabin\conda-hook.ps1"

Start-Process -FilePath "C:\Users\nikol\Anaconda2\python.exe" -NoNewWindow -ArgumentList "tools/configure.py",
"--source-directory","../apps/common/src-input", "--platform", "threadx", "--architecture", "arm32", "--module","bg96", "--output-directory","build/fin", "--verbose" -Wait 