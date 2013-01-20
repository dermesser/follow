README for follow
=================

USE
---

follow is a quite simple program using inotify for following a file, e.g. a logfile in which a server writes.
It has the same effect like `tail -f` on a file, but it echoes the whole file before starting to follow, and it's very small.

It was originally written as "glue" between the file-oriented IRC client ii and a perl script so you could follow the chat in the perl script
by opening a pipe like this:

~~~perl
open(CHAT,"follow out|");
~~~

By reading and writing raw bytes, it may be used with any encoding.

If you use follow on your console, simply stop it via CTRL-C (^C), like `tail -f`

COMPILING
---------

    $ make follow

INSTALLING
----------

    # install follow /usr/local/bin

COMMAND LINE ARGUMENTS
----------------------
    -t : Produce timestamps after every newline being written as last character (newlines within a larger block of data written at once are not timestamped)
