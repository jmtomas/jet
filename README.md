![C/C++ CI](https://github.com/jmtomas/jet/workflows/C/C++%20CI/badge.svg)

Jet: a remote text editor
==================
This project aims to be a daemon that can be accessed remotely to edit files.

# disclaimer
This project is not even in alpha so everything is subject to change at any moment.

*For now only a linux build is available.*

# dependencies
- ncurses

# build
- make

# usage

- on a terminal execute:

path_to_project/jet #server

- on other terminal execute:

path_to_project/jetc #client

# key bindings
Current key bindings work kinda like vim.

- h: move left
- l: move right
- i: enter insert mode
- C-g: on insert mode it exits to normal mode, on normal mode it exits the client
