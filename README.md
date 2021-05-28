# Connect_4_GUI
My second programming project ever.
## How to compile ?

**First install MinGW GCC Compiler x64**

### Install SFML 2.5.1

**Download SFML 2.5.1 from this official Link [SFML 2.5.1 x64](https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip)**

**Then un-zip the downloaded file.**

**Now copy _```bin```_ ,_```lib```_ and _```include```_ directories then paste them in main MinGW directory.**

**(You may find it on ```C``` drive)**

**_(Note that these directories must be existed before pasting them in main MinGW directory)_**

_**then just write that piece of code in a consol or a bash in the same Directory of the project.**_

`g++ -c Source.cpp `

`g++ Source.o -o CONNECT_FOUR_GUI -L<sfml-install-path>/lib -lsfml-graphics -lsfml-window -lsfml-system `

`./CONNECT_FOUR_GUI`

**You can just copy them from Commands.txt file !**

_*Tested on **gcc version 10.3.0 (Rev2, Built by MSYS2 project)***_

**Have Fun Dude !**
