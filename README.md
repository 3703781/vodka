A baremetal project generating program on cortex-m7 of s32g3 with RTD4.0.2

# Steps to build

1. Install prerequisites
    ```bash
    sudo apt install bear make
    ```

2. Check that libncurses5 installed:
    ```bash
    sudo apt install libncurses5
    # or
    sudo dnf install ncurses-libs
    ```
    It may be a little tricky on fedora 39+ or ubuntu 23+, you can search for legacy supports on https://packages.fedoraproject.org/ and https://packages.ubuntu.com/

    \[Optional\] For a fluent experience of coding with vscode, install `clangd` extension by llvm.org with prerequisite of an installed `clangd`:
    ```bash
    sudo apt install clangd
    # or
    sudo dnf install clang clang-tools-extra
    ```
    \[Optional\] Download GNU baremetal cross-toolchain (arm-none-eabi-xxx), and then extract the tarball or install the bin to wherever you like.
    ```bash
    wget https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
    tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz -C /wherever/you/like
    ```
    The GNU cross-toolchain is also optional for that STM32CubeCLT describe in the next step contains an instance of arm-none-eabi-xxx

3. Install STM32CubeCLT from st.com
    To visit https://www.st.com/en/development-tools/stm32cubeclt.html to download a deb or rpm bundle, extract to a shell script and excute.
    

4. Customize config/config.mk
   - set `ABSPROJDIR` to absolute path of this project, ending with a slash (/)
   - set `PREFIX` to path and prefix of bearmetal cross-toolchain
   - set `CUBEPROGDIR` to parent dir of `STM32_Programmer_CLI` executable of STM32CubeCLT, ending with a slash (/)
   - choose `TGTIMG` from `sdcard`, `intflash`, or `emmc`

5. Customize config/vsclaunch.json
   - set `configurations[1]` -> `stm32cubeprogrammer` to where `STM32_Programmer_CLI` located
   - set `configurations[1]` -> `armToolchainPath`to where the shipped GNU cross-toolchain located

6. Build && Flash
    ```bash
    make defconfig

    make
    # or if you have bear 3.0+ installed
    make bear

    make install
    ```

# Build Commands

If it is a brand new build, or after a fully clean, project specific configrations and information are desired to achieve enhanced comportment of:
  - code completion
  - compile errors and warnings
  - go-to-definition and cross references
  - hover information and inlay hints
  - include management
  - code formatting
  - simple refactorings

First to generate vscode settings
```bash
make defconfig
```

To generate or refresh compile_commands.json, build an additional target `bear`
```bash
make bear
```

After this, images are generated in m7proj-h7 folder varing depending on the boot device you choose

Incremental build
```bash
make
```

Clean
```bash
make clean
```

Fully clean: all host and arm targets, compile_commands.json, vscode workspace settings
```bash
make distclean
```

# Build system

The build system for m7proj-h7 is built upon GNU Make and is divided into two parts: a host build system responsible for building host tools (host-tools domain, HD), and a build system dedicated to cross-compiling for the target board (target-board domain, TD). The top-level Makefile specifies the "all" target to sequentially build host domain and target-board programs. Therefore, executing the `make` command in the project's root directory is sufficient to compile the entire project.

In essence, when executing the top-level Makefile, it scans all immediate subdirectories that are set by `dirs` variable, in which a Makefile must be contained. It then proceeds to enter these directories and execute the sub Makefile. After the build process in each subdirectory concludes, a *tmp.make* file is generated in each sub directory. *tmp.make* records the compilation results that need to be communicated to the Makefile in the parent directory.

In a simplified explanation (actual it is slightly more complex), the *tmp.make* file contains a list of generated files from the compilation. The Makefile in the parent directory finally reads the compilation results from each subdirectory, incorporating them into the necessary operations for the compilation of the parent directory.

The goals for building HD and TD differ significantly, encompassing distinctions in toolchains, compilation parameters, linking methods, and more. The top-level Makefile is designed as part of the TD build system. To interrupt the top-level Makefile's search and build processes for the *tools* directory, there is no Makefile in the *tools* directory. Instead, compilation is accomplished through the `hosttools` target and its corresponding `cleanhosttools` target in the top-level Makefile. The primary difference compared to automatic search and build lies in controlling the sequence of compilation -- hosttools in HD are compiled first, followed by the TD. Similarly, directories not involved in the compilation process can be excluded from the build directory tree by not placing a Makefile in them, such as the *config* directory.

