**Svenska:** Det här programmet skapar ett skript för att installera en skrivare. Endast scriptet som programmet genererar ska du ge ut till användaren, ej hela programmet.
När du kör programmet, skapas en mapp som heter 'PrintSetup' i samma mapp där programmet finns. Till exempel, om programmet ligger i 'folder/program.exe', så skapas mappen 'folder/PrintSetup'. Inuti den mappen skapas en fil, till exempel 'folder/PrintSetup/script.bat', som används för att installera skrivardrivrutinen.

Om du inte anger en specifik drivrutin, använder programmet Windows standarddrivrutin (Microsoft PS Class Driver). Om du vill lägga till en specifik drivrutin måste den vara en ensam .exe-fil utan några andra filer (om drivrutinen är en mapp med flera filer bredvid en "sharp_printer_driver.exe" glöm den). Denna drivrutin läggs också i 'PrintSetup'-mappen och används av skriptet för att installera skrivaren med rätt namn och IP-adress.

Väldigt viktigt att veta, programmet tar inte hänsyn till felskrivningar. Så det är fullt möjligt att skriva t.ex "hallojsan" som IP.
Detta är pga. att om ip:n skulle ligger under en domän (av någon anledning) så "borde" den funka.  
Så skriv rätt, annars blir det inte rätt.

**För att kompiliera:** ladda ner en kompilator t.ex. TDM-GCC (https://jmeubank.github.io/tdm-gcc/download/).
Skriv detta i cmd/powershell (du får ta hänsyn vart cmd körs ifrån och justera sökvägarna efter det):

gcc -o "output filens namn (exe)" "källkodens filnamn" -lcomdlg32

inga citat tecken, såklart.

-------------------------------------------------------

**English:** This program creates a script to install a printer. You should only give the script that the program generates to the customer, not the entire program.
When you run the program, a folder called 'PrintSetup' is created in the same folder where the program is located. For example, if the program is in 'folder/program.exe', the folder 'folder/PrintSetup' is created. Inside that folder, a file is created, for example 'folder/PrintSetup/script.bat', which is used to install the printer driver.

If you do not specify a specific driver, the program uses the Windows default driver (Microsoft PS Class Driver). If you want to add a specific driver, it must be a single .exe file without any other files (if the driver is a folder with several files next to a "sharp_printer_driver.exe" forget it). This driver is also placed in the 'PrintSetup' folder and is used by the script to install the printer with the correct name and IP address.

Very important to know, the program does not take into account misspellings. So it is quite possible to write e.g. "hallojsan" as IP.
This is because if the IP were to be under a domain (for some reason) then it "should" work. 
So write it correctly, otherwise it will not be correct.

**To compile:** download a compiler e.g. TDM-GCC (https://jmeubank.github.io/tdm-gcc/download/).
Type this in cmd/powershell (you have to take into account where cmd is run from and adjust the paths accordingly):

gcc -o "output file name (exe)" "source code file name" -lcomdlg32

no quotes, of course.
