# RISCV toolchain (32 bit)

---
### Dependencies

In order to build the cross toolchain, first install all needed software packages. Additional dependencies will be downloaded and installed during the build process.

	$ sudo apt-get update
	$ sudo apt-get install flex bison libgmp3-dev libmpfr-dev autoconf texinfo build-essential libncurses5-dev

### Build procedure

The `build_riscv_toolchain` script will automate the whole process of downloading, configuring and building basic binary tools and the compiler. All we need to do is create a working directory and copy the build script before starting the whole process.

	$ mkdir riscv-tools && cd riscv-tools
	$ cp ../build_riscv_toolchain .

We need permissions to execute the build script. If not set, change the execute bit of the script:

	$ chmod +x build_riscv_toolchain

And now we are ready to build the cross toolchain. All software packages will be downloaded (binutils, GCC), unpacked and built. Note that we don't need root permissions to do perform this step.

	$ ./build_riscv_toolchain

Now go do something else. The process will take between 15 minutes to 1 hour, depending on your host machine. After the process completes, you can check if the new compiler and tools were built:

	$ ls riscv32-unknown-elf/gcc-8.3.0/bin/

If, among other files, the `riscv32-unknown-elf-gcc` executable is present then we are ok.

### Installation

The tools can be installed in any directory. Most of the time, the toolchain can be shared among users, so it makes sense to install the toolchain in the `/usr/local` directory. To install, just move the `risc32-unknown-elf` directory the its place. We need root permissions for this step. We will also create a symbolic link to the toolchain so we can keep multiple versions of the same tools in the system.

	$ sudo mv riscv32-unknown-elf /usr/local
	$ cd /usr/local/riscv32-unknown-elf
	$ sudo ln -s gcc-8.3.0 gcc

One last step is to update the PATH environment variable. This can be done in the current terminal or we can update the `.bashrc` script found in the home directory of the current user.

	$ cd ~
	$ echo "export PATH=$PATH:/usr/local/riscv32-unknown-elf/gcc/bin" >> .bashrc
	$ source .bashrc

### Test it!

To test the toolchain,

	$ riscv32-unknown-elf-gcc -v

	Using built-in specs.
	COLLECT_GCC=riscv32-unknown-elf-gcc	COLLECT_LTO_WRAPPER=/usr/local/riscv32-unknown-elf/gcc-8.3.0/bin/../libexec/gcc/riscv32-unknown-elf/8.3.0/lto-wrapper
	Target: riscv32-unknown-elf
	Configured with: /home/sergio/Downloads/tools/riscv-toolchain/source/gcc-8.3.0/configure --prefix=/home/sergio/Downloads/tools/riscv-toolchain/riscv32-unknown-elf/gcc-8.3.0 --target=riscv32-unknown-elf --disable-shared --disable-threads --enable-tls --enable-languages=c,c++ --with-newlib --disable-libmudflap --disable-libssp --disable-libquadmath --disable-libgomp --disable-nls --disable-multilibs
	Thread model: single
	gcc version 8.3.0 (GCC)

If something along this lines is printed, we are all set.