Here is a template for a Makefile that you can place in your new directory. Additionally, you'll need to update the dirs variable in the Makefile of the parent directory to include the new directory.

```makefile
this_file := $(lastword $(MAKEFILE_LIST))

objs := a.o b.o c.o
confs := d.h.in e.c.in
dirs := dir_1/ dir_2/
softlink_froms := \
relative/path/to/*.c \
relative/path/to/aaa.h \
relative/path/to/bbb.h \
relative/path/to/ccc.h.in \
relative/path/to/ddd.h.in \
relative/path/to/folder/

EXTRA_CFLAGS_SELF := -I$(this_dir) -Wstrict-prototypes
EXTRA_CFLAGS_DOWN += -I$(this_dir) -I$(ABSPROJDIR)lwip/apps

CONFIG_SELF_no_need_escape := 0-9 A-z <SP> ~ ! @ % ^ & * ( ) _ + ` - = { } | [ ] \ : " ; ' < > ? , . /
CONFIG_SELF_need_escape := \# and $$
major_version = $(shell $(HOSTCC) -dumpfullversion | cut -f1 -d.)
CONFIG_DOWN_major_define = \#define GCC_MAJOR "$(major_version)"
CONFIG_DOWN_my_define :=

include $(obj_mk)
```

In this context, the `objs` variable contains the C and assembly source code files that need to be compiled in the CURRENT Makefile directory. These source code files have their suffix changed to ".o" and are added to the `objs` variable list. During the build process, these source files are compiled into binary object files. The `confs` variable in the CURRENT Makefile directory contains CMake configure files that need processing, identified by the ".in" file name suffix. Before building the current directory, these files undergo processing to generate versions without the ".in" suffix. After removing the suffix, if a file is identified as a C or assembly source code file (by suffix .c or .s), it is automatically included in the list specified by the `objs` variable for compilation. Therefore, there is no need for additional manual addition to the `objs` variable. The `dirs` variable specifies the subdirectories that need to be entered for building. It is important to note that the entries in `dirs` MUST end with "/", and a Makefile file must exist in each of the specified subdirectories. 

Reviewing the replacement rules for CMake configured files, as excerpted from the CMake documentation:

CMake can substitutes variable values referenced as @VAR@, ${VAR}, $CACHE{VAR} or $ENV{VAR} in the input file content. Each variable reference will be replaced with the current value of the variable, or the empty string if the variable is not defined. Furthermore, input lines of the form `#cmakedefine VAR ...` will be replaced with either `#define VAR ...` or `/* #undef VAR */` depending on whether `VAR` is set in CMake to any value not considered a false constant by the if() command. The "..." content on the line after the variable name, if any, is processed as above. Unlike lines of the form `#cmakedefine VAR ...`, in lines of the form `#cmakedefine01 VAR`, `VAR` itself will expand to `VAR 0` or `VAR 1` rather than being assigned the value `...`. Therefore, Lines of the form `#cmakedefine01 VAR` will be replaced with either `#define VAR 0` or `#define VAR 1`. 

The functionality described is implemented by *tools/genconfig*. During the project build process, it preprocesses the CMake configured files based on the `confs` variable and variables starting with "CONFIG_SELF_" and "CONFIG_DOWN_". This preprocessing follows the rules described earlier for CMake configured files. The difference lies on that `CONFIG_SELF_` signifies that a particular configuration applies exclusively to the ".in" files specified in the current Makefile and, on the other hand, `CONFIG_DOWN_` indicates that the configuration extends its effect to the Makefile in the current directory and all its subdirectories, including subdirectories of subdirectories, and so forth (in a recursive manner). In the Makefile example above, 4 "CONFIG_SELF/DOWN_"s are set. Assuming the file to be processed is *e.c.in* in current directory with the following content:

```c
#cmakedefine01 my_define
#cmakedefine not_defined
#define ABC abcde@no_need_escape@12345
#define DEF ${need_escape}
@major_define@
```

*e.c* is generated as follows:

