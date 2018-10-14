Simple Parallel Processes (spp)
========
Simple Parallel Processes, or spp for short, is a simple tool
that can be used in Bash-style scripts on any OS with modern
C++ support to easily parallelize long tasks. For example,
it can be used to parallelize a large set of videos to transcode.
It works by taking in a list of commands to execute, with one 
command per line in a text file, and a number of commands to run
simultaneously. For details on its use, see the rest of this
README, which also serves as the official manual.

License
-------
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.


Installation
------------
spp supports any platform with a full C++11 implementation, 
including C++11 threads and is very simple to install on any 
supported platform. Such platforms include Ubuntu 16.04, 
Arch Linux, and recent versions of Windows. To install, simply
take the threader.cpp file and call your c++ compiler as follows:
    
    clang++ threader.cpp -o spp -pthread

The above command is for clang on Linux, but it can be adjusted
for any conformant compiler and implementation. For example,
to compile with GCC on Linux, simply replace `clang++` with `g++`.
After compiling, you may place the resulting spp binary in your
`/bin`, or wherever else you want. 

You may have noticed that a CMakeLists.txt file is provided. You
may also use this to compile spp, but it is not recommended due
to the increase difficulty and overhead unless you want to work
on spp in an IDE.

Usage
-----
To use spp, the first step is to create a text file with the set
of commands you want to run, with one command per line. These
commands will be passed to the shell that spp is run in, so you
can use things like `|` or `>>`. spp is allowed to run these
commands in any order, so ensure that no command depends on
any other. You may also, optionally, create a script to create
the command list file. The following is an example of one such
script:

    for f in ./*
    do
    	echo "ffmpeg -n -i \"$f\" -c:v vp9 -c:a vorbis -strict -2 \"$f.mkv\"" >> ../list
    done
    
The above example creates a command list to convert a set of videos
to a free format.

After your command list is created, running spp is simple. The
command line interface is:

    spp </path/to/command/list> [threads]
    
You must always specify the path to the command list, but you may,
optionally, leave the threads argument blank, in which case spp
will use the number of hardware threads available (cores on AMD
CPUs and hyperthreads on Intel CPUs). Running the above command
will execute all of the listed commands, executing `[threads]` 
at a time.

Once spp is running, it will, every 5 seconds, output a status
update with how many tasks have been launched. You may, at any
time, type `cancel` into the terminal spp is running in, after
which no new commands will be launched, and spp will exit after
all already-launched commands finish. It is considered undefined
behaviour if any commands running in spp interact with stdin, as
spp uses stdin to receive the aforementioned `cancel` command.
This is why the `-n` flag is given to ffmpeg in the above example,
to ensure that it will automatically skip over already-existing
files instead of prompting the user for what to do.

Isn't this just GNU Parrallel or \<insert other program here\>?
-------------------------
There are other programs, most notably GNU Parrallel, that seek
to accomplish similar goals. spp is different from these programs
in that it is very simple to use, where reading just the Usage
section of this document is sufficient to learn it. Of course,
this means that it is less powerful, but it also sets it apart as
a different, and still useful, tool. spp's simplicity, with only a
single file of source code also allows users to easily modify it
to accomplish what they want, if it isn't available by default.