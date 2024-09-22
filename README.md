# Installation on windows

* First, you need to install git and its tools:

[https://git-scm.com/download/win](https://git-scm.com/download/win)

* Start git shell and get the depot from git:
```
git clone --recursive https://github.com/hubartstephane/Code.git
```
* Go within installation directory and install all dependencies (this will make all required libraries available):
```
install-extra-windows.bsh
```

# Installation on linux

* First, you need to install git and its tools:

```
sudo apt install git
```

* Then get the depot from git

```
git clone --recursive https://github.com/hubartstephane/Code.git
```
* Go within installation directory and install all dependencies (this will make all required libraries available):

```
sudo install-extra-linux.bsh -lib
sudo install-extra-linux.bsh -exe
sudo install-extra-linux.bsh -tools                    (this is not mandatory)
```
