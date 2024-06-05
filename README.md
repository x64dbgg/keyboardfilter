# keyboardfilter
The Keyboard Filter Driver is a Windows kernel-level driver designed to intercept keyboard inputs and provide customizable handling for key events. Created by me purely because of boredom.
Features

    Efficient Keyboard Input Interception: Utilizing advanced techniques in kernel-level programming, this driver intercepts keyboard inputs with minimal overhead, ensuring smooth performance and responsiveness.

    Customizable Key Event Handling: Tailor key event handling to suit your needs with customizable actions for specific key presses. Whether you're implementing custom keyboard shortcuts, enhancing security with keylogging functionality, or enforcing user restrictions by blocking specific keys, this driver offers unparalleled flexibility.

    Robust Debugging Support: Comprehensive debug output facilitates easy troubleshooting and debugging, ensuring a smooth development and deployment process.

Installation and Usage

    Building the Driver: Compile the provided source code (KeyboardFilter.c) using Visual Studio with the Windows Driver Kit to create the driver (KeyboardFilter.sys).

    Driver Installation:
        Copy KeyboardFilter.sys to the target system.
        Install the driver using the provided INF file (KeyboardFilter.inf):
            Open Device Manager.
            Right-click on your computer name and select "Add legacy hardware".
            Choose "Install the hardware that I manually select from a list".
            Select "Show All Devices" and then "Have Disk".
            Browse to the directory containing KeyboardFilter.inf and complete the installation.

    Starting the Driver:
        Open a command prompt as an administrator.
        Use the command sc start KeyboardFilter to start the driver.

Follow me :)
