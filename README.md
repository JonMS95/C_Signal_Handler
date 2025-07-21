# C_Signal_Handler: a single handler to rule them all

## Table of contents 🗂️
* [**Introduction** 📑](#introduction)
* [**Features** 🌟](#features)
* [**Prerequisites** 🧱](#prerequisites)
* [**Installation instructions** 📓](#installation-instructions)
  * [**Download and compile** ⚙️](#download-and-compile)
  * [**Compile and run test** 🧪](#compile-and-run-test)
* [**Usage** 🖱️](#usage)
* [**To do** ☑️](#to-do)
* [**Related documents** 🗄️](#related-documents)


## Introduction <a id="introduction"></a> 📑
When dealing with multiple signal handlers in a project, it may happen that a signal-handling function is overwritten by another, leading the priorly existing one not to be ever executed.
For these situations to be somehow avoided, a signal handler shall be created. Such handler should register every callback to be executed as well as a mask to indicate which signals should it handle.

By now, it has been designed for it to be run on Linux distros (such as [Ubuntu](https://ubuntu.com/)). The resulting library is a *.so* file alongside a C language header (*.h*) file.


## Features <a id="features"></a> 🌟
In order to get some knowledge about how to use the library alongside its options, go to [Usage](#usage).


## Prerequisites <a id="prerequisites"></a> 🧱
By now, the application has only been tested in POSIX-compliant Linux distros. In these, many of the dependencies below may already come installed in the OS.
In the following list, the minimum versions required (if any) by the library are listed.

| Dependency                   | Purpose                                 | Minimum version |
| :--------------------------- | :-------------------------------------- |:-------------: |
| [gcc][gcc-link]              | Compile                                 |11.4            |
| [Bash][bash-link]            | Execute Bash/Shell scripts              |4.4             |
| [Make][make-link]            | Execute make file                       |4.1             |
| [Git][git-link]              | Download GitHub dependencies            |2.34.1          |
| [Xmlstarlet][xmlstarlet-link]| Parse [configuration file](config.xml)  |1.6.1           |

[gcc-link]:        https://gcc.gnu.org/
[bash-link]:       https://www.gnu.org/software/bash/
[make-link]:       https://www.gnu.org/software/make/
[git-link]:        https://git-scm.com/
[xmlstarlet-link]: https://xmlstar.sourceforge.net/

Except for Make and Bash, the latest version of each of the remaining dependencies will be installed automatically if they have not been found beforehand. 

In any case, installing **_Xmlstarlet_** before executing any of the commands below is strongly recommended. Otherwise, it can lead to error since make file
contains some calls to it at the top. If that happens, just repeat the process (Xmlstarlet would have been already installed).

On top of the ones listed above, there are some *JMS* dependencies (libraries that were also made by myself) that are required for both the library and the test executable to be built,
(although these are managed by the library itself, so no need to download them manually). The required version for each of them is specified by the [config.xml](config.xml) file.

| Dependency                                                              | Purpose                                  |
| :---------------------------------------------------------------------- | :--------------------------------------- |
| [C_Common_shell_files](https://github.com/JonMS95/C_Common_shell_files) | Process [configuration file](config.xml) |
| [C_Severity_Log](https://github.com/JonMS95/C_Severity_Log)             | Show logs during testing stage           |
| [C_Mutex_Guard](https://github.com/JonMS95/C_Mutex_Guard)               | Manage mutexes safely                    |


## Installation instructions <a id="installation-instructions"></a> 📓
### Download and compile <a id="download-and-compile"></a> ⚙️
1. In order to download the repo, just clone it from GitHub to your choice path by using the [link](https://github.com/JonMS95/C_Signal_Handler) to the project.

```bash
cd /path/to/repos
git clone https://github.com/JonMS95/C_Signal_Handler
```

**Note**: by default, the path to the repository should be found within a directory (found in Desktop) called "scripts" (it's to say, **_~/Desktop/scripts/C_**). In order to modify this, change the paths specified in [config.xml](config.xml). If dependencies are not found in the specified path, the path itself would be created, then dependencies will be compiled there. 

2. Then navigate to the directory in which the repo has been downloaded, and set execution permissions to every file just in case they have not been sent beforehand.

```bash
cd /path/to/repos/C_Signal_Handler

find . -type f -exec chmod u+x {} +
```

3. For the library to be built (i.e., clean, download dependencies and compile), just type the following:

```bash
make
```

The result of the line above will be a new API directory (which will match the used version). Within it, a *.h* and a *.so* file will be found.
- **/path/to/repos/C_Signal_Handler/API**
  - **vM_m**
    - **lib**
      - **_libSignalHandler.so.M.m_**
    - **inc**
      - **SignalHandler_api.h_**

Where **_M_** and **_m_** stand for the major and minor version numbers.
**SignalHandler_api.h_** could also be found in **_/path/to/repos/C_Signal_Handler/src/SignalHandler_api.h_** although it may differ depending on the version.


### Compile and run test <a id="compile-and-run-test"></a> 🧪
For the test executable file to be compiled and executed, use:

```bash
make test
```

Again, the one below is the path to the generated executable file:
- **/path/to/repos/C_Signal_Handler/test**
  - **exe**
      - **_main_**
  - src
  - deps


## Usage <a id="usage"></a> 🖱️
Store new signal-handling function:
'''c
C_SIGNAL_HANDLER_API int SignalHandlerAddCallback(void (*cb)(const int sig_num), const uint16_t sig_mask);
'''

Erase every priorly installed callback:
'''c
C_SIGNAL_HANDLER_API void SignalHandlerResetCallbacks(void);
'''

Get error code:
'''c
C_SIGNAL_HANDLER_API int SignalHandlerGetErrorCode(void);
'''

Get error string (given an error code):
'''c
C_SIGNAL_HANDLER_API const char* SignalHandlerGetErrorString(const int error_code);
'''

Get latest error string (defined macro):
'''c
SIG_HDL_GET_LAST_ERR_STR
'''

## To do <a id="to-do"></a> ☑️


## Related Documents <a id="related-documents"></a> 🗄️
* [LICENSE](LICENSE)
* [CONTRIBUTING.md](Docs/CONTRIBUTING.md)
* [CHANGELOG.md](Docs/CHANGELOG.md)

