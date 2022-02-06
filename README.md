##
# Usage
##

# Selecting and building one of the experiments

Navigate to build directory, you can remove all contents 
(with rm -rf *) in that folder.  Execute 'cmake ../hello_usb'
from that folder, the folder will get populated with the 
project selected, in this case 'hello_usb'. Type 'make'
after cmake has setup the project in the build folder.

# Uploading new build to your Pico - using SWD

Assuming you have a *.cfg set up that works with your 
programmer and rp2040, execute an upload as follows 
with openocd :

openocd -f ../pico.cfg -c "targets rp2040.core0; program hello_usb.elf verify reset exit"

# Debugging with gdb

Start openocd in a terminal window with :

openocd -f ../pico.cfg

Start gdb with the elf file that you'd loaded on another terminal
window with and run the macro shown with it :

gdb -x ../gdbinit ./hello_usb.elf

# Macros in gdbinit

With all wired and hooked up correctly, you can issue command 

mri

to renitialize the monitor reset the device, then

cll

to perform compile, link load.  At that point program will be loaded
and ready to start, set up whatever monitor tool (like logic analyzer
or breakpoints etc.) then press 'c [press enter key]' and your program
runs.  This is a very fast debug cycle with PIO.
