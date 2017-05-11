lmerge
========

This is a simple POSIX command that fairly merges the output of
multiple line-oriented streams into a single stream.

Usage
-----

    $ lmerge <(yes foo) <(yes bar) | head

lmerge also accepts input from standard input as well.

    $ yes foo | lmerge <(yes bar) <(yes baz) | head

Build
-----

Building lmerge requires a standard C++11 compiler and a standard
POSIX build environment. You can build it on the command line as
follows:

    $ make

Copyright
---------

Copyright (c) 2017 Rian Hunter <rian@alum.mit.edu>

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
