# ExecuteFromRam
This project running CLR (.net framework) v4.0.30319 from encrypt file.
That protect your source code in file (but not protect from ram)

To encrypt: 
```ExecuteFromRam.exe -e WpfApp.exe```  
It create `.encrypt` file.

To run:
`ExecuteFromRam.exe` it will find first file `.encrypt` and run it  
or  
`ExecuteFromRam.exe -r WpfApp.exe.encrypt`  

Change your AES key/salt and remove encrypt for release.
