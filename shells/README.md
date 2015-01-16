# Shells

I created these Arduino sketches in order to get a command shell on the Galileo, with nothing but a micro USB cable to plug it into a PC. The Galileo already runs getty on ttyS1 (which I believe is the 3.5mm FTDI jack on the Gen1 and the 6-pin TTL on the Gen2), however I didn't have cables for that available. So, I wrote sketches to send commands over USB serial (ttyGS0).

So far I wrote three shells, in the following order. The most advanced is shell3, which I recommend using.

It may be possible to use these on other Arduino boards. Start with shell3 (which requires a POSIX-compliant OS) and work backwards until you find one that works best.

- shell: Very basic terminal emulation. Newline must be sent after each command. Commands are executed using popen() (basically same as system() with stdout pipe) and their output is piped to the serial console. Backspace is supported. Delete, arrow keys, and other advanced functions like Ctrl+C are not supported, so be careful about running commands that don't exit immediately.

- shell2: Runs sh in a child process that will automatically exit when the sketch is reset. All input to sh is piped in from the serial console, stdout and stderr are piped to the serial console. In order to make it as advanced as possible, a few attributes of the serial device are changed (refer to the stty command in spawnShell). Specifically, if the icanon attribute is enabled (to get native support for backspace/delete characters), uploads will fail.

- shell3: Runs sh in a child process that will automatically exit when the sketch is reset. This is the best version of the shell, as it is the most like a native ssh/telnet shell. It allocates a pty (pseudo terminal) device in which to run sh, and pipes all input/output done by the shell over the serial console. By using a pty, it doesn't interfere with the actual serial device (ttyGS0), so it shouldn't cause uploads to stop working.

If uploads stop working after using one of these shell programs, you will have to delete the sketch from the board. If not using an SD card, this will happen automatically when the board is rebooted. If you are using an SD card, just delete /sketch/sketch.elf (or copy /sketch/sketch.elf.old).
