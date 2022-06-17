# Ramses_Onboarding

1. Use gdbus-codegen tool for generating DBUS stubs with the following command:
gdbus-codegen --generate-c-code app-generated --c-namespace training_application --interface-prefix org.gtk.GDBus.TestInterface alarmClock.xml

2. Instal d-feet interface to simulate the client side and test the DBus Server application

3. Make a org.gtk.GDBus.conf(name it as the interface) file in the following path: /usr/share/dbus-1/system.d, 
in order to set the permissions for the created DBus System. The content of the file:

   <?xml version="1.0" encoding="UTF-8"?>
   <!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-BUS Bus Configuration 1.0//EN"
    "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
   <busconfig>
     <!-- Only my-service-user can own the service -->
     <policy user="anita">
       <allow own="org.gtk.GDBus"/>
     </policy>

     <!-- Anyone can send messages to the service -->
     <policy context="default">
       <allow send_destination="org.gtk.GDBus"/>
     </policy>
   </busconfig>


DBUS Proxy Client library(.so) generation for Server - Client communication:

Run make for the makefile from /lib directory and after generating the lib_dbus_client.so library in /bin directory, follow this steps:
  1. Copy lib_dbus_client.so library in /usr/lib path 
  2. Chmod 755 /usr/lib/lib_dbus_client.so
  3. Now the file is in a standard location, with correct permissions, readable by everybody. 
  We need to tell the loader it is available for use, so let us update the cache with this command: 
       ldconfig
  4.That should create a link to our shared library and update the cache so it is available for immediate use. Let us double check:
       ldconfig -p | grep lib_dbus_client
       lib_dbus_client.so (libc6) => /usr/lib/lib_dbus_client.so

       