```c
#define my_define 1
/* #undef not_defined */
#define ABC abcde0-9 A-z <SP> ~ ! @ % ^ & * ( ) _ + ` - = { } | [ ] \ : " ; ' < > ? , . /12345
#define DEF # and $
#define GCC_MAJOR "13"
```

Note that no escape needed in the Makefile expect for `#` and `$` characters, both of which are escaped as Makefile literal values. This means variables in Makefile can be dereferred in a common manner like `CONFIG_DOWN_major_define` does. Furthermore, the "lazy" assignment operator `=` will be excuted exactly before ".in" file can be processed.

The `softlink_froms` variable is more succinct, comprising relative paths from existing files to the project's root directory. Before compilation, all files listed in the `softlink_froms` variable are symbolically linked to the directory containing the current Makefile. The build system automatically categorizes each symbolic link into the `objs`, `confs`, and `dirs` variables based on file extensions, eliminating the need for manual additions to corresponding variables. During the build process, the system autonomously computes dependencies for all required files and symbolic links. For instance, in the case of header files not explicitly included in C source code, even if specified in `softlink_froms`, symbolic links are not created to maintain simplicity whenever possible. If there are features in the above Makefile template that are not needed, you can either leave them empty or delete them outright. However, it is imperative to retain the `this_file` at the beginning and the `include` at the end of the template.

If the current Makefile belongs to the TD, the last line should be written as `include $(obj_mk)`; if it belongs to HD, the last line should be written as `include $(hostobj_mk)`. The term "obj" indicates the targets to be generated in the current directory tree (referred to as the current directory tree, CDT) rooted at the directory containing the current Makefile. These targets include all binary OBJECT files specified by the `objs` in the entire subtree of the project directory tree, parsed by the build system based on the settings in various Makefiles in the project (referred to as the project directory tree, PDT). This tree is constructed by traversing from the root node (current directory) in a depth-first manner. The traversal stops on each branch upon encountering the first target that is not of the OBJECT type, treating the corresponding directory as a leaf node for that branch. If no non-OBJECT targets are encountered, the search continues deeper until reaching the leaf nodes of the project directory tree, treating those as leaf nodes for the current branch. After constructing the entire CDT, a *tmp.make* file is generated in the root node of CDT as a result of the bottom-up propagation. This process involves merging the contents of *tmp.make* files from all subdirectories with the content specified by `objs` in the current Makefile, and writing the result into the *tmp.make* file in the current directory. The entire bottom-up propagation concludes with the creation of *tmp.make* in the current directory, and this directory participates in upward propagation as needed. It is essential to note that all directory trees belonging to HD cannot intersect with any nodes in the PDT, except for the project root directory node where the top-level Makefile is located.

In addition to setting `include $(obj_mk)` and `include $(hostobj_mk)`, you can also configure `include $(target_mk)` and `include $(hosttarget_mk)`. The term "target" denotes the generation of non-OBJECT type targets. These targets are created in the current directory through linking or archiving, resulting in SHARED, STATIC, MODULE, or EXECUTABLE type targets, with the specific type determined by the "target" variable:
- `target := xxx.a` signifies archiving to produce a STATIC target with the filename xxx.a.
- `target := xxx.so` signifies dynamic linking to generate a SHARED target with the filename xxx.so. NOT supoported in TD.
- `target := xxx.so.module` signifies dynamic linking to create a MODULE target with the filename xxx.so. NOT supoported in TD.
- `target := xxx` indicates linking to generate an EXECUTABLE file with the filename xxx.

Here is an example Makefile of HD to generate a STATIC target named "libmylibrary.a" in its directory:
```makefile
this_file := $(lastword $(MAKEFILE_LIST))
target := libmylibrary.a
objs := ...
confs := ...
dirs := ...
softlink_froms := ...

EXTRA_CFLAGS_SELF := ...
EXTRA_CFLAGS_DOWN += ...

CONFIG_SELF_...
CONFIG_DOWN_...
...

include $(hostobj_mk)
```

Up to this point, we have comprehensively covered the four types of targets: OBJECT, STATIC, SHARED, MODULE, and EXECUTABLE. These align with the concepts in CMake: An OBJECT library compiles source files but does not archive or link their object files into a library. STATIC libraries are archives of object files for use when linking other targets. SHARED libraries are dynamically linked and loaded at runtime. MODULE libraries are plugins that are not linked into other targets but may be loaded dynamically at runtime using functionality similar to dlopen. EXECUTABLE represents an executable file.

