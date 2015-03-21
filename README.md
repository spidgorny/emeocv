emeocv
======

Read and recognize the counter of an electricity meter with OpenCV.

Forked from @skaringa with MySQL-DB feature.

![OpenCV for the electricity meter](http://www.kompf.de/cplus/images/emeocv_m.png)


Prerequisites
=============

* g++, make
* OpenCV 2 (Debian/Ubuntu/Raspbian: `apt-get install libopencv-dev`)
* MySQL-Client (`apt-get install mysql-client libmysqlclient-dev`)
* log4cpp (`apt-get install liblog4cpp5-dev`)

Build
=====

    cd emeocv
    make

Usage
=====

    emeocv [-i <dir>|-c <cam>] [-l|-t|-a|-w|-o <dir>] [-d <schedule>] 
            [-f <image>] [-s <delay>] [-v <level>] [-p]

    Image input:
      -i <image directory> : read image files (png) from directory.
      -c <camera number> : read images from camera.

    Operation:
      -a : adjust camera.
      -o <directory> : capture images into directory.
      -l : learn OCR.
      -t : test OCR.
      -w : write OCR data to MySQL database. This is the normal working mode.

    Options:
      -d <t> : ONLY WORKS WITH OPERATION "-w"! If time is divisible without 
                remainder by t, write to DB. e.g.: 1h = every full hour or 
                10m = every full 10 minutes. (t=<number>[h|m|s]) (default=None).
      -f <image> : ONLY WORKS WITH OPERATION "-w"! Output capture image to static
                path, when OCR data is written to DB. (e.g.: image=<dir>/<file.png>).
      -s <n> : Sleep n milliseconds after processing of each image (default=1000).
      -v <l> : Log level. One of DEBUG, INFO, ERROR (default).
      -p : Print log also in Console (default=False)


There is a tutorial in german language that explains use case and function of the program: [OpenCV Praxis: OCR für den Stromzähler](http://cplus.kompf.de/emeocv.html).

License
=======

* Copyright 2015 Martin Kompf
* Copyright 2015 Matthias Freund (MAFLO321)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
