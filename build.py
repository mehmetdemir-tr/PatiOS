import platform, build_func, os
from colorama import Fore

if platform.system() == "Linux":
    print(Fore.RED + "KedyBox - PatiOS ENV Builder by mehmetdemir-tr")
    
    build_func.root_control()
    askquest = input("Do you want to create the Disk again? (Y/N) >> ")
    if askquest == "Y":
        build_func.create_disk()
    build_func.extract()
    build_func.compile()
    build_func.create_cmd_symlinks()
    build_func.create_rootfs_etc()
    build_func.create_nodes()
    build_func.move()
    build_func.create_system_partition()
    build_func.compile_initramfs_init()
    build_func.build_initramfs()
else:
    print("You cannot run this script on your system. Please make sure you are using a Linux-based (like Ubuntu or WSL) operating system")