The concepts of STATIC, SHARED, MODULE, and EXECUTABLE types in the CDT (Current Directory Tree) are analogous to the previously described OBJECT type. This entails starting from the current directory where the Makefile is located as the root node, conducting a depth-first search, and stopping when encountering a node of a different type or a leaf node in the PDT (Project Directory Tree). The node causing the termination is designated as the leaf node for that branch. As the *tmp.make* file generated at the root node of the CDT continues to be reported to the corresponding parent node in the PDT when needed, the EXECUTABLE type is not suitable as a child node at any position in the PDT (EXECUTABLE types should serve as the root node of the PDT).

In addition, after generating STATIC, SHARED, MODULE, and EXECUTABLE targets, the content of the *tmp.make* file in the same directory is modified to reflect the generated targets. This ensures that only the targets themselves are exposed to the parent nodes in the PDT, unlike OBJECT-type targets that aggregate all ".o" files generated. When linking SHARED or MODULE targets in subdirectories, the build system automatically sets the rpath. As a result, there is no need to configure runtime environment variables such as `LD_LIBRARY_PATH` after the build is complete, and there's no requirement to install any targets into the system's library directory.

When executing the compilation command, additional compilation parameters can be passed to the compiler through the `EXTRA_CFLAGS_SELF` and EXTRA_CFLAGS_DOWN variables. The distinction lies in the fact that `EXTRA_CFLAGS_SELF` is effective only for all targets compiled by the current Makefile (i.e., the objs resulting from symbolic links and automatic dependency analysis, along with the specified target). On the other hand, `EXTRA_CFLAGS_DOWN` is effective for all targets compiled by the current Makefile as well as those of Makefile in all child nodes in the entire CDT.

Bellow are some simple examples.

## To create a new sources directory that contains .c files:

- Create the new dir (e.g. m7proj-h7/board/newdir/)
- Modify the Makefile in its parent folder (m7proj-h7/board/Makefile) - to append `newdir/` (ending with a slash "/") to `dirs` variable

    ```makefile
    # m7proj-h7/board/Makefile
    this_file := $(lastword $(MAKEFILE_LIST))
    objs :=
    confs :=
    dirs := newdir/
    softlink_froms :=
    EXTRA_CFLAGS_SELF :=
    EXTRA_CFLAGS_DOWN +=
    include $(obj_mk)
    ```
- Create a Makefile in the new directory:
    
    ```makefile
    # m7proj-h7/board/newdir/Makefile
    this_file := $(lastword $(MAKEFILE_LIST))
    objs := a.o b.o
    confs :=
    dirs :=
    softlink_froms :=
    EXTRA_CFLAGS_SELF :=
    EXTRA_CFLAGS_DOWN +=
    include $(obj_mk)
    ```

## To create a new include directory that contains .h files:

Add includes to the ENTIRE PDT:
- Create the new dir (e.g. m7proj-h7/board/myinclude/)
- Modify m7proj-h7/config/config.mk for TD - to append it to `CINCS` variable:
    
    ```makefile
    CINCS := ... \
    ... \
    -I$(ABSPROJDIR)board/myinclude
    ```

    where the `ABSPROJDIR` refers to absolute path of this project set in the previous build steps. Note that no "/" is between `$(ABSPROJDIR)` and `board`

- Modify m7proj-h7/config/hostconfig.mk for HD - to append it to `CINCS` variable:
    
    ```makefile
    HOSTCINCS := ... \
    ... \
    -I$(ABSPROJDIR)board/myinclude
    ```

Add includes to CDT rooted from m7proj-h7/board/newdir:
- Create the new dir (e.g. m7proj-h7/board/myinclude/)
- Create a Makefile in m7proj-h7/board/newdir, and append myinclude to `EXTRA_CFLAGS_DOWN`:
    
    ```makefile
    # m7proj-h7/board/newdir/Makefile
    this_file := $(lastword $(MAKEFILE_LIST))
    objs :=
    confs :=
    dirs := 
    softlink_froms :=
    EXTRA_CFLAGS_SELF :=
    EXTRA_CFLAGS_DOWN += -I$(ABSPROJDIR)board/myinclude
    include $(obj_mk)
    ```
