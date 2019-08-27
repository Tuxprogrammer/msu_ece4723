# ESOS Development Environment self-setup

This is a collection of system provisioning scripts to deploy and configure a Ubuntu 16.04 image with
the required versions of MPLAB X IDE and the Microchip XC16 compiler

# Instructions

1. First examine the `Vagrantfile` and note the configurable areas. This file should be customized
by you according to your system specifications to ensure a fluid development experience.
2. After configuring the `Vagrantfile`, install the required plugin dependencies with this command:
```
vagrant plugin install vagrant-disksize
```
3. Run the following command. **NOTE: This command is creating an entire virtual operating system
and installing multiple software packages. As a result, this will take > 20 mins on some systems. Go
grab a coffee while you wait.
```
vagrant up
```
4. Open Virtualbox up and look at the VM that was added. Click the "show" button to connect to the
GUI. Login to the VM with the following credentials:
```
user: esos
pass: esos
```
Your development environment is now fully configured. Happy Coding


# Troubleshooting
## Restore environment
If at any point you want to restart the process, run these commands
```
git checkout .
vagrant destroy
```
That will reset the Vagrantfile and remove the ESOS development environment from your system.
## VBoxManage: error: Implementation of the USB 2.0 controller not found!
If you get an error like this:
```
There was an error while executing `VBoxManage`, a CLI used by Vagrant
for controlling VirtualBox. The command and stderr is shown below.

Command: ["startvm", "8cfe2db2-7268-4661-b351-13726414fc24", "--type", "headless"]

Stderr: VBoxManage: error: Implementation of the USB 2.0 controller not found!
VBoxManage: error: Because the USB 2.0 controller state is part of the saved VM state, the VM cannot be started. To fix this problem, either install the 'Oracle VM VirtualBox Extension Pack' or disable USB 2.0 support in the VM settings.
VBoxManage: error: Note! This error could also mean that an incompatible version of the 'Oracle VM VirtualBox Extension Pack' is installed (VERR_NOT_FOUND)
VBoxManage: error: Details: code NS_ERROR_FAILURE (0x80004005), component ConsoleWrap, interface IConsole
```
To fix this, you need to download the extension-pack from Oracle's documents: [https://www.virtualbox.org/manual/ch01.html#intro-installing](https://www.virtualbox.org/manual/ch01.html#intro-installing)

On Linux hosts, you must add your user to the `vboxusers` group and logout or reboot:
```
sudo adduser $USER vboxusers
